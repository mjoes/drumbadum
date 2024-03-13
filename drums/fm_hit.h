#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>
#include "../utils/envelopes.h"

using namespace std;

struct FmHitSculpt {
    float fm_amount_, velocity_;
    uint16_t frequency_;    
    uint8_t ratio_[2];
};

class FmHit {
public:
    FmHit(
        uint16_t sample_rate)
        :
        sample_rate_(sample_rate),
        rd(), 
        gen(rd()), 
        dis(-1, 1)
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
        FM.frequency_ = frequency;
    }

    void set_velocity(uint16_t velocity) {
        FM.velocity_ = velocity / 1000.0;
    }

    void set_ratio(uint16_t ratio) {
        uint8_t normalized_ratio = ratio / 20;
        FM.ratio_[0] = (3 * normalized_ratio / 7);
        FM.ratio_[1] = normalized_ratio;
    }

    void set_fm_amount(uint16_t fm_amount, bool fm_type) {
        if (fm_type == 0) {
            FM.fm_amount_ = fm_amount / 1000.0;
        } else { // bonkers mode
            FM.fm_amount_ = fm_amount / 10.0;
        }
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
        float rel_env = interpolate_env(length_decay_);
        sample = GenerateSample(t, rel_env);
        sample *= FM.velocity_;
        sample *= rel_env;
        int16_t output = sample;

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    uint32_t rel_pos_, end_i_, length_decay_, decay_;
    const uint16_t sample_rate_;
    bool running_, decay_type_;
    const uint16_t* lookup_table_;
    FmHitSculpt FM;

    random_device rd;
    mt19937 gen;
    uniform_int_distribution<int32_t> dis;

    int32_t GenerateSample(float t, float rel_env) {
        // Replace this with your own waveform generation logic
        float amp_ratio_ = FM.fm_amount_ * rel_env; // Uniform for nows
        float mod_1 = amp_ratio_ * sin(2 * M_PI * (FM.ratio_[0] * FM.frequency_) * t);
        float mod_2 = amp_ratio_ * sin(2 * M_PI * (FM.ratio_[1] * FM.frequency_) * t);
        // The next number could come from a lookup table, not sure if that's more efficient though
        // memory vs speed....
        float mod_3 = dis(gen) * interpolate_env(3480); 

        int32_t sample = 32767 * sin(2 * M_PI * FM.frequency_ * t + mod_1 + mod_2 + mod_3);
        
        return sample;
    }

    float interpolate_env(uint32_t length_decay){
        float pos = static_cast<float>(rel_pos_) / (length_decay) * 256.0;
        uint16_t int_pos = int(pos);
        uint16_t output;
        if (int_pos > 255) {
            output = 0;
        } else {
            float frac = pos - int_pos;
            uint16_t a = lookup_table_[int_pos];
            uint16_t b = lookup_table_[int_pos + 1];
            output = a + frac * (b - a);
        }

        return output / 65535.0;
    }
};

