#include <iostream>
#include <fstream>
#include <plot.h>
#include "drums/bass_drum.h"
#include "drums/hi_hat.h"
#include "drums/fm_hit.h"
#include "drums/fx.h"
#include "drums/sequencer.h"
#include "global.h"
#include "rhythmic_pattern.h"
#include "utils.h"
#include <random>
// #include <cstring>

using namespace std;

int main(int argc, char** argv) {
    random_device rd{};
    minstd_rand gen{rd()};

    // Input params
    const uint16_t duration = 10;
    const uint8_t bpm = 120;

    // Init variables
    const uint16_t sample_rate = 44100;
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
    Effects FX(sample_rate, gen);
    Sequencer SQ;
    

    // Initialize sequencer
    uint8_t steps = 16; // 8, 16 or 32
    uint32_t bar_sample = (60 * sample_rate * 4) / (bpm);
    uint16_t steps_sample = bar_sample / steps;

    // Init stutter
    SQ.set_stutter_samples(bar_sample);

    // Loop
    for (size_t i = 0; i < num_samples; ++i) {
        SQ.run_sequencer(true, false, steps_sample);

        // Generate waveform sample
        if (SQ.hits[0] == 1) {
            fm.set_start(pot_snd_1, pot_snd_2, pot_snd_fm, SQ.accent[0]);
        }
        if (SQ.hits[1] == 1) {
            bass_drum.set_start(pot_snd_1, pot_snd_2, pot_snd_bd, SQ.accent[1]);
        }
        if (SQ.hits[2] == 1) {
            hi_hat.set_start(pot_snd_1, pot_snd_2, pot_snd_hh, SQ.accent[2]);
        }
        if (SQ.FX_flag) {
            FX.set_start(steps_sample);
        }
        
        bass_drum_out = bass_drum.Process();
        hi_hat_out = hi_hat.Process();
        fm_out = fm.Process();

        out_l = (bass_drum_out.out_l * 10 + hi_hat_out.out_l * 20 + fm_out.out_l * 8)/30;
        out_r = (bass_drum_out.out_r * 10 + hi_hat_out.out_r * 20 + fm_out.out_r * 8)/30;
        
        FX.Process(&left_samples[i], &right_samples[i], &out_l, &out_r, pot_volume, 5);
        SQ.reset_hits();
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

