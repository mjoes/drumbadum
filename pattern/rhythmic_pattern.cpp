#include "rhythmic_pattern.h"
#include "../utils/utils.h"
#include <iostream>
#include <cstdlib>


int8_t drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step) {
    uint8_t hit_1 = patterns[knob_1][step] * 3 / 100;
    uint8_t hit_2 = patterns[2 - knob_2][step] * 3 / 100;
    printf("%i, %i, %i, %i\n",patterns[knob_1][step], patterns[2 - knob_2][step],patterns[knob_1][step] * 3 / 100,patterns[2 - knob_2][step] * 3 / 100);
    int8_t output = hit_1 - hit_2;
    return abs(output);
}

int8_t chance_drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step) {
    uint8_t output;
    if (bernoulli_draw(patterns[knob_1][step])) { 
        output = patterns[knob_2][step] * 3 / 100;
    } else {
        output = 4;
    }
    return output;
}


const bool rhythms[5][16] = {
    { // son clave
        1, 0, 0, 1, 
        0, 0, 1, 0, 
        0, 0, 1, 0, 
        1, 0, 0, 0 
    },
    { // tresillo
        1, 0, 0, 1, 
        0, 0, 1, 0, 
        1, 0, 0, 1, 
        0, 0, 1, 0
    },
    { // 4x4
        1, 0, 0, 0, 
        1, 0, 0, 0, 
        1, 0, 0, 0, 
        1, 0, 0, 0 
    },
    { // rumba clave
        1, 0, 0, 1, 
        0, 0, 0, 1, 
        0, 0, 1, 0, 
        1, 0, 0, 0 
    },
    { // Morten
        1, 0, 0, 1, 
        0, 0, 1, 0, 
        0, 1, 0, 0, 
        1, 0, 1, 0 
    },
};

const uint8_t patterns[3][16] {
    {
        78, 82, 38, 66,
        85, 86, 8, 85,
        22, 81, 57, 24,
        37, 2, 2, 62
    },
    {
        84, 59, 47, 57,
        70, 44, 2, 23,
        91, 9, 41, 16,
        69, 21, 74, 56
    },
    {
        22, 80, 32, 14,
        13, 67, 77, 93,
        90, 84, 98, 54,
        35, 36, 96, 22
    }
};
