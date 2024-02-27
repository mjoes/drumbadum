#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <algorithm>
#include <random>

using namespace std;

struct BassDrumSculpt {
    float overdrive_;
    float velocity_;
    float harmonics_;
    uint16_t frequency_;
    uint16_t attack_;
    uint8_t decay_;
};

struct BassDrumEnv {
    float interval_;
    float phase_;
    float slope_;
    float phase_end_;
    uint16_t f0_;
};


class BassDrum {
public:
    BassDrum(
        uint16_t sample_rate) 
        : 
        sample_rate_(sample_rate),
        flutter_(3)
        {
            rel_pos_ = 0; 
            seg_tmp_ = 0.0f; 
            segment_ = 1; 
        }
    ~BassDrum() {}

    void set_frequency(uint16_t frequency) {
        BD.frequency_ = frequency;
    }

    void set_overdrive(uint16_t overdrive) {
        BD.overdrive_ = overdrive / 200.0f + 1.0f;
    }

    void set_velocity(uint16_t velocity) {
        BD.velocity_ = velocity / 1000.0;
    }

    void set_decay(uint16_t decay) {
        BD.decay_ = 52-(decay/20+1);
        length_decay_ = static_cast<float>(log(1e-4)) / -BD.decay_ * sample_rate_;
    }

    void set_attack(uint16_t attack) {
        BD.attack_ = 1001 - attack;
        length_attack_ = (1.0f/static_cast<float>(BD.attack_)) * sample_rate_;
    }

    void set_harmonics(uint16_t harmonics) {
        BD.harmonics_ = static_cast<float>(harmonics) / 1000;
    }

    void set_envelope(uint16_t envelope) {
        ENV.f0_ = BD.frequency_ * envelope / 20; // 20 is for a kick drum range, might want to play around with this
        ENV.interval_ = 0.05; // TODO: This needs tuning
        ENV.slope_ = (BD.frequency_ - ENV.f0_) / ENV.interval_;
        ENV.phase_end_ = -2 * M_PI * (ENV.f0_ + BD.frequency_ ) / 2 * ENV.interval_;
    }

    void set_start() {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_attack_ + length_decay_;
        segment_ = 1;

        for (int i = 0; i < 3; ++i) {
            flutter_[i] = d(gen);
        }
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        float sample = GenerateSample(rel_pos_);
        float env = GenerateEnv(rel_pos_);
        float out_val = sample * env;
        out_val = Overdrive(out_val, 1); // Apply distortion

        rel_pos_ += 1;
        
        
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }

        return out_val * 32767;                
    }

private:
    float seg_tmp_;
    bool running_;
    uint32_t rel_pos_;
    uint32_t end_i_;
    uint32_t length_decay_;
    uint16_t length_attack_;
    uint16_t sample_rate_;
    uint8_t segment_;
    vector<int16_t> flutter_; 
    BassDrumSculpt BD;
    BassDrumEnv ENV;
    
    random_device rd{};
    mt19937 gen{rd()};
    normal_distribution<double> d{0, 1000};

    float Overdrive(float value, uint8_t dist_type) {
        float clipped_value;
        float overdriven_value = (value * BD.overdrive_);
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
        if (t > ENV.interval_) {
            sample = sin(2 * M_PI * BD.frequency_ * t + ENV.phase_end_);
        } else {
            float func = ENV.slope_ * pow(t,2) / 2.0 + ENV.f0_ * t;
            sample = sin(2.0 * M_PI * func);  
        }
        sample += GenerateHarmonics(t);
        return sample * BD.velocity_;
    }

    float GenerateHarmonics(float t) {
        float sample = 0.0;
        sample += 0.2 * BD.harmonics_ * sin(2 * M_PI * (220.0 + flutter_[0]/125.0) * t + 0.5);
        sample += 0.3 * BD.harmonics_ * sin(2 * M_PI * (BD.frequency_ * 7.8 + flutter_[1]/125.0) * t + 1.2);
        sample += 0.2 * BD.harmonics_ * sin(2 * M_PI * (BD.frequency_ * 10.2 + flutter_[2]/125.0) * t + 2.1);
        return sample;
    }

    float GenerateEnv(size_t rel_pos_env) {
        float rel_pos_tmp = static_cast<float>(rel_pos_env)/sample_rate_;
        float out_;
        switch (segment_) {
            case 1: 
                out_ = BD.attack_ * rel_pos_tmp;
                if (out_ >= 1) {
                    segment_ = 2;
                    seg_tmp_ = rel_pos_tmp;
                }
                break;
            case 2:
                out_ = 1.0f * exp(-BD.decay_ * (rel_pos_tmp-seg_tmp_));
                break;
        }
    
        return out_;
    }

};
