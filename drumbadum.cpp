#include <iostream>
#include <fstream>
#include <plot.h>
#include "drums/bass_drum.h"
#include "drums/hi_hat.h"
#include "drums/snare.h"
#include "drums/fm_hit.h"
#include <random>

using namespace std;

enum {ARG_NAME,ARG_DUR,ARG_NARGS};

int main(int argc, char** argv) {
    // Define parameters for the waveform
    const uint16_t duration = atoi(argv[ARG_DUR]);
    const uint16_t sample_rate = 48000;
    uint32_t num_samples = duration * sample_rate; // Number of samples (assuming 1 second at 48kHz)
    int16_t samples[num_samples] = {0};
    bool t_BD;
    bool t_HH;
    bool t_SD;
    bool t_FM;
    uint16_t trig_BD[2] = {0, 60000}; // Dummy triggers sample nr
    uint16_t trig_HH[2] = {30000, 60000}; // Dummy triggers sample nr
    uint16_t trig_FM[2] = {30000, 60000}; // Dummy triggers sample nr
    uint32_t trig_SD[2] = {20000, 70000}; // Dummy triggers sample nr

    // Initialize and define BassDrum & HiHat processor
    HiHat hi_hat(sample_rate);
    BassDrum bass_drum(sample_rate);
    FmHit fm(sample_rate);
    SnareDrum snare_drum;

    // Generate waveform samples and store them in a buffer
    for (size_t i = 0; i < num_samples; ++i) {
        // Check if trigger is hit
        for(int j = 0; j < static_cast<int>(sizeof(trig_BD) / sizeof(trig_BD[0])); j++){
            if (trig_BD[j] == i){
                t_BD = 1;
            }
        }
        for(int j = 0; j < static_cast<int>(sizeof(trig_HH) / sizeof(trig_HH[0])); j++){
            if (trig_HH[j] == i){
                t_HH = 1;
            }
        }
        for(int j = 0; j < static_cast<int>(sizeof(trig_SD) / sizeof(trig_SD[0])); j++){
            if (trig_SD[j] == i){
                t_SD = 1;
            }
        }
        for(int j = 0; j < static_cast<int>(sizeof(trig_FM) / sizeof(trig_FM[0])); j++){
            if (trig_FM[j] == i){
                t_FM = 1;
            }
        }
        // Generate waveform sample
        // if (t_BD == 1) {
        //     bass_drum.set_frequency(40);
        //     bass_drum.set_envelope(100);  // range 1-1000
        //     bass_drum.set_overdrive(10); // range 1-1000
        //     bass_drum.set_harmonics(200); // range 1-1000
        //     bass_drum.set_velocity(1000); // range 1-1000
        //     bass_drum.set_decay(800);     // range 1-1000
        //     bass_drum.set_attack(950);      // range 1-1000
        //     bass_drum.set_start();
        // }
        // if (t_HH == 1) {
        //     hi_hat.set_decay(200,0);
        //     hi_hat.set_frequency(5000);
        //     hi_hat.set_bandwidth(1000);
        //     hi_hat.set_start();
        // }
        // if (t_SD == 1) {
        //     snare_drum.Init(sample_rate);
        //     snare_drum.set_frequency(frequency*2);
        //     snare_drum.set_decay(decay);
        //     snare_drum.set_start(i);
        // }
        if (t_FM == 1) {
            fm.set_decay(200,0);
            fm.set_frequency(5000);
            fm.set_bandwidth(1000);
            fm.set_start();
        }
        samples[i] = (bass_drum.Process() + hi_hat.Process() + fm.Process() + snare_drum.Process(i))/3;
        
        t_HH = 0;
        t_BD = 0;
        t_SD = 0;
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
