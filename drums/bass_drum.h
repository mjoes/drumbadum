#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <random>
#include "../utils/envelopes.h"
#include "../utils/utils.h"
#include "../pattern/rhythmic_pattern.h"

using namespace std;

struct BassDrumSculpt {
    float interval_, phase_, slope_, phase_end_;
    uint16_t frequency_, attack_, velocity_, overdrive_, harmonics_, f0_;
    uint32_t length_decay_;
    uint8_t decay_;
};

class BassDrum {
public:
    BassDrum(
        uint16_t sample_rate,
        mt19937& gen) 
        : 
        sample_rate_(sample_rate),
        flutter_(3),
        gen_(gen)
        {
            rel_pos_ = 0; 
            set_attack(0);
        }
    ~BassDrum() {}

    void set_pattern(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        if (accent == true) {
            randomness = 0;
        }
        BD.frequency_ = (snd_random(patterns[pattern_nr][1],random_pattern_nr,1,randomness) * 60 / 100) + 25;
        BD.overdrive_ = (snd_random(patterns[pattern_nr][2],random_pattern_nr,2,randomness) << 4) / 30 + (1 << 4); 
        BD.harmonics_ = ((snd_random(patterns[pattern_nr][3],random_pattern_nr,3,randomness) * 10) << 8) / 1000;
        BD.length_decay_ = (snd_random(patterns[pattern_nr][4],random_pattern_nr,4,randomness) * 10) * sample_rate_ / 400;
        set_envelope(snd_random(patterns[pattern_nr][5],random_pattern_nr,5,randomness)); // Envelope needed fixing, but cannot use current solution
    }

    void set_frequency(uint16_t frequency) {
        BD.frequency_ = frequency;
    }

    void set_overdrive(uint16_t overdrive) {
        BD.overdrive_ = (overdrive << 4) / 200 + (1 << 4); 
    }

    void set_velocity(uint16_t velocity, bool accent) {
        if (accent == true) {
            BD.velocity_ = 256;
        } else {
            BD.velocity_ = (velocity << 8) / 1000;
        }
    }

    void set_decay(uint16_t decay) {
        BD.length_decay_ = decay * sample_rate_ / 400;
    }

    void set_attack(uint16_t attack) {
        BD.attack_ = 1001 - attack;
        length_attack_ = sample_rate_ / BD.attack_;
    }

    void set_harmonics(uint16_t harmonics) {
        BD.harmonics_ = (harmonics << 8) / 1000;
    }

    void set_envelope(uint16_t envelope) {
        BD.f0_ = BD.frequency_ * envelope / 10; // Aimed at a kick drum range, might want to play around with this
        BD.interval_ = 0.05; // TODO: This needs tuning
        BD.slope_ = (BD.frequency_ - BD.f0_) / BD.interval_;
        float y_1 = sin(2 * M_PI * (BD.slope_ * BD.interval_ * BD.interval_ / 2 + BD.f0_ * BD.interval_));
        float dy_1 = 2 * M_PI * (BD.slope_ * BD.interval_ + BD.f0_) * cos(2 * M_PI * (BD.slope_ * BD.interval_ * BD.interval_ / 2 + BD.f0_ * BD.interval_));
        float phi_1 = asin(y_1);
        float phi_2 = M_PI - phi_1;
        float dz_1 = 2 * M_PI * BD.frequency_ * cos(phi_1);
        while (phi_1 < 0) { 
            phi_1 += 2*M_PI;
        }
        while (phi_2 > (2*M_PI)){
            phi_2 = 2*M_PI;
        }
        if (dy_1/abs(dy_1) == dz_1/abs(dz_1)) {
            BD.phase_end_ = phi_1;
        } else {
            BD.phase_end_ = phi_2;
        }
    }

    void set_start(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_attack_ + BD.length_decay_;

        for (int i = 0; i < 3; ++i) {
            flutter_[i] = d(gen_);
        }
        set_velocity(500, accent);
        set_pattern(pattern_nr, random_pattern_nr, randomness, accent);
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        int32_t sample;
        float t = static_cast<float>(rel_pos_) / sample_rate_;

        sample = GenerateSample(t);
        sample += GenerateHarmonics(t);
        sample = (sample * BD.velocity_) >> 8 ;
        sample *= interpolate_env(rel_pos_, BD.length_decay_, exp_env);
        int16_t output = Overdrive(sample, 1); // Apply distortion
 
        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    uint32_t rel_pos_, end_i_;
    uint16_t length_attack_;
    const uint16_t sample_rate_;
    const uint16_t* lookup_table_;
    vector<int16_t> flutter_; 
    bool running_;
    mt19937& gen_;
    normal_distribution<double> d{0, 1000};
    BassDrumSculpt BD;

    int16_t Overdrive(int32_t value, uint8_t dist_type) {
        int16_t clipped_value;
        int32_t overdriven_value = (value * BD.overdrive_) >> 4;
        float scaled_overdriven_value = overdriven_value / 32767.0;
        switch (dist_type){
            case 1: // SOFT clipping 2
                if (overdriven_value <= -32767) {
                    clipped_value = -32767;
                } 
                else if (overdriven_value >= 32767) {
                    clipped_value = 32767;
                } 
                else {
                    clipped_value = 32767 * (scaled_overdriven_value - pow(scaled_overdriven_value,3)/3.0) * (3.0/2.0);
                }
                break;
            case 2: // HARD clipping
                clipped_value = 0.5 * (fabs(overdriven_value + 32767) - fabs(overdriven_value - 32767));
                break;
                
        }
        return clipped_value;
    }
            
    int16_t GenerateSample(float t) {
        int16_t sample;
        if (t >= BD.interval_) {
            sample = 32767 * sin(2.0 * M_PI * BD.frequency_ * (t - BD.interval_) + BD.phase_end_);
        } else {
            float func = BD.slope_ * t * t / 2.0 + BD.f0_ * t;
            sample = 32767 * sin(2.0 * M_PI * func);  
        }
        return sample;
    }

    int16_t GenerateHarmonics(float t) {
        int32_t sample;
        sample = 6553 * BD.harmonics_ * sin(2 * M_PI * (220 + flutter_[0]/125.0) * t + 0.5);
        sample += 9810 * BD.harmonics_ * sin(2 * M_PI * (BD.frequency_ * 7.8 + flutter_[1]/125.0) * t + 1.2);
        sample += 6553 * BD.harmonics_ * sin(2 * M_PI * (BD.frequency_ * 10.2 + flutter_[2]/125.0) * t + 2.1);
        sample >>= 8;
        return sample;
    }
};
