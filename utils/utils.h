#ifndef UTILS_H
#define UTILS_H

#include <cstdint>


struct Out {
    int16_t out_l, out_r;
};

uint16_t cv_uniform(uint16_t lower = 0, uint16_t upper = 1000);
bool bernoulli_draw(int8_t probability);
uint8_t pot_map(uint16_t input, uint8_t range = 3);
uint8_t snd_random(uint8_t value, uint8_t random_pattern_nr, uint8_t random_pattern_index, uint8_t randomness);

#endif