#include <cstdint>
#include <random>
#include "utils.h"
#include "envelopes.h"
#include "rhythmic_pattern.h"


class FX {
public:
    FX(
        uint16_t sample_rate,
        minstd_rand& gen)
        :
        sample_rate_(sample_rate),
        bitsSine(10),
        max_bit_(4294967295),
        flip_(false),
		running_(false),
        gen_(gen),
        in_buffer_l{},
        in_buffer_r{},
        buffer_index_l(0),
        buffer_index_r(0),
        dis(-1024,1024),
        dis_u(0,100)
        {
            tW_l = max_bit_ / sample_rate_ * ring_freq_l;
            tW_r = max_bit_ / sample_rate_ * ring_freq_r;
        }
    ~FX() {}

    void set_start(uint16_t steps_sample) {
        running_ = true;
        rel_pos_ = 0;
        end_i_ = steps_sample;
        param_id_ = rand() % 50;
        for (uint8_t i = 0; i < 5; i++){
            param_chance_[i] = (dis_u(gen_) < patterns[param_id_][i]);
            // printf("%i\n", param_chance_[i]);
        }
    }

    void Process(int16_t *out_l, int16_t *out_r, int16_t *in_l, int16_t *in_r, uint8_t pot_volume, uint8_t vol_scaling) {
        // Having speed issues on the STM32
        if (running_ == false) {
            *out_l = (*in_l * pot_volume / vol_scaling) >> 7;
            *out_r = (*in_r * pot_volume / vol_scaling) >> 7;
        } else {
            out_l_ = *in_l;
            out_r_ = *in_r;
           if (param_chance_[0]){
               out_l_ = bitCrush(*in_l, 50);
               out_r_ = bitCrush(*in_r, 50);
           }
           if (param_chance_[1]){
               out_l_ = waveFolder(out_l_, 100, 64);
               out_r_ = waveFolder(out_r_, 100, 64);
           }
            if (param_chance_[2]){
                out_l_ = combFilter(out_l_, &buffer_index_l, in_samples_l, in_buffer_l);
                out_r_ = combFilter(out_r_, &buffer_index_r, in_samples_r, in_buffer_r);
            }
            if (param_chance_[3]){
                out_l_ = ringMod(out_l_, tW_l, &phase_acc_l);
                out_r_ = ringMod(out_r_, tW_r, &phase_acc_r);
            }
           if (param_chance_[4]){
               out_l_ = artifacts(out_l_);
               out_r_ = artifacts(out_r_);
           }
            out_l_ = Overdrive(out_l_) / 2; // reduce volume of FX to align with dry sound
            out_r_ = Overdrive(out_r_) / 2;
            *out_l = (out_l_ * pot_volume / vol_scaling) >> 7;
            *out_r = (out_r_ * pot_volume / vol_scaling) >> 7;

        }
        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }

    }

private:
    const uint16_t sample_rate_;
    const uint8_t bitsSine;
    const uint32_t max_bit_;

    static const uint16_t in_samples_l = 600;
    static const uint16_t in_samples_r = 1000;
    static const uint16_t ring_freq_l = 440;
    static const uint16_t ring_freq_r = 600;

    bool flip_, running_;
    minstd_rand& gen_;
    uint8_t param_id_;
    int16_t in_buffer_l[in_samples_l];
    int16_t in_buffer_r[in_samples_r];
    uint32_t phase_acc_l, phase_acc_r, tW_l, tW_r;
    int32_t out_l_, out_r_;
    Out out;
    uint16_t buffer_index_l, buffer_index_r, end_i_, rel_pos_;
    uniform_int_distribution<int16_t> dis, dis_u;
    uint8_t param_chance_[5];

    int32_t bitCrush(int16_t sample, uint8_t depth) {
        // depth between 1-50
        if (param_chance_[0] > patterns[param_id_][0]) {
            return sample;
        }
        int32_t out;
        out = ((sample + 32768) * depth) >> 16;
        out = (out << 16) / depth;
        out = static_cast<int16_t>(out - 32768);

        return out;
    }

    int16_t combFilter(int16_t sample, uint16_t *buffer_index, uint16_t in_samples, int16_t* in_buffer) {
        int16_t read_index_ = (*buffer_index + 1) % in_samples;
        int16_t in_prev_ = in_buffer[read_index_];
        int16_t out = sample + in_prev_ / 2; // g_1 = 2

        in_buffer[*buffer_index] = sample;
        *buffer_index = (*buffer_index + 1) % in_samples;

        return out;
    }

    int32_t ringMod(int16_t sample, uint32_t tW, uint32_t *phase_acc) {
        *phase_acc += tW;
        uint16_t phase_inc = *phase_acc >> (32 - bitsSine);
        int32_t fraction_fp = (*phase_acc & ((1 << (32 - bitsSine)) - 1));
        int16_t a = sine[phase_inc];
        int16_t b = sine[phase_inc + 1];
        int16_t ring_sample = a + ((b - a) * (fraction_fp) >> 22);
        int32_t out = (sample * (ring_sample / 3)) >> 12;

        return (out + sample * 2) / 3;
    }


    int32_t artifacts(int32_t sample){
        int16_t chance = rand() % 100;
        int32_t out = sample;
        if (chance < 2){
           out = -out; // semi harsh crackle
            // out - out + dis(gen_) * 2; // White noise type crackle
            // flip_ = !flip_; // for the fully deciate below
        }
        // FULLY DECIMATE
        // if (flip_) {
        //     out = -out;
        // }

        return out;
    }

    int16_t waveFolder(int32_t sample, int16_t amount, int16_t bias) {
        // Very bare bones
        // amount from 0-127
        // bias from 0-127
        int32_t driven_sample = sample + (bias - 64) * 100;
        uint16_t threshold = 32767 - 10000 - amount * 100;
        int32_t out = driven_sample;
        // printf("%i , %i, %i \n", sample, driven_sample, threshold);
        if (driven_sample <= -threshold) {
            out = -threshold + (-threshold - driven_sample);
        }
        else if (driven_sample >= threshold) {
            out = threshold - (driven_sample - threshold);
        }
        return static_cast<int16_t>(out);
    }

    int16_t Overdrive(int32_t value) {
        int16_t clipped_value;
        if (value <= -32767) {
            clipped_value = -32767;
        }
        else if (value >= 32767) {
            clipped_value = 32767;
        }
        else {
        	clipped_value = value; // Faster than below
//            uint32_t scaled_ov = ((value) << 15) / 32767 + (1 << 15);
//            scaled_ov *= (255 / 2);
//            uint32_t int_pos = ((scaled_ov) >> 15);
//            int16_t a = env_overdrive[int_pos];
//            int16_t b = env_overdrive[int_pos + 1];
//            clipped_value = a + ((b - a) * (scaled_ov & ((1 << 15) - 1)) >> 15);
        }
        return clipped_value;
    }
};
