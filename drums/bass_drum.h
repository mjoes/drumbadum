#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <algorithm>

class BassDrum {
public:
    BassDrum() {}
    ~BassDrum() {}

    void Init(
        uint16_t sample_rate, 
        size_t rel_pos = 0, 
        float out = 0.0f, 
        float seg_tmp = 0.0f, 
        uint8_t segment = 1) 
    {
        sample_rate_ = sample_rate;
        rel_pos_ = rel_pos;
        out_ = out;
        seg_tmp_ = seg_tmp;
        segment_ = segment;
    }

    void set_frequency(uint16_t frequency) {
        frequency_ = frequency;
        // phase_ = static_cast<float>(frequency_) * (2.0f * 3.14159f) / sample_rate_;
    }

    void set_decay(uint16_t decay) {
        decay_ = decay;
    }

    void set_envelope(uint16_t envelope) {
        envelope_ = envelope;
    }

    void set_attack(uint16_t attack) {
        attack_ = attack;
    }

    void set_start(size_t start) {
        rel_pos_ = 0;
        start_i_ = start;
        end_i_ = start + lengthHit();
    }

    int16_t Process(size_t it) {
        // Generate waveform sample
        if (it > start_i_ && it < end_i_) {
            int16_t sample = GenerateSample(rel_pos_);
            float env = GenerateEnv(rel_pos_);
            int16_t out_val = static_cast<int16_t>(sample * env);
            rel_pos_ += 1;

            // Clip sample if necessary
            return out_val;                
        }
        return 0;
    }

private:
    uint16_t frequency_;
    float phase_;
    uint8_t cross_fade_;
    uint16_t decay_;
    uint16_t sample_rate_;
    uint16_t envelope_;
    uint16_t attack_;
    size_t rel_pos_;
    size_t start_i_;
    size_t end_i_;
    float out_;
    float seg_tmp_;
    uint8_t segment_;
    
    int32_t GenerateSample(size_t rel_pos_samp) {
        // Replace this with your own waveform generation logic
        // float freq_slope = -200.0f ;
        float T = 1.0f;
        float f0 = 10;
        float f1 = 4000;
        float t = static_cast<float>(rel_pos_samp) / sample_rate_;
        float b = log(f1/f0) / T;
        float a = 2.0 * 3.14159f * f0 / b;
        float t_ = T * t;
        // float k = pow((f1 / f0),(T/t));
        // printf("%f\n",sin(2.0f * 3.14159f *frequency * t));
        int32_t sample = 32767.0f * sin(a * exp(b * t_));
        // int32_t sample = static_cast<int32_t>(32767.0f * sin((2.0f * 3.14159f * f0) *  
                    // ((pow(k,(t/T))-1)/log(k)))); // 32767 is for PCM waves
        // int32_t sample = static_cast<int32_t>(32767.0f * sin((2.0f * 3.14159f) *  
        //     ((static_cast<float>(frequency_)  * t ) + (freq_slope * pow(t,2.0f) / 2.0f)))); // 32767 is for PCM waves
        return sample;
    }

    float GenerateEnv(size_t rel_pos_env) {
        float rel_pos_tmp = static_cast<float>(rel_pos_env)/sample_rate_;
        switch (segment_) {
            case 1: 
                out_ = attack_ * rel_pos_tmp;
                if (out_ >= 1) {
                    segment_ = 2;
                    seg_tmp_ = rel_pos_tmp;
                }
                break;
            case 2:
                out_ = 1.0f * exp(-decay_ * (rel_pos_tmp-seg_tmp_));
                if (out_ <= 1e-4) {
                    segment_ = 3;
                }
                break;
            case 3:
                out_ = 0.0f;
                break;
        }
    
        return out_;
    }

    uint16_t lengthHit() {
        uint16_t segment_1= (1.0f/static_cast<float>(attack_)) * sample_rate_;
        uint16_t segment_2= (static_cast<double>(log(1e-4)) / -decay_) * sample_rate_;
        uint16_t total = (segment_1+segment_2) * 1.1;
        return total;
    }
};
