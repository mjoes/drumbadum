#include <cstdint>

#ifndef GLOBAL_H
#define GLOBAL_H

// Declare the global variable
extern volatile bool reset_step_sample;
extern volatile bool run;
extern volatile uint16_t step_sample;
extern volatile uint8_t step;
extern uint8_t bpm_source[3];

extern uint8_t pot_seq_1;
extern uint8_t pot_seq_2;
extern uint8_t pot_seq_3;
extern uint8_t pot_seq_rd;
extern uint8_t pot_seq_art;
extern uint8_t pot_seq_turing;
extern uint8_t pot_snd_1;
extern uint8_t pot_snd_2;
extern uint8_t pot_snd_bd;
extern uint8_t pot_snd_hh;
extern uint8_t pot_snd_fm;
extern uint8_t pot_xtra;
extern uint8_t pot_bpm;
extern uint8_t pot_volume;
extern uint8_t pot_steps;
extern uint8_t pot_vol_bd;
extern uint8_t pot_vol_fm;
extern uint8_t pot_vol_hh;

#endif // GLOBAL_H
