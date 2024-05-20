#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>
#include "envelopes.h"
#include "utils.h"

using namespace std;

struct FmHitSculpt {
    uint16_t frequency_, velocity_, fm_amount_;
    uint16_t mod_[2];
};

class FmHit {
public:
    FmHit(
        uint16_t sample_rate,
        mt19937& gen)
        :
        sample_rate_(sample_rate),
        max_bit_(4294967295),
        gen_(gen),
        bitsSine(10),
        dis(-1,1)
        {
            rel_pos_ = 0;
        }
    ~FmHit() {}

    void set_pattern(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        if (accent == true) {
            randomness = 0;
        }
        set_decay(snd_random(patterns[pattern_nr][10],random_pattern_nr,10,randomness));
        set_fm_amount(snd_random(patterns[pattern_nr][11],random_pattern_nr,11,randomness),randomness);
        set_ratio(snd_random(patterns[pattern_nr][12],random_pattern_nr,12,randomness));
        set_frequency(snd_random(patterns[pattern_nr][13],random_pattern_nr,13,randomness));
    }

    void set_decay(uint16_t decay, bool decay_type = 0) {
        length_decay_ = decay * sample_rate_ / 25;
    }

    void set_frequency(uint8_t frequency) {
        FM.frequency_ = frequency / 3 + 60; // range between 60 and 90
    }

    void set_velocity(uint16_t velocity, bool accent) {
        if (accent == true) {
            FM.velocity_ = 1000;
        } else {
            FM.velocity_ = velocity;
        }
    }

    void set_ratio(uint8_t ratio) {
        uint8_t normalized_ratio = (ratio * 7 / 2 + 150) / 2; // range between 75 and 250
        FM.mod_[0] = (3 * normalized_ratio * FM.frequency_ / 7);
        FM.mod_[1] = normalized_ratio * FM.frequency_;
        tW_0_ = max_bit_ / sample_rate_ * FM.mod_[0];
        tW_1_ = max_bit_ / sample_rate_ * FM.mod_[1];
    }

    void set_fm_amount(uint16_t fm_amount, uint8_t fm_type_prb) {
        if (bernoulli_draw(fm_type_prb - 40) == 0) { // Tunes how often it is overdriven
            FM.fm_amount_ = fm_amount;
        } else { // bonkers mode
            FM.fm_amount_ = fm_amount * 100;
        }
    }

    void set_start(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_decay_;
        set_velocity(300, accent);
        set_pattern(pattern_nr, random_pattern_nr, randomness, accent);
    }

    Out Process() {
        Out out;
        // Generate waveform sample
        if (running_ == false) {
            out.out_l = 0;
            out.out_r = 0;
            return out;
        }

        int32_t sample;
        uint16_t rel_env = interpolate_env(rel_pos_, length_decay_, exp_env);
        sample = GenerateSample(rel_env);
        sample = sample * FM.velocity_ / 1000;
        sample *= rel_env / 2;
        int16_t output = sample / 32767; // Again rather cheeky :)

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        out.out_l = output;
        out.out_r = output;
        return out;   
    }

private:
    uint32_t rel_pos_, end_i_, length_decay_, decay_;
    const uint16_t sample_rate_;
    const uint32_t max_bit_;
    bool running_;
    mt19937& gen_;
    uint32_t phase_acc, phase_acc_0, phase_acc_1, tW_0_, tW_1_;
    const uint8_t bitsSine;
    FmHitSculpt FM;
    uniform_int_distribution<int32_t> dis;

    int16_t GenerateSample(uint16_t rel_env) {
        phase_acc_0 += tW_0_;
        phase_acc_1 += tW_1_;
        int32_t mod_0 = cosine[phase_acc_0 >> (32 - bitsSine)] * FM.mod_[0] / 32767;
        int32_t mod_1 = cosine[phase_acc_1 >> (32 - bitsSine)] * FM.mod_[1] / 32767;
        int32_t total_mod = (mod_0 + mod_1) * FM.fm_amount_ / 100 * rel_env / 65535;
        int16_t f_inst = 440 + total_mod;
        uint32_t tW_new = max_bit_ / sample_rate_ * f_inst;

        phase_acc += tW_new;
        uint16_t phase_inc = phase_acc >> (32 - bitsSine);
        int32_t fraction_fp = (phase_acc & ((1 << (32 - bitsSine)) - 1));
        int16_t a = sine[phase_inc];
        int16_t b = sine[phase_inc + 1];
        int16_t base_sample = a + ((b - a) * (fraction_fp) >> 22);
        
        return base_sample;
    }
};
