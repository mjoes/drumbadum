#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>
#include "envelopes.h"

using namespace std;

struct FmHitSculpt {
    uint16_t frequency_, velocity_, fm_amount_;
    uint16_t ratio_[2];
};

class FmHit {
public:
    FmHit(
        uint16_t sample_rate,
        mt19937& gen)
        :
        sample_rate_(sample_rate),
        gen_(gen),
        dis(-1,1)
        {
            rel_pos_ = 0;
            samplesPerCycle = 1024;
            bitsSine = 10;
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
        tW_ = 4294967296 / sample_rate_ * 440;//FM.frequency_;
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
        FM.ratio_[0] = (3 * normalized_ratio * FM.frequency_ / 7);
        FM.ratio_[1] = normalized_ratio * FM.frequency_;
        tW_0_ = 4294967296 / sample_rate_ * FM.ratio_[0];
        tW_1_ = 4294967296 / sample_rate_ * FM.ratio_[1];
    }

    void set_fm_amount(uint16_t fm_amount, uint8_t fm_type_prb) {
        if (bernoulli_draw(fm_type_prb - 40) == 0) { // Tunes how often it is overdriven
            FM.fm_amount_ = fm_amount;
        } else { // bonkers mode
            FM.fm_amount_ = fm_amount * 100;
        }
        amp_ratio_ = (FM.fm_amount_ / 100.0f) * (1.0f / 65535.0f); // Uniform for nows
        amp_ratio_s = (FM.fm_amount_ << 15) / (100 * 65535);
    }

    void set_start(uint8_t pattern_nr, uint8_t random_pattern_nr, uint8_t randomness, bool accent) {
        rel_pos_ = 0;
        sampleNr = 0;
        running_ = true;
        end_i_ = length_decay_;
        set_velocity(300, accent);
        set_pattern(pattern_nr, random_pattern_nr, randomness, accent);
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        int32_t sample;
        uint16_t rel_env = interpolate_env(rel_pos_, length_decay_, exp_env);
        sample = GenerateSample(rel_env);
        // int32_t nan = GenerateSample_old(rel_env);
        sample = sample * FM.velocity_ / 1000;
        sample *= rel_env / 2;
        int16_t output = sample / 32767;

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;
    }

private:
    uint32_t rel_pos_, end_i_, length_decay_, decay_;
    const uint16_t sample_rate_;
    bool running_;
    mt19937& gen_;
    FmHitSculpt FM;
    // float dt = 1.0f / sample_rate_;
    // float sample_f, angle;
    // float period = sample_rate_ / FM.frequency_;

    float angle, amp_ratio_;
    uint32_t amp_ratio_s;
    uint16_t sample_N;
    uint32_t sampleNr;
    uint16_t samplesPerCycle;
    uint32_t phase_acc, phase_acc_0, phase_acc_1, tW_, tW_0_, tW_1_;
    uint8_t bitsSine;
    uint16_t phase_inc;

    uniform_int_distribution<int32_t> dis;

    inline int16_t LookupFM(uint16_t frequency) {
        uint8_t shift = 16;
        uint32_t s1 = (frequency << shift) / sample_rate_;
        uint32_t s2 = rel_pos_ * s1;
        uint16_t int_pos = ((s2 & ((1 << shift) - 1)) * 1024 / (1 << shift));
        int16_t a = sine[int_pos];
        int16_t b = sine[int_pos + 1];
        int16_t base_sample = a + (b - a) * (s2 & ((1 << shift) - 1) >> shift);

        return base_sample;
    }

    int32_t GenerateSample_old(uint16_t rel_env_old) {
        float t = static_cast<float>(rel_pos_) / sample_rate_;
        float rel_env = interpolate_env_old(rel_pos_, length_decay_, exp_env);
        float amp_ratio_ = FM.fm_amount_ / 100.0f * rel_env; // Uniform for nows
        float mod_1 = amp_ratio_ * sin(2 * M_PI * (FM.ratio_[0]) * t);
        float mod_2 = amp_ratio_ * sin(2 * M_PI * (FM.ratio_[1]) * t);


        float mod_3 = dis(gen_) * interpolate_env_old(rel_pos_, 3480, exp_env); // Whitenoise transient

        int32_t sample = 32767 * sin(2 * M_PI * 440 * t + mod_1 + mod_2);// + mod_3);
        printf("old: %i \n", (sample));
        return sample;
    }

    int16_t GenerateSample(uint16_t rel_env) {
        phase_acc_0 += tW_0_;
        phase_acc_1 += tW_1_;
        int32_t mod_0 = cosine[phase_acc_0 >> (32 - bitsSine)] * FM.ratio_[0] / 32767;
        int32_t mod_1 = cosine[phase_acc_1 >> (32 - bitsSine)] * FM.ratio_[1] / 32767;
        int32_t total_mod = (mod_0 + mod_1) * FM.fm_amount_ / 100 * rel_env / 65535;
        
        // printf("%i, ", total_mod);
        // total_mod = total_mod * FM.fm_amount_ / 100;
        // printf("%i, ", total_mod);
        // total_mod = total_mod * rel_env / 65535;
        // printf("%i ,rel_env: %i \n", total_mod, rel_env);
        // float accurate = mod_0 * (FM.fm_amount_ / 100.0f) * (rel_env / 65535.0f);
        // printf("float: %f , int: %i\n",accurate, total_mod);
        int16_t f_inst = 440 + total_mod;
        uint32_t tW_new = 4294967296 / sample_rate_ * f_inst;
        // printf("%i, %i \n", mod_0, f_inst );

        phase_acc += tW_new;
        phase_inc = phase_acc >> (32 - bitsSine);
        int32_t fraction_fp = (phase_acc & ((1 << (32 - bitsSine)) - 1));
        int16_t a = sine[phase_inc];
        int16_t b = sine[phase_inc + 1];
        int16_t base_sample;
        if ((b-a) < 0) {
            base_sample = a - (((a - b) * fraction_fp) >> 22);
        } else {
            base_sample = a + (((b - a) * fraction_fp) >> 22);
        }

        float fraction = static_cast<float>(phase_acc & ((1 << (32 - bitsSine)) - 1)) / (1 << 22);  // Fractional part of phase increment
        int16_t base_sample_2 = a + (b - a) * (fraction);
        int16_t base_sample_3 = a + ((b - a) * (fraction_fp) >> 22);
        int16_t base_sample_4 = a + ((b - a) * (phase_acc -(phase_inc<<22))>>22);
        //printf("%i, %i, %i, %i, %i, %i  \n", a, b, base_sample, base_sample_2, base_sample_3 ,base_sample_4);
        
        return base_sample_3;
    }
};

// static inline signed short fast_interpolate(unsigned short x) {
//         register unsigned short i = (x >> 8);
//         return lut[i] + (signed short)((signed long)(lut[i + 1] - lut[i]) * (x - (i << 8)) >> 8);
// }







    
//  int32_t GenerateSample(uint16_t rel_env) {
//         int16_t mod_1 = LookupFM(FM.ratio_[0]);
//         int16_t mod_2 = LookupFM(FM.ratio_[1]);
//         // int64_t total_mod;
//         // total_mod = ((mod_1 + mod_2) * 10000000 / 32767) * ((rel_env << 15) / 65535);
//         // total_mod >>= 15;
//         // // printf("step1: %li \n", total_mod);
//         // total_mod = total_mod * FM.fm_amount_ / 100; // Total mod factor 1e7 too large
//         float flt_mod = ((mod_1 + mod_2) / 32767.0f) * (rel_env / 65535.0f) * (FM.fm_amount_ / 100.0f);
//         int16_t offset = static_cast<int16_t>(flt_mod * 1024);
        
//         // uint64_t angle_alt = ((sampleNr * FM.frequency_) << 15)/sample_rate_;
//         // int64_t total_ang;
//         // if (total_mod > 0) {
//         //     total_ang = ((total_mod << 15) / 62831853) + angle_alt;
//         // } else {
//         //     total_ang =  angle_alt - ((-total_mod << 15) / 62831853);
//         // }


//         // printf("step2: %li, fm_amount: %i \n", total_mod, FM.fm_amount_);
//         // printf("mod: %i \n", total_ang >> 15);
//         // printf ("total_shift: %i,%f\n", (test), ((double)(test & ((1 << 15) - 1))/(1 << 15)));

//         uint64_t angle_alt = ((sampleNr * FM.frequency_) << 15)/sample_rate_;
//         //  printf ("total_shift: %i,%f\n", (total_ang), ((double)(total_ang & ((1 << 15) - 1))/(1 << 15)));
//         //  printf ("angle_alt_2: %i,%
//         , (angle_alt), ((double)(angle_alt & ((1 << 15) - 1))/(1 << 15)));


    //     uint16_t int_pos = ((angle_alt & ((1 << 15) - 1)) * 1024 / (1 << 15));
    //     int16_t int_pos_off = (int_pos + offset);
    //     // printf("%i, %i, %i \n", int_pos, offset, int_pos_off);
    //     while (int_pos_off < 0) int_pos_off += 1024;
    //     while (int_pos_off >= 1023) int_pos_off -= 1024;
    //     // printf("%i \n", int_pos_off);
        
    //     int16_t a = sine[int_pos_off];
    //     int16_t b = sine[int_pos_off + 1];
    //     int16_t base_sample = a + (b - a) * (angle_alt & ((1 << 15) - 1) >> 15);

    //     // printf("old: %f  ", angle);
    //     // printf ("new: %i,%f\n", (int_pos), ((double)(int_pos & ((1 << 15) - 1))/(1 << 15)));
        
	// 	int32_t out = base_sample;
    //     sampleNr++;
    //     if (sampleNr >= sample_N) sampleNr = 0; 

    //     return out;
    // }

    
