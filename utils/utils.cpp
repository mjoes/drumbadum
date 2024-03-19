#include "utils.h"
#include <random>

uint16_t cv_uniform(uint16_t lower, uint16_t upper) {
    uint16_t range = upper - lower + 1;
    return rand() % range + lower;
}

bool bernoulli_draw(int8_t probability) {
    uint8_t random_number = rand() % 100;
    return random_number < probability;
}