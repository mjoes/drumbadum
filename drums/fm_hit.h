#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>
#include "../envelopes.h"

using namespace std;

class FmHit {
public:
    FmHit(
        uint16_t sample_rate)
        :
        sample_rate_(sample_rate)
        {
            rel_pos_ = 0;
        }
    ~FmHit() {}

    void set_decay(uint16_t decay, bool decay_type = 0) {
        decay_type_ = decay_type;
        length_decay_ = decay * sample_rate_ / 500;
        if (decay_type_ == 0) {
            lookup_table_ = exp_env;
        } else {
            lookup_table_ = log_env;
        }
    }

    void set_frequency(uint16_t frequency) {
        frequency_ = frequency;
        // phi_ = 2 * cos(2 * M_PI * frequency_ / sample_rate_);
    }

    void set_start() {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_decay_;
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        int32_t sample;
        float t = static_cast<float>(rel_pos_) / sample_rate_;
        float rel_env = interpolate_env();
        sample = GenerateSample(t, rel_env);
        sample *= rel_env;
        int16_t output = sample;

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    uint32_t rel_pos_;
    uint32_t end_i_;
    uint32_t length_decay_;
    uint16_t decay_;
    uint16_t frequency_;    
    const uint16_t sample_rate_;
    bool running_;
    bool decay_type_;
    const uint16_t* lookup_table_;

    int32_t GenerateSample(float t, float rel_env) {
        // Replace this with your own waveform generation logic
        float amp_ratio_ = 0.5 * rel_env; // Uniform for now
        int16_t ratio_[2] = { 3, 7 };
        float mod_1 = amp_ratio_ * sin(2 * M_PI * (ratio_[0] * frequency_) * t);
        float mod_2 = amp_ratio_ * sin(2 * M_PI * (ratio_[1] * frequency_) * t);
        int32_t sample = 32767 * sin(2 * M_PI * frequency_ * t + mod_1 + mod_2);
        
        return sample;
    }

    float interpolate_env(){
        float pos = static_cast<float>(rel_pos_) / (length_decay_) * 256.0;
        uint16_t int_pos = int(pos);
        float frac = pos - int_pos;
        uint16_t a = lookup_table_[int_pos];
        uint16_t b = lookup_table_[int_pos + 1];
        uint16_t output = a + frac * (b - a);

        return output / 65535.0;
    }
};

