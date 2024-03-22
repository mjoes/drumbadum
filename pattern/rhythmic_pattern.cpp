#include "rhythmic_pattern.h"
#include <iostream>
#include <cstdlib>

int8_t drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step) {
    int8_t output = patterns[knob_1][step] - patterns[2 - knob_2][step];
    return abs(output);
}

const bool clave_pat[] = {
    1, 0, 0, 1, 
    0, 0, 1, 0, 
    0, 0, 1, 0, 
    1, 0, 0, 0 
};

const uint8_t patterns[3][16] {
    {
        0, 1, 2, 0, 
        2, 0, 2, 1, 
        0, 2, 2, 0, 
        1, 1, 0, 2 
    },
    {
        1, 0, 1, 1,
        2, 1, 2, 0,
        0, 1, 1, 1,
        1, 0, 2, 1
    },
    {
        0, 0, 0, 2,
        0, 0, 1, 0,
        0, 1, 2, 0,
        2, 2, 0, 0
    }
};
