#ifndef RHYTHMIC_PATTERN_H
#define RHYTHMIC_PATTERN_H

#include <cstdint>

void drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step, int16_t* hits);
void chance_drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step, int16_t* hits);
extern const bool rhythms[][16];
extern const uint8_t patterns[][16];

#endif // RHYTHMIC_PATTERN_H