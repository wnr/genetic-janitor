#include <vector>
#include <sstream>
#include <iostream>
#include "genetic.h"

using namespace genetic;
using namespace std;

Genome genetic::createRandomGenome(RandomUtil& randomUtil, long numStates, std::vector<int> actions) {
    Genome genome((unsigned long)numStates);
    const int NUM_POSSIBLE_ACTIONS = (int)actions.size();

    for (int i = 0; i < numStates; i++) {
        int index = randomUtil.randomInt(NUM_POSSIBLE_ACTIONS);

        genome[i] = actions[index];
    }

    return genome;
}

string genetic::toString(const Individual &individual) {
    std::stringstream result;
    std::copy(individual.genome.begin(), individual.genome.end(), std::ostream_iterator<int>(result, ""));
    return result.str();
}

Population genetic::createPopulation(int size, std::function<Genome(void)> createGenome) {
    Population population;

    for (int i = 0; i < size; i++) {
        population.push_back(Individual(createGenome()));
    }

    return population;
}

long genetic::play(const Individual& individual, std::function<bool(void)> isGameOver, std::function<long(void)> getStateValue,
                   std::function<void(int)> performAction, std::function<long(void)> getFitness) {
    while (!isGameOver()) {
        long state = getStateValue();
        int action = individual.genome[state];
        performAction(action);
    }

    return getFitness();
}

std::vector<PlayResult> genetic::playAll(const Population &population, std::function<long(const Individual&)> playIndividual) {
    const int POPULATION_SIZE = (int)population.size();
    std::vector<PlayResult> playResults;

    for (int i = 0; i < POPULATION_SIZE; i++) {
        long fitness = playIndividual(population[i]);
        playResults.push_back(PlayResult(i, fitness));
    }

    std::sort(playResults.begin(), playResults.end(), [](const PlayResult& a, const PlayResult& b){
        return a.fitness > b.fitness;
    });

    return playResults;
}

struct Score {
    long score;
    int individualIndex;

    Score(long score, int individualIndex) : score(score), individualIndex(individualIndex) {}
};

Population genetic::select(RandomUtil& randomUtil, const Population &population, const std::vector<PlayResult> &results, float selectPercentage) {
    const long NUM_RESULTS = results.size();

    long lowestFitness = results[NUM_RESULTS - 1].fitness;

    if (lowestFitness > 0) {
        lowestFitness = 0;
    }

    lowestFitness -= 1;

    vector<Score> selectScores;

    for (int i = 0; i < NUM_RESULTS; i++) {
        float randomValue = (float)randomUtil.randomInt(100) / 100;
        long fitness = results[i].fitness;
        long score = (long)((double)(fitness - lowestFitness) * randomValue);
        selectScores.push_back(Score(score, results[i].individual));
    }

    sort(selectScores.begin(), selectScores.end(), [](const Score& a, const Score& b){
        return a.score > b.score;
    });

    const int NUM_SELECTED = (int)(NUM_RESULTS * selectPercentage);

    Population selectedPopulation;

    for (int i = 0; i < NUM_SELECTED; i++) {
        selectedPopulation.push_back(population[selectScores[i].individualIndex]);
    }

    return selectedPopulation;
}

Individual mate(RandomUtil& randomUtil, const Individual& mom, const Individual& dad, float mutationPercentage, float mutationSize, std::vector<int> possibleActions) {
    int cutIndex = randomUtil.randomInt((int)(mom.genome.size() / 3.0f)) + (int)(mom.genome.size() / 3.0f);
    Genome childGenome(mom.genome.size());

    for (int i = 0; i < cutIndex; i++) {
        childGenome[i] = mom.genome[i];
    }

    for (int i = cutIndex; i < mom.genome.size(); i++) {
        childGenome[i] = dad.genome[i];
    }

    if (randomUtil.shouldHappen(mutationPercentage)) {
        int numMutations = (int)(mutationSize * childGenome.size());

        for (int i = 0; i < numMutations; i++) {
            childGenome[randomUtil.randomInt((int)childGenome.size())] = possibleActions[randomUtil.randomInt((int)possibleActions.size())];
        }
    }

    return Individual(childGenome);
}

Population genetic::breed(RandomUtil& randomUtil, const Population& population, int newPopulationSize, float randomIndividualsPercentage, float mutationPercentage, float mutationSize, std::function<Genome(void)> createGenome, std::vector<int> possibleActions) {
    const int NUM_RANDOM_INDIVIDUALS = (int)(randomIndividualsPercentage * newPopulationSize);

    const int POPULATION_SIZE = population.size();

    Population newPopulation;

    auto randomPopulation = createPopulation(NUM_RANDOM_INDIVIDUALS, createGenome);

    while ((newPopulation.size() + NUM_RANDOM_INDIVIDUALS + POPULATION_SIZE) < newPopulationSize) {
        Individual mom = population[randomUtil.randomInt((int)population.size())];
        Individual dad = population[randomUtil.randomInt((int)population.size())];
        Individual child = mate(randomUtil, mom, dad, mutationPercentage, mutationSize, possibleActions);
        newPopulation.push_back(child);
    }

    newPopulation.insert(newPopulation.end(), randomPopulation.begin(), randomPopulation.end());
    newPopulation.insert(newPopulation.end(), population.begin(), population.end());

    return newPopulation;
}
