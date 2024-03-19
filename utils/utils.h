#ifndef UTILS_H
#define UTILS_H

#include <cstdint>

uint16_t cv_uniform(uint16_t lower = 0, uint16_t upper = 1000);
bool bernoulli_draw(int8_t probability);

#endif