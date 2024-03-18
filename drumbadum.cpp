#include <iostream>
#include <fstream>
#include <plot.h>
#include "drums/bass_drum.h"
#include "drums/hi_hat.h"
#include "drums/fm_hit.h"
#include <random>

using namespace std;

enum {ARG_NAME,ARG_DUR,ARG_NARGS};

bool bernoulli_draw(int8_t probability) {
    uint8_t random_number = rand() % 100;
    return random_number < probability;
}

uint16_t cv_uniform(uint16_t lower = 0, uint16_t upper = 1000) {
    uint16_t range = upper - lower + 1;
    return rand() % range + lower;
}

int main(int argc, char** argv) {
    // Define parameters for the waveform
    const uint16_t duration = 10;
    const uint8_t bpm = 130;
    const uint16_t sample_rate = 48000;
    uint32_t num_samples = duration * sample_rate; // Number of samples (assuming 1 second at 48kHz)
    int16_t samples[num_samples] = {0};
    bool t_BD;
    bool t_HH;
    bool t_FM;
    srand(time(NULL));
    random_device rd{};
    mt19937 gen{rd()};

    // Initialize and define BassDrum & HiHat processor
    HiHat hi_hat(sample_rate, gen);
    BassDrum bass_drum(sample_rate, gen);
    FmHit fm(sample_rate, gen);

    // Initialize sequencer
    uint8_t steps = 16; // 8, 16 or 32
    uint16_t steps_sample = (60 * sample_rate * 4) / (bpm  * steps);
    uint16_t bar_sample = (60 * sample_rate) / (bpm  * steps * 4);

    // Generate waveform samples and store them in a buffer
    for (size_t i = 0; i < num_samples; ++i) {
        // Check if trigger is hit
        if (i % steps_sample == 0){
            t_BD = bernoulli_draw(40);
            t_HH = bernoulli_draw(80);
            t_FM = bernoulli_draw(50);
            if (i % bar_sample == 0) {
                t_BD = 1;
            }
        }
        
        // Generate waveform sample
        if (t_BD == 1) {
            bass_drum.set_frequency(40);
            bass_drum.set_envelope(50);  // range 1-1000
            bass_drum.set_overdrive(cv_uniform()); // range 1-1000
            bass_drum.set_harmonics(20); // range 1-1000
            bass_drum.set_velocity(1000); // range 1-1000
            bass_drum.set_decay(cv_uniform());     // range 1-1000
            bass_drum.set_attack(0);      // range 1-1000
            bass_drum.set_start();
        }
        if (t_HH == 1) {
            hi_hat.set_decay(cv_uniform(),bernoulli_draw(10));
            hi_hat.set_frequency(cv_uniform(8000,12000));
            hi_hat.set_velocity(1000);
            hi_hat.set_bandwidth(cv_uniform(300,3000));
            hi_hat.set_start();
        }
        if (t_FM == 1) {
            fm.set_decay(cv_uniform(),0);
            fm.set_fm_amount(cv_uniform(),0);
            fm.set_ratio(cv_uniform(150,500));
            fm.set_velocity(800);
            fm.set_frequency(74);
            fm.set_start(); 
        }
        samples[i] = (bass_drum.Process() + hi_hat.Process() + fm.Process())/3;
        
        t_HH = 0;
        t_BD = 0;
        t_FM = 0;
    }

    // Write buffer to a raw file
    ofstream raw_file("output.raw", ios::out | ios::binary);
    if (raw_file.is_open()) {
        raw_file.write(reinterpret_cast<const char*>(samples), num_samples * sizeof(int16_t));
        raw_file.close();
        cout << "Waveform written to 'bass_drum_waveform.raw'" << endl;
    } else {
        cerr << "Error: Unable to open file for writing!" << endl;
        return 1;
    }
    // Plot buffer
    ofstream dataFile("output.txt", std::ofstream::out | std::ofstream::trunc);
    for (uint32_t i = 0; i < num_samples; ++i) {
        dataFile << static_cast<float>(i)/sample_rate << " " << samples[i]/32767.0f << "\n";
    }
    cout << "Data written to output.txt" << endl;
    dataFile.close();

    return 0;
}

