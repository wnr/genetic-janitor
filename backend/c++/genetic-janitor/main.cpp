#ifdef _OPENMP
   #include <omp.h>
#endif

#include <iostream>
#include <chrono>
#include <math.h>
#include <array>
#include "game.h"
#include "genetic.h"

using namespace std;
using namespace game;
using namespace genetic;

const int ACTION_GO_LEFT    = 0;
const int ACTION_GO_UP      = 1;
const int ACTION_GO_RIGHT   = 2;
const int ACTION_GO_DOWN    = 3;
const int ACTION_PICKUP     = 4;

const int NUM_MAX_ACTIONS           = 100;
const int NUM_CELL_VALUES           = 3;
const int NUM_GENERATIONS           = 10;
const int NUM_MAPS                  = 100;
const int POPULATION_SIZE           = 200;
const float RANDOM_POPULATION_SIZE  = 0.05f;
const float MUTATION_PROBABILITY    = 0.3f;
const float MUTATION_SIZE           = 0.02f;

long getStateValue(const GameState& gameState, int numCellValues) {
    long state = 0;
    int count = 0;

    const Map &map = gameState.map;
    int x = gameState.playerX;
    int y = gameState.playerY;
    bool includeDiagonals = false;

    std::array<Cell, 5> cells;

    getStateCells(gameState.map, gameState.playerX, gameState.playerY, false, cells.data(), 0);

    for (int i = 0; i < cells.size(); i++) {
        state += cells[i] * pow(numCellValues, count);
        count++;
    }

    return state;
}

bool isGameOver(const GameState& gameState, int actionsLimit) {
    return gameState.score == gameState.maxScore || gameState.numActionsPerformed >= actionsLimit;
}

void performAction(GameState& gameState, int action) {
    switch (action) {
        case ACTION_GO_LEFT: move(gameState, -1, 0); break;
        case ACTION_GO_UP: move(gameState, 0, -1); break;
        case ACTION_GO_DOWN: move(gameState, 0, 1); break;
        case ACTION_GO_RIGHT: move(gameState, 1, 0); break;
        case ACTION_PICKUP: pickup(gameState); break;
    }
}

long getFitness(const GameState& gameState) {
    return gameState.score * 10 - gameState.numActionsPerformed;
}

long playIndividual(GameState &gameState, const Individual& individual) {
    return play(individual,
         std::function<bool(void)>(std::bind(isGameOver, std::ref(gameState), NUM_MAX_ACTIONS)),
         std::function<long(void)>(std::bind(getStateValue, std::ref(gameState), NUM_CELL_VALUES)),
         std::function<void(int)>(std::bind(performAction, std::ref(gameState), std::placeholders::_1)),
         std::function<long(void)>(std::bind(getFitness, std::ref(gameState))));
}

std::string formatThousandSeparator(long number) {
    std::string str = to_string(number);

    for (long i = (long)str.length() - 3; i > 0; i -= 3) {
        str.insert(i, " ");
    }

    return str;
}

int main() {
    const vector<int> ACTIONS = {ACTION_GO_LEFT, ACTION_GO_UP, ACTION_GO_RIGHT, ACTION_GO_DOWN, ACTION_PICKUP};
    const long NUM_STATES = (const long)pow(NUM_CELL_VALUES, ACTIONS.size());

    Genome winningGenome;
    RandomUtil randomUtil;
    std::function<Genome(void)> createGenome = std::bind(createRandomGenome, randomUtil, NUM_STATES, ACTIONS);

    Population population = createPopulation(POPULATION_SIZE, createGenome);

    auto start = std::chrono::steady_clock::now();

    std::vector<Map> maps;
    int numAvergeJunk = 0;
    for (int i = 0; i < NUM_MAPS; i++) {
        auto map = createMap(randomUtil, 10, (float)1/3);
        numAvergeJunk += game::getNumCellsOfType(map, CELL_TYPE_JUNK);
        maps.push_back(map);
    }
    numAvergeJunk /= maps.size();

    #ifdef _OPENMP
    std::cout << "Running on " << omp_get_max_threads() << " threads :D" << endl;
    #else
    std::cout << "Running sequentially :(" << endl;
    #endif

    for (int i = 0; i < NUM_GENERATIONS; i++) {
        auto results = playAll(population, NUM_MAPS, [&maps](const Individual& individual, int mapIndex){
            GameState gameState = createGame(maps[mapIndex], 1, 1);
            return playIndividual(gameState, individual);
        });

        {
            long fitness = 0;
            for (int j = 0; j < results.size(); j++) {
                PlayResult result = results[j];
                fitness += result.fitness;
            }
            fitness /= (float)results.size();

            std::cout << "Generation: " << i << "/" << NUM_GENERATIONS << endl;
            std::cout << "Average fitness: " << fitness << endl;
            std::cout << "Best fitness:    " << results[0].fitness << "/" << numAvergeJunk * 10 - NUM_MAX_ACTIONS << endl;
            std::cout << endl;
        }

        winningGenome = population[results[0].individual].genome;

        Population selected = select(randomUtil, population, results, 0.1);

        population = breed(randomUtil, selected, POPULATION_SIZE, RANDOM_POPULATION_SIZE, MUTATION_PROBABILITY, MUTATION_SIZE, createGenome, ACTIONS);
    }


    auto end = chrono::steady_clock::now();
    auto diff = end - start;
    cout << "Best genome: " + toString(winningGenome) << endl;
    cout << "Runtime: " << formatThousandSeparator(chrono::duration <double, milli> (diff).count()) << " ms" << endl;

    return 0;
}