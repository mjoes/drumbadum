#include <iostream>
#include <fstream>
#include <plot.h>
#include "drums/bass_drum.h"

using namespace std;

enum {ARG_NAME,ARG_FREQUENCY,ARG_DECAY,ARG_DUR,ARG_NARGS};

int main(int argc, char** argv) {
    // Define parameters for the waveform
    uint16_t frequency = atoi(argv[ARG_FREQUENCY]); // Example frequency
    uint16_t decay = atoi(argv[ARG_DECAY]);     // Example decay
    uint16_t duration = atoi(argv[ARG_DUR]);
    uint16_t sample_rate = 48000;
    size_t num_samples = duration * sample_rate; // Number of samples (assuming 1 second at 48kHz)
    int16_t samples[num_samples];

    // Initialize and define BassDrum processor
    BassDrum bass_drum;
    bass_drum.Init(sample_rate);
    bass_drum.set_frequency(frequency);
    bass_drum.set_decay(decay);

    // Generate waveform samples and store them in a buffer
    for (size_t i = 0; i < num_samples; ++i) {
        // Generate waveform sample
        bass_drum.Process(1, &samples[i], 1);
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
    ofstream dataFile("output.txt");
    for (size_t i = 0; i < num_samples; ++i) {
        dataFile << static_cast<float>(i)/sample_rate << " " << samples[i]/32767.0f << "\n";
    }
    cout << "Data written to output.txt" << endl;
    dataFile.close();

    return 0;
}
