#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <random>
#include "envelopes.h"
#include "utils.h"
#include "rhythmic_pattern.h"

using namespace std;

struct BassDrumSculpt {
    uint16_t frequency_, period_, velocity_, overdrive_, harmonics_, harmonics_f0, harmonics_f1, harmonics_f2;
    uint32_t length_decay_;
    uint8_t decay_;
};

class BassDrum {
public:
    BassDrum(
        uint16_t sample_rate,
        minstd_rand& gen) 
        : 
        max_bit_(4294967295),
        sample_rate_(sample_rate),
        flutter_(3),
        bitsSine(10),
        gen_(gen)
        {
            rel_pos_ = 0; 
            // set_attack(0);
        }
    ~BassDrum() {}

    void set_pattern(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        if (accent == true) {
            randomness = 0;
        }
        set_frequency(snd_random(patterns[pattern_nr][1],random_pattern_nr,1,randomness));
        BD.overdrive_ = (snd_random(patterns[pattern_nr][2],random_pattern_nr,2,randomness) << 8) / 30 + (1 << 8); 
        set_harmonics((snd_random(patterns[pattern_nr][3],random_pattern_nr,3,randomness) * 10) << 8);
        BD.length_decay_ = (snd_random(patterns[pattern_nr][4],random_pattern_nr,4,randomness) * 10) * sample_rate_ / 400;
        set_envelope(snd_random(patterns[pattern_nr][5],random_pattern_nr,5,randomness)); // Envelope needed fixing, but cannot use current solution
    }

    void set_frequency(uint16_t frequency) {
        BD.frequency_ = (frequency * 60 / 100) + 25;
        BD.period_ = sample_rate_ / BD.frequency_;
    }

    void set_overdrive(uint16_t overdrive) {
        BD.overdrive_ = (overdrive << 4) / 200 + (1 << 4); 
    }

    void set_velocity(uint16_t velocity, bool accent) {
        if (accent == true) {
            BD.velocity_ = 1000;
        } else {
            BD.velocity_ = velocity;
        }
    }

    void set_decay(uint16_t decay) {
        BD.length_decay_ = decay * sample_rate_ / 400;
    }

    // UNUSED
    // void set_attack(uint16_t attack) {
    //     BD.attack_ = 1001 - attack;
    //     length_attack_ = sample_rate_ / BD.attack_;
    // }

    void set_harmonics(uint16_t harmonics) {
        BD.harmonics_ = harmonics / 1000;
        BD.harmonics_f0 = 220 + flutter_[0]/125;
        BD.harmonics_f1 = BD.frequency_ * 78 / 100 + flutter_[1]/125;
        BD.harmonics_f2 = BD.frequency_ * 102 / 100 + flutter_[2]/125;
    }

    void set_envelope(uint16_t envelope) {
        if (envelope < 50){
            envelope = 50;
        }
        uint16_t f0_ = BD.frequency_ * envelope / 50; // Aimed at a kick drum range, might want to play around with this
        d_freq_ = ((f0_ - BD.frequency_) << 15) * 20 / sample_rate_; // the number 20 can be tuned for length of decay
        inst_freq_ = (f0_ << 15);
    }

    void set_start(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        rel_pos_ = 0;
        phase_acc = 0;
        running_ = true;
        
        for (int i = 0; i < 3; ++i) {
            flutter_[i] = d(gen_);
        }
        set_velocity(500, accent);
        set_pattern(pattern_nr, random_pattern_nr, randomness, accent);
        end_i_ = BD.length_decay_;
    }

    Out Process() {
        // Generate waveform sample
        if (running_ == false) {
            out.out_l = 0;
            out.out_r = 0;
            return out;
        }
        int32_t sample;
        sample = GenerateSample();
        // sample += GenerateHarmonics();
        sample = (sample * BD.velocity_) >> 10; //  /1024
        interpolate_env_alt(&sample, rel_pos_, BD.length_decay_, exp_env); 
        int16_t output = Overdrive(sample);
        
        rel_pos_ += 1;

        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        
        out.out_l = output;
        out.out_r = output;
        return out;          
    }

private:
    uint32_t rel_pos_, end_i_, phase_acc, tW_, d_freq_, inst_freq_;
    const uint32_t max_bit_;
    const uint16_t sample_rate_;
    const uint16_t* lookup_table_;
    vector<int16_t> flutter_; 
    bool running_;
    const uint8_t bitsSine;
    minstd_rand& gen_;
    normal_distribution<double> d{0, 1000};
    BassDrumSculpt BD;
    Out out;

    int16_t Overdrive(int32_t value) {
        int16_t clipped_value;
        int32_t overdriven_value = (value * BD.overdrive_) >> 8;
        if (overdriven_value <= -32768) {
            clipped_value = -32768;
        } 
        else if (overdriven_value >= 32767) {
            clipped_value = 32767;
        } 
        else {
            uint32_t scaled_ov = overdriven_value + (1 << 15);
            scaled_ov <<= 7;
            uint32_t int_pos = (scaled_ov) >> 15;
            int16_t a = env_overdrive[int_pos];
            int16_t b = env_overdrive[int_pos + 1];
            clipped_value = a + ((b - a) * (scaled_ov & ((1 << 15) - 1)) >> 15);
        }
        // HARD CLIPPING
        // clipped_value = 0.5 * (fabs(overdriven_value + 32767) - fabs(overdriven_value - 32767));
        return clipped_value;
    }
            
    int16_t GenerateSample() {
        // uint16_t pos_ = (rel_pos_ << 15) / BD.period_;
        // uint16_t int_pos = (pos_ & ((1 << 15) - 1)) * 1024 / (1 << 15);
        // int16_t a = sine[int_pos];
        // int16_t b = sine[int_pos + 1];
        // int16_t sample = a + (b - a) * (pos_ & ((1 << 15) - 1) >> 15);

        if ((inst_freq_ >> 15) >= BD.frequency_) {
            inst_freq_ = inst_freq_ - d_freq_; 
            // printf("%i, %i \n", (d_freq_ >> 15), (inst_freq_ >> 15));
        }
        tW_ = max_bit_ / sample_rate_ * (inst_freq_ >> 15);
        phase_acc += tW_;
        uint16_t phase_inc = phase_acc >> (32 - bitsSine);
        int32_t fraction_fp = (phase_acc & ((1 << (32 - bitsSine)) - 1));
        int16_t a = sine[phase_inc];
        int16_t b = sine[phase_inc + 1];
        int16_t sample = a + ((b - a) * (fraction_fp) >> 22);

        return sample;
    }

    // CODE FOR HARMONICS, DISABLED NOW
    // inline int16_t LookupHarmonics(uint16_t frequency, uint16_t phase_shift, uint8_t scale_factor) {
    //     uint16_t pos_ = (rel_pos_ << 15) / (sample_rate_ / frequency);
    //     uint16_t int_pos = (phase_shift + ((pos_ & ((1 << 15) - 1)) * 1024 / (1 << 15))) % 1024;;
    //     int16_t a = sine[int_pos];
    //     int16_t b = sine[int_pos + 1];
    //     int16_t base_sample = a + (b - a) * (pos_ & ((1 << 15) - 1) >> 15);
        
    //     return base_sample / scale_factor;
    // }

    // int32_t GenerateHarmonics() {
    //     int32_t sample_harmonics;
    //     sample_harmonics = LookupHarmonics(BD.harmonics_f0, 100, 5) + LookupHarmonics(BD.harmonics_f1, 200, 3) + LookupHarmonics(BD.harmonics_f2, 300, 5);
    //     sample_harmonics *= BD.harmonics_ ;
    //     return sample_harmonics >> 8;
    // }
};
