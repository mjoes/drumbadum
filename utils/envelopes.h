#ifndef ENVELOPES_H
#define ENVELOPES_H

#include <cstdint>

extern const uint16_t log_env[];
extern const uint16_t exp_env[];
extern const uint16_t lambda[];
extern const int16_t sine[];
extern const int16_t cosine[];
extern const int16_t env_overdrive[];
extern const int16_t beta_inv[];

uint16_t interpolate_env(uint32_t rel_pos_, uint32_t length_decay_, const uint16_t* lookup_table_);
void interpolate_env_alt(int32_t* sample, uint32_t rel_pos_, uint32_t length_decay_, const uint16_t* lookup_table_);

#endif // ENVELOPES_H