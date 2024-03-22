#ifndef RHYTHMIC_PATTERN_H
#define RHYTHMIC_PATTERN_H

#include <cstdint>

int8_t drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step);
int8_t chance_drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step);
extern const bool rhythms[][16];
extern const uint8_t patterns[][16];

#endif // RHYTHMIC_PATTERN_H