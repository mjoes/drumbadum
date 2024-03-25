#include "rhythmic_pattern.h"
#include "../utils/utils.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step, int16_t* hits) {
    // Probability of hits based on difference between pattern 1 and 2 is not equally distributed:
    // 4/9 for kick
    // 3/9 for fm hit
    // 2/9 for hihat 

    // for (int i = 0; i < 3; ++i) {
    //     cout << hits[i] << " ";
    // }
    // cout << "\n";
    uint8_t hit_1 = patterns[knob_1][step];
    uint8_t hit_2 = patterns[2 - knob_2][step];
    // printf("%i, %i, %i, %i\n",patterns[knob_1][step], patterns[2 - knob_2][step],patterns[knob_1][step] * 3 / 100,patterns[2 - knob_2][step] * 3 / 100);
    int16_t output = hit_1 * 3 / 100 - hit_2 * 3 / 100;
    hits[abs(output)] = 1;
    if (bernoulli_draw(patterns[knob_1][step])) { 
        hits[hit_2 * 3 / 100] = 1;
    }
    cout << "Drum hits: ";
    for (int i = 0; i < 3; ++i) {
        cout << hits[i] << " ";
    }
    cout << "\n";
}

void chance_drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t knob_rd, uint8_t step, int16_t* hits) {
    uint8_t prob = patterns[knob_1][step];
    if (rand() % 100 < prob - 10) { // tweak the prob-10 for how bare bones the beat should be
        uint8_t hit = patterns[knob_2][step] / 5;
        uint8_t output = prob_hat[hit]; // Bias to hat in spite of FM hit
        if (rand() % 100 < knob_rd) { 
            output += rand() % 3;
            output %= 3;
        }
        hits[output] = 1;
        
        cout << "Chance hits: ";
        for (int i = 0; i < 3; ++i) {
            cout << hits[i] << " ";
        }
        cout << "\n";
    } 
}

void artifacts_hit(uint8_t knob_1, uint8_t knob_rd, uint8_t knob_art, uint8_t step, int16_t* hits) {
    if (rand() % 100 < knob_art) {
        uint8_t output = patterns[knob_1][16-step] * 3 / 100;
        if (rand() % 100 < knob_rd) { 
            output += rand() % 3;
            output %= 3;
        }
        hits[output] = 1;
    }
}

const uint8_t prob_hat[20] {
    0, 0, 0, 0, 0, 
    1, 1, 1, 1, 1,
    2, 2, 2, 2, 2,
    2, 2, 2, 2, 2
};

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
