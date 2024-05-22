#include <iostream>
#include <fstream>
#include <plot.h>
#include "drums/bass_drum.h"
#include "drums/hi_hat.h"
#include "drums/fm_hit.h"
#include "drums/fx.h"
#include "rhythmic_pattern.h"
#include "utils.h"
#include <random>
// #include <cstring>

using namespace std;

int main(int argc, char** argv) {
    random_device rd{};
    minstd_rand gen{rd()};
    int16_t hits[3] = { 0, 0, 0};
    int16_t seq_buffer[3][16] = {0};

    // Input params
    uint8_t pot_seq_1 = pot_map(300,5);
    uint8_t pot_seq_2 = pot_map(600,50);
    uint8_t pot_seq_3 = pot_map(400,50);
    uint8_t pot_seq_rd = pot_map(100,100);
    uint8_t pot_seq_art = pot_map(400,100);
    uint8_t pot_seq_turing = pot_map(500,100);
    uint8_t pot_snd_1 = pot_map(300,50);
    uint8_t pot_snd_2 = 50 - pot_map(600,50);
    uint8_t pot_snd_bd = pot_map(200,100);
    uint8_t pot_snd_hh = pot_map(900,100);
    uint8_t pot_snd_fm = pot_map(200,100);
    const uint16_t duration = 10;
    const uint8_t bpm = 130;

    // uint8_t shift_ = 14;
    // uint16_t pos_ = (43231 << shift_) / 33123;
    // printf("pos is %d, %f\n", (pos_ >> shift_), ((double)(pos_ & ((1 << shift_) - 1)) / (1 << shift_)));

    // Init variables
    const uint16_t sample_rate = 48000;
    uint32_t num_samples = duration * sample_rate; // Number of samples (assuming 1 second at 48kHz)
    int16_t left_samples[num_samples] = {0};
    int16_t right_samples[num_samples] = {0};
    int16_t out_l, out_r;
    Out bass_drum_out;
    Out hi_hat_out;
    Out fm_out;
    srand(time(NULL));

    // Initialize and define BassDrum & HiHat processor
    HiHat hi_hat(sample_rate, gen);
    BassDrum bass_drum(sample_rate, gen);
    FmHit fm(sample_rate, gen);
    FX fx(sample_rate, gen);

    // Initialize sequencer
    uint8_t steps = 16; // 8, 16 or 32
    uint32_t bar_sample = (60 * sample_rate * 4) / (bpm);
    uint16_t steps_sample = bar_sample / steps;
    // uint16_t glitch_sample = steps_sample / 4 + 1;
    // uint16_t beat_sample = bar_sample / 4;

    // Generate waveform samples and store them in a buffer
    uint8_t step = 0;
    uint16_t step_sample = 0;
    uint8_t glitch = 0;
    bool accent = false;
    for (size_t i = 0; i < num_samples; ++i) {
        // Check if trigger is hit
        // disabling glitch
        // if (step_sample % glitch_sample == 0 && glitch / 10 > 0) {
        //     hits[glitch % 10]=1;
        //     glitch -= 10;
        // }
        if (step_sample == steps_sample){
            if (pot_seq_turing < 20 || pot_seq_turing > 80 ) {
                for (int i = 0; i < 3; ++i) {
                    hits[i] = seq_buffer[i][step]; // Access each element using array subscript notation
                }
            } else {
                if (rhythms[pot_seq_1][step] == true){
                    drum_hit(pot_seq_2,pot_seq_3,step, hits);
                    accent = true;
                } 
                else {
                    chance_drum_hit(pot_seq_2, pot_seq_3, pot_seq_rd, step, hits);
                    accent = false;
                }
                glitch = artifacts_hit(pot_seq_2, pot_seq_rd, pot_seq_art, step, hits);

                for (int i = 0; i < 3; ++i) {
                    seq_buffer[i][step] = hits[i];
                }
            }
            step_sample = 0;
            ++step;
            if (step > 15) {
                step = 0;
            }
        }
        ++step_sample;

        // Generate waveform sample
        if (hits[0] == 1) {
            fm.set_start(pot_snd_1, pot_snd_2, pot_snd_fm, accent);
        }
        if (hits[1] == 1) {
            bass_drum.set_start(pot_snd_1, pot_snd_2, pot_snd_bd, accent);
        }
        if (hits[2] == 1) {
            hi_hat.set_start(pot_snd_1, pot_snd_2, pot_snd_hh, accent);
        }

        bass_drum_out = bass_drum.Process();
        hi_hat_out = hi_hat.Process();
        fm_out = fm.Process();

        out_l = (bass_drum_out.out_l + hi_hat_out.out_l + fm_out.out_l)/3;
        out_r = (bass_drum_out.out_r + hi_hat_out.out_r + fm_out.out_r)/3;

        fx.Process(&left_samples[i], &right_samples[i], &out_l, &out_r);

        for (int i = 0; i < 3; ++i) {
            hits[i] = 0; // Access each element using array subscript notation
        }
    }

    // Write buffer to a raw file
    ofstream raw_file("output.raw", ios::out | ios::binary);
    if (!raw_file.is_open()) {
        cerr << "Error: Unable to open file for writing!" << endl;
        return 1;
    }
    for (size_t i = 0; i < num_samples; ++i) {
        raw_file.write(reinterpret_cast<const char*>(&left_samples[i]), sizeof(int16_t));
        raw_file.write(reinterpret_cast<const char*>(&right_samples[i]), sizeof(int16_t));
    }
    raw_file.close();
    cout << "Waveform written to 'output.raw'" << endl;

    // Plot buffer
    // ofstream dataFile("output.txt", std::ofstream::out | std::ofstream::trunc);
    // for (uint32_t i = 0; i < num_samples; ++i) {
    //     dataFile << static_cast<float>(i)/sample_rate << " " << samples[i]/32767.0f << "\n";
    // }
    // cout << "Data written to output.txt" << endl;
    // dataFile.close();

    // return 0;
}

