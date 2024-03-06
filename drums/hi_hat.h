#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>
#include "../envelopes.h"

using namespace std;

class HiHat {
public:
    HiHat(
        uint16_t sample_rate)
        :
        sample_rate_(sample_rate),
        rd(), 
        gen(rd()), 
        dis(-32767, 32767)
        {
            rel_pos_ = 0;
        }
    ~HiHat() {}

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
        phi_ = 2 * cos(2 * M_PI * frequency_ / sample_rate_);
    }

    void set_velocity(uint16_t velocity) {
        velocity_ = velocity / 1000.0;
    }

    void set_bandwidth(uint16_t bandwidth) {
        bandwidth_ = bandwidth;
        lambda_ = 1 / tan(M_PI * bandwidth_ / sample_rate_);
    }

    void set_start() {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_decay_;
        a0 = 1 / (1 + lambda_);
        b1 = - lambda_ * phi_ * a0;
        b2 = a0 * (lambda_ - 1);
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        int32_t sample;
        sample = bp_filter_2(GenerateSample());
        sample *= velocity_;
        sample *= interpolate_env();
        int16_t output = sample;

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    float lambda_;
    float phi_;
    float a0;
    float b1;
    float b2;
    float velocity_;
    uint32_t rel_pos_;
    uint32_t end_i_;
    uint32_t length_decay_;
    uint16_t decay_;
    uint16_t bandwidth_;
    uint16_t frequency_;    
    const uint16_t sample_rate_;
    int32_t x_n_1 = 0;
    int32_t y_n_1 = 0;
    int32_t x_n_2 = 0;
    int32_t y_n_2 = 0;
    bool running_;
    bool decay_type_;
    const uint16_t* lookup_table_;
    vector<int16_t> flutter_; 

    random_device rd;
    mt19937 gen;
    uniform_int_distribution<int32_t> dis;

    int32_t GenerateSample() {
        // Replace this with your own waveform generation logic
        int32_t sample = dis(gen);
        
        return sample;
    }

    int32_t bp_filter_2(int32_t x_n) {
        int32_t filtered = a0 * x_n - a0 * x_n_2 - b1 * y_n_1 - b2 * y_n_2;
        y_n_2 = y_n_1;
        y_n_1 = filtered;
        x_n_2 = x_n_1;
        x_n_1 = x_n;
        return filtered;
    }

    float interpolate_env(){
        float pos = static_cast<float>(rel_pos_) / length_decay_ * 256.0;
        float frac = pos - int(pos);
        uint16_t a = lookup_table_[int(pos)];
        uint16_t b = lookup_table_[int(pos) + 1];
        uint16_t output = a + frac * (b - a);
        return output / 65535.0;
    }
};

