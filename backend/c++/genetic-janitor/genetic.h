#ifndef GENETIC_JANITOR_GENETIC_H
#define GENETIC_JANITOR_GENETIC_H

#include "randomUtil.h"
#include <functional>

namespace genetic {
    typedef std::vector<int> Genome;

    struct Individual {
        Genome genome;

        Individual(Genome genome) : genome(genome) {};
    };

    struct PlayResult {
        int individual;
        long fitness;

        PlayResult(int individual, long fitness) : individual(individual), fitness(fitness) {}
    };

    typedef std::vector<Individual> Population;

    Genome createRandomGenome(RandomUtil& randomUtil, long numStates, std::vector<int> actions);
    std::string toString(const Individual& individual);
    Population createPopulation(int size, std::function<Genome(void)> createGenome);
    long play(const Individual& individual, std::function<bool(void)> isGameOver, std::function<long(void)> getStateValue, std::function<void(int)> performAction, std::function<long(void)> getFitness);
    std::vector<PlayResult> playAll(const Population& population, std::function<long(const Individual&)> playIndividual);
    Population select(RandomUtil& randomUtil, const Population& population, const std::vector<PlayResult>& results, float selectPercentage);
    Population breed(RandomUtil& randomUtil, const Population& population, int newPopulationSize, float randomIndividualsPercentage, float mutationPercentage, float mutationSize, std::function<Genome(void)> createGenome, std::vector<int> possibleActions);
}

#endif //GENETIC_JANITOR_GENETIC_H
