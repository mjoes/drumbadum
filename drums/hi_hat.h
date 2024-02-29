#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>
#include <functional>

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

    typedef float (HiHat::*MemberFunctionPtr)(float);

    void set_decay(uint16_t decay, bool decay_type = 0) {
        decay_type_ = decay_type;
        if (decay_type_ == 0) {
            decay_ = 52-(decay/20+1);
            length_decay_ = static_cast<float>(log(1e-4)) / -decay_ * sample_rate_;
            envPtr = &HiHat::_exp_decay;
        } else {
            length_decay_ = decay * sample_rate_ / 1000;
            envPtr = &HiHat::_hard_decay;
        }
    }

    void set_start() {
        rel_pos_ = 0;
        running_ = true;
        end_i_ = length_decay_;
    }

    int16_t Process() {
        // Generate waveform sample
        if (running_ == false)
            return 0;
            
        int32_t sample;
        float t = static_cast<float>(rel_pos_) / sample_rate_;

        sample = GenerateSample();
        sample *= (this->*envPtr)(t);
        int16_t output = sample;

        rel_pos_ += 1;
        if (rel_pos_ >= end_i_) {
            running_ = false;
        }
        return output;           
    }

private:
    uint32_t rel_pos_;
    uint32_t end_i_;
    uint32_t length_decay_;
    uint16_t decay_;
    uint16_t sample_rate_;
    bool running_;
    bool decay_type_;
    vector<int16_t> flutter_; 
    MemberFunctionPtr envPtr; 

    random_device rd;
    mt19937 gen;
    uniform_int_distribution<int32_t> dis;

    int32_t GenerateSample() {
        // Replace this with your own waveform generation logic
        int32_t sample = dis(gen);
        return sample;
    }

    float _exp_decay(float t) {
        return 1.0f * exp(-decay_ * t);
    }

    float _hard_decay(float t) {
        return 1.0;
    }
};

