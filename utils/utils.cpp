#include "utils.h"
#include "rhythmic_pattern.h"
#include <random>

uint8_t snd_random(
        uint8_t value, 
        uint8_t random_pattern_nr, 
        uint8_t random_pattern_index,
        uint8_t randomness
    ) {
        uint16_t new_prb;
        uint16_t prb = patterns[random_pattern_nr][random_pattern_index] << 5;
        if (randomness < 50) {
            new_prb = (prb / 50 * randomness);
        } else {
            new_prb = ((100 << 5) - prb) * randomness / 50 + 2 * prb - (100 << 5);
        }
        new_prb >>= 5;
        uint8_t out = (new_prb * (rand() % 101) + (100 - new_prb) * value) / 100;
        return out;
}

uint8_t pot_map(uint16_t input, uint8_t range) {
    uint8_t mapped_val = input * range / 1000;
    return mapped_val;
}

bool bernoulli_draw(int8_t probability) {
    uint8_t random_number = rand() % 100;
    return random_number < probability;
}