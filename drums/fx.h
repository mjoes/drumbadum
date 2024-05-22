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
        gen_(gen),
        in_buffer_l{},
        in_buffer_r{},
        buffer_index_l(0),
        buffer_index_r(0)

        // dis(-1,1)
        {
            tW_l = max_bit_ / sample_rate_ * ring_freq_l;
            tW_r = max_bit_ / sample_rate_ * ring_freq_r;
        }
    ~FX() {}

    void Process(int16_t *out_l, int16_t *out_r, int16_t *in_l, int16_t *in_r) {
        out_l_ = bitCrush(*in_l, 20);
        out_r_ = bitCrush(*in_r, 20);
        out_l_ = combFilter(out_l_, &buffer_index_l, in_samples_l, in_buffer_l);
        out_r_ = combFilter(out_r_, &buffer_index_r, in_samples_r, in_buffer_r);
        out_l_ = ringMod(out_l_, tW_l, &phase_acc_l);
        out_r_ = ringMod(out_r_, tW_r, &phase_acc_r);
        out_l_ = Overdrive(out_l_);
        out_r_ = Overdrive(out_r_);
        *out_l = out_l_;
        *out_r = out_r_;
    }

private:
    const uint16_t sample_rate_;
    const uint8_t bitsSine;
    const uint32_t max_bit_;

    static const uint16_t in_samples_l = 600;
    static const uint16_t in_samples_r = 1000;
    static const uint16_t ring_freq_l = 440;
    static const uint16_t ring_freq_r = 600;

    minstd_rand& gen_;
    int16_t in_buffer_l[in_samples_l];
    int16_t in_buffer_r[in_samples_r];
    uint32_t phase_acc_l, phase_acc_r, tW_l, tW_r;
    int32_t out_l_, out_r_;
    Out out;
    uint16_t buffer_index_l, buffer_index_r;
    // uniform_int_distribution<int32_t> dis;

    int16_t bitCrush(int16_t sample, uint8_t depth) {
        // depth between 1-50
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
        int32_t out = (sample * ring_sample) >> 12;
        
        return (out + sample) / 2;
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
            uint32_t scaled_ov = ((value) << 15) / 32767 + (1 << 15);
            scaled_ov *= (255 / 2);
            uint32_t int_pos = ((scaled_ov) >> 15);
            int16_t a = env_overdrive[int_pos];
            int16_t b = env_overdrive[int_pos + 1];
            clipped_value = a + ((b - a) * (scaled_ov & ((1 << 15) - 1)) >> 15);
        }
        return clipped_value;
    }
};
