#ifndef UTILS_H
#define UTILS_H

#include <cstdint>


struct Out {
    int16_t out_l, out_r;
};

struct Panning {
    int16_t pan_l, pan_r;
};

bool bernoulli_draw(int8_t probability);
uint8_t pot_map(uint16_t input, uint8_t range = 3);
uint8_t snd_random(uint8_t value, uint8_t random_pattern_nr, uint8_t random_pattern_index, uint8_t randomness);

#endif