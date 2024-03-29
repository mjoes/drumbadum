#include "utils.h"
#include <random>

uint16_t cv_uniform(uint16_t lower, uint16_t upper) {
    uint16_t range = upper - lower + 1;
    return rand() % range + lower;
}

uint8_t pot_map(uint16_t input, uint8_t range) {
    uint8_t mapped_val = input * range / 1000;
    return mapped_val;
}

bool bernoulli_draw(int8_t probability) {
    uint8_t random_number = rand() % 100;
    return random_number < probability;
}