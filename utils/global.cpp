#include "global.h"

bool volatile reset_step_sample = true;
bool volatile run = false;
uint16_t volatile step_sample = 0;
uint8_t volatile step = 0;
uint8_t bpm_source[3] = { 120, 120, 120 };

uint8_t pot_seq_1 = 2; // pot_data[6]
uint8_t pot_seq_2 = 25; // pot_data[5]
uint8_t pot_seq_3 = 25; // pot_data[7]
uint8_t pot_seq_rd = 50; // pot_data[4]
uint8_t pot_seq_art = 50;// pot_data[3]
uint8_t pot_seq_turing = 50; // pot_data[2]
uint8_t pot_snd_1 = 25; // pot_data[10]
uint8_t pot_snd_2 = 50 - 34;
uint8_t pot_snd_bd = 50; // pot_data[13]
uint8_t pot_snd_hh = 50; // pot_data[9]
uint8_t pot_snd_fm = 50; // pot_data[8]
uint8_t pot_xtra = 0; // pot_data[12]
uint8_t pot_bpm = 120; // pot_data[1]
uint8_t pot_volume = 100; // pot_data[0]

uint8_t pot_steps = 64;
uint8_t pot_vol_bd = 100;
uint8_t pot_vol_fm = 100;
uint8_t pot_vol_hh = 100;
