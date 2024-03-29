#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <random>
#include "../utils/envelopes.h"

using namespace std;

struct BassDrumSculpt {
    uint16_t frequency_, attack_, velocity_, overdrive_, harmonics_;
    uint8_t decay_;
};

struct BassDrumEnv {
    float interval_, phase_, slope_, phase_end_;
    uint16_t f0_;
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
        }
    ~BassDrum() {}

    void set_frequency(uint16_t frequency) {
        BD.frequency_ = frequency;
    }

    void set_overdrive(uint16_t overdrive) {
        BD.overdrive_ = (overdrive << 4) / 200 + (1 << 4); 
    }

    void set_velocity(uint16_t velocity) {
        BD.velocity_ = (velocity << 8) / 1000;
    }

    void set_decay(uint16_t decay) {
        length_decay_ = decay * sample_rate_ / 400;
    }

    void set_attack(uint16_t attack) {
        BD.attack_ = 1001 - attack;
        length_attack_ = sample_rate_ / BD.attack_;
    }

    void set_harmonics(uint16_t harmonics) {
        BD.harmonics_ = (harmonics << 8) / 1000;
    }

    void set_envelope(uint16_t envelope) {
        ENV.f0_ = BD.frequency_ * envelope / 20; // 20 is for a kick drum range, might want to play around with this
        ENV.interval_ = 0.05; // TODO: This needs tuning
        ENV.slope_ = (BD.frequency_ - ENV.f0_) / ENV.interval_;
        ENV.phase_end_ = -2 * M_PI * (ENV.f0_ + BD.frequency_ ) / 2 * ENV.interval_;
    }

    void set_start() {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_attack_ + length_decay_;

        for (int i = 0; i < 3; ++i) {
            flutter_[i] = d(gen_);
        }
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
        sample *= interpolate_env(rel_pos_, length_decay_, exp_env);
        int16_t output = Overdrive(sample, 1); // Apply distortion
 
        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    uint32_t rel_pos_, end_i_, length_decay_;
    uint16_t length_attack_;
    const uint16_t sample_rate_;
    const uint16_t* lookup_table_;
    vector<int16_t> flutter_; 
    bool running_;
    mt19937& gen_;
    normal_distribution<double> d{0, 1000};
    BassDrumSculpt BD;
    BassDrumEnv ENV;

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
        if (t > ENV.interval_) {
            sample = 32767 * sin(2 * M_PI * BD.frequency_ * t + ENV.phase_end_);
        } else {
            float func = ENV.slope_ * pow(t,2) / 2.0 + ENV.f0_ * t;
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
