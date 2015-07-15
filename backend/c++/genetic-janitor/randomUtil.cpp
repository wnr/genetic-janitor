#include <stdlib.h>
#include <random>
#include "randomUtil.h"

RandomUtil::RandomUtil() : engine(std::random_device{}()) {}

bool RandomUtil::shouldHappen(float probability) {
    static std::uniform_int_distribution<short> distribution(0, 100);

    int value = distribution(engine);
    int probabilityValue = probability * 100 + 0.5;

    return probabilityValue >= value;
}

int RandomUtil::randomInt(int max) {
    std::uniform_int_distribution<int> distribution(0, max - 1);

    return distribution(engine);
}