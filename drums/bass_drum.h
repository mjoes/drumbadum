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
            length_attack_t_ = 0.0; 
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
        length_attack_t_ = (1.0f/static_cast<float>(BD.attack_));
        length_attack_ = length_attack_t_ * sample_rate_;
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

        for (int i = 0; i < 3; ++i) {
            flutter_[i] = d(gen);
        }
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;

        int32_t sample;
        float t = static_cast<float>(rel_pos_) / sample_rate_;

        sample = GenerateSample(t);
        sample += GenerateHarmonics(t);
        sample *= BD.velocity_;
        sample *= GenerateEnv(t);
        int16_t output = Overdrive(sample, 1); // Apply distortion

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    float length_attack_t_;
    uint32_t rel_pos_;
    uint32_t end_i_;
    uint32_t length_decay_;
    uint16_t length_attack_;
    const uint16_t sample_rate_;
    bool running_;
    vector<int16_t> flutter_; 
    BassDrumSculpt BD;
    BassDrumEnv ENV;

    random_device rd{};
    mt19937 gen{rd()};
    normal_distribution<double> d{0, 1000};

    int16_t Overdrive(int32_t value, uint8_t dist_type) {
        int16_t clipped_value;
        int32_t overdriven_value = (value * BD.overdrive_);
        float scaled_overdriven_value = overdriven_value / 32767.0;
        switch (dist_type){
            case 1: // SOFT clipping 2
                if (overdriven_value <= -32767) {
                    clipped_value = -32767;
                } 
                else if (overdriven_value >= 32767) {
                    clipped_value = 32767;
                } 
                else {
                    clipped_value = 32767 * (scaled_overdriven_value - pow(scaled_overdriven_value,3)/3.0) * (3.0/2.0);
                }
                break;
            case 2: // HARD clipping
                clipped_value = 0.5 * (fabs(overdriven_value + 32767) - fabs(overdriven_value - 32767));
                break;
                
        }
        return clipped_value;
    }
            
    int16_t GenerateSample(float t) {
        int16_t sample;
        if (t > ENV.interval_) {
            sample = 32767 * sin(2 * M_PI * BD.frequency_ * t + ENV.phase_end_);
        } else {
            float func = ENV.slope_ * pow(t,2) / 2.0 + ENV.f0_ * t;
            sample = 32767 * sin(2.0 * M_PI * func);  
        }
        return sample;
    }

    int16_t GenerateHarmonics(float t) {
        int16_t sample;
        sample = 0.2 * 32767.0 * BD.harmonics_ * sin(2 * M_PI * (220.0 + flutter_[0]/125.0) * t + 0.5);
        sample += 0.3 * 32767.0 * BD.harmonics_ * sin(2 * M_PI * (BD.frequency_ * 7.8 + flutter_[1]/125.0) * t + 1.2);
        sample += 0.2 * 32767.0 * BD.harmonics_ * sin(2 * M_PI * (BD.frequency_ * 10.2 + flutter_[2]/125.0) * t + 2.1);
        return sample;
    }
    
    float GenerateEnv(float t) {
        float out_;
        if ( rel_pos_ >= length_attack_ ) {
            out_ = 1.0f * exp(-BD.decay_ * (t-length_attack_t_));
        } else {
            out_ = BD.attack_ * t;
        }
        return out_;
    }
};
