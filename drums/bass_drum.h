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
    }

    void set_overdrive(uint16_t overdrive) {
        overdrive_ = overdrive / 200.0f + 1.0f;
    }

    void set_decay(uint16_t decay) {
        decay_ = decay;
    }

    void set_envelope(uint16_t envelope) {
        // Assuming envelope takes value between 0 and 1024
        f0_ = frequency_ * envelope / 20; // 20 is for a kick drum range, might want to play around with this
        interval_ = 0.05; // TODO: This needs tuning
        slope_ = (frequency_ - f0_) / interval_;
        phase_end_ = -2 * M_PI * (f0_ + frequency_ ) / 2 * interval_;
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
            float sample = GenerateSample(rel_pos_);
            float env = GenerateEnv(rel_pos_);
            float out_val = sample * env;
    
            out_val = Overdrive(out_val, 1); // Apply distortion

            rel_pos_ += 1;
            return out_val * 32767;                
        }
        return 0;
    }

private:
    uint16_t frequency_;
    float phase_;
    uint8_t cross_fade_;
    uint16_t f0_;
    float interval_;
    float slope_;
    float phase_end_;
    uint16_t decay_;
    uint16_t sample_rate_;
    uint16_t envelope_;
    float overdrive_;
    uint16_t attack_;
    size_t rel_pos_;
    size_t start_i_;
    size_t end_i_;
    float out_;
    float seg_tmp_;
    uint8_t segment_;

    float Overdrive(float value, uint8_t dist_type) {
        float clipped_value;
        float overdriven_value = (value * overdrive_);
        switch (dist_type){
            case 0: // SOFT clipping 1
                // This algorithm is not ideal....
                clipped_value = (2.0f / M_PI) * atan(overdriven_value);
                break;
            case 1: // SOFT clipping 2
                if (overdriven_value <= -1.0) {
                    clipped_value = -1.0;
                } 
                else if (overdriven_value >= 1.0) {
                    clipped_value = 1.0;
                } 
                else {
                    clipped_value = (overdriven_value - pow(overdriven_value,3)/3.0) * (3.0/2.0);
                }
                break;
            case 2: // HARD clipping
                clipped_value = 0.5 * (fabs(overdriven_value + 1.0) - fabs(overdriven_value - 1.0));
                break;
                
        }
        return clipped_value;
    }
            
    float GenerateSample(size_t rel_pos_samp) {
        float t = static_cast<float>(rel_pos_samp) / sample_rate_;
        float sample;
        if (t > interval_) {
            sample = sin(2 * M_PI * frequency_ * t + phase_end_);
        } else {
            float func = slope_ * pow(t,2) / 2.0 + f0_ * t;
            sample = sin(2.0 * M_PI * func);  
        }
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
