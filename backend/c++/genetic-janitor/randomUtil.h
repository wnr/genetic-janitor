#ifndef GENETIC_JANITOR_RANDOM_H
#define GENETIC_JANITOR_RANDOM_H

#include <random>
#include <algorithm>

class RandomUtil {
private:
    std::default_random_engine engine;

public:
    RandomUtil();

    bool shouldHappen(float probability);
    int randomInt(int max);
};

#endif //GENETIC_JANITOR_RANDOM_H
