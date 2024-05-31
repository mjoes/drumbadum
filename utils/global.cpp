#include "global.h"
#include "utils.h"

bool reset_step_sample = true;
bool run = false;
bool stutter_flag = false;
uint16_t step_sample = 0;
uint8_t rxByte;
uint8_t bpm_source[3] = { 120, 120, 120 };

uint8_t pot_seq_1 = pot_map(100,5);
uint8_t pot_seq_2 = pot_map(900,50);
uint8_t pot_seq_3 = pot_map(200,50);
uint8_t pot_seq_rd = pot_map(100,100);
uint8_t pot_seq_art = pot_map(400,100);
uint8_t pot_seq_turing = pot_map(500,100);
uint8_t pot_snd_1 = pot_map(300,50);
uint8_t pot_snd_2 = 50 - pot_map(600,50);
uint8_t pot_snd_bd = pot_map(900,100);
uint8_t pot_snd_hh = pot_map(800,100);
uint8_t pot_snd_fm = pot_map(10,100);
uint8_t pot_xtra = pot_map(0,100);
uint8_t pot_volume = pot_map(1000,100);
