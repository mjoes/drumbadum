#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>
#include "../utils/envelopes.h"

using namespace std;

struct HiHatSculpt {
    float velocity_;
    uint16_t bandwidth_;
    uint16_t frequency_;  
};

class HiHat {
public:
    HiHat(
        uint16_t sample_rate,
        mt19937& gen)
        :
        sample_rate_(sample_rate),
        gen_(gen),
        dis(-32767, 32767)
        {
            rel_pos_ = 0;
        }
    ~HiHat() {}

    void set_pattern(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        if (accent == true) {
            randomness = 0;
        }
        set_decay(snd_random(patterns[pattern_nr][6],random_pattern_nr,6,randomness), bernoulli_draw(10));
        set_frequency(snd_random(patterns[pattern_nr][8],random_pattern_nr,8,randomness));
        set_bandwidth(snd_random(patterns[pattern_nr][9],random_pattern_nr,9,randomness));
    }

    void set_decay(uint16_t decay, bool decay_type = 0) {
        decay_type_ = decay_type;
        length_decay_ = decay * sample_rate_ / 50;
        if (decay_type_ == 0) {
            lookup_table_ = exp_env;
        } else {
            lookup_table_ = log_env;
        }
    }

    void set_frequency(uint16_t frequency) {
        HH.frequency_ = frequency * 4000 / 100 + 8000; //range from 8000-12000
        phi_ = 2 * cos(2 * M_PI * HH.frequency_ / sample_rate_);
    }

    void set_velocity(uint16_t velocity, bool accent) {
        if (accent == true) {
            HH.velocity_ = 1.0;
        } else {
            HH.velocity_ = velocity / 1000.0;
        }
    }

    void set_bandwidth(uint16_t bandwidth) {
        HH.bandwidth_ = bandwidth * 3000 / 100 + 300; //range from 300-3300;
        lambda_ = 1 / tan(M_PI * HH.bandwidth_ / sample_rate_);
    }

    void set_start(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        rel_pos_ = 0;
        running_ = true;
        a0 = 1 / (1 + lambda_);
        b1 = - lambda_ * phi_ * a0;
        b2 = a0 * (lambda_ - 1);
        set_velocity(500, accent);
        set_pattern(pattern_nr, random_pattern_nr, randomness, accent);
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        int32_t sample;
        sample = bp_filter_2(GenerateSample());
        sample *= HH.velocity_;
        sample *= interpolate_env(rel_pos_, length_decay_, lookup_table_);
        int16_t output = sample;

        rel_pos_ += 1;
        if (rel_pos_ >= length_decay_) {
            running_ = false;
        }
        return output;           
    }

private:
    float lambda_, phi_, a0, b1, b2;
    uint32_t rel_pos_, decay_, length_decay_;
    int32_t x_filter[2] = { 0, 0 };
    int32_t y_filter[2] = { 0, 0 };
    const uint16_t sample_rate_;
    const uint16_t* lookup_table_;
    vector<int16_t> flutter_; 
    bool running_, decay_type_;
    mt19937& gen_;
    uniform_int_distribution<int32_t> dis;
    HiHatSculpt HH;

    int32_t GenerateSample() {
        // Replace this with your own waveform generation logic
        int32_t sample = dis(gen_);
        
        return sample;
    }

    int32_t bp_filter_2(int32_t x_n) {
        int32_t filtered = a0 * x_n - a0 * x_filter[1] - b1 * y_filter[0] - b2 * y_filter[1];
        y_filter[1] = y_filter[0];
        y_filter[0] = filtered;
        x_filter[1] = x_filter[0];
        x_filter[0] = x_n;
        return filtered;
    }
};

