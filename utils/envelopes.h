#ifndef ENVELOPES_H
#define ENVELOPES_H

#include <cstdint>

extern const uint16_t log_env[];
extern const uint16_t exp_env[];
extern const uint16_t lambda[];
extern const int16_t sine[];
extern const int16_t env_overdrive[];

uint16_t interpolate_env(uint32_t rel_pos_, uint32_t length_decay_, const uint16_t* lookup_table_);
float interpolate_env_old(uint32_t rel_pos_, uint32_t length_decay_, const uint16_t* lookup_table_);


#endif // ENVELOPES_H