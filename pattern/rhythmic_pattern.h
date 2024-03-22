#ifndef RHYTHMIC_PATTERN_H
#define RHYTHMIC_PATTERN_H

#include <cstdint>

int8_t drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step);
extern const bool clave_pat[];
extern const uint8_t patterns[][16];

#endif // RHYTHMIC_PATTERN_H