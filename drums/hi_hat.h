#include <cstdint>
#include <cmath>
#include <stdio.h>
#include <random>

using namespace std;
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<int32_t> dis(-32767, 32767);

class HiHat {
public:
    HiHat() {}
    ~HiHat() {}

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

    void set_decay(uint16_t decay) {
        decay_ = decay;
    }

    void set_start(size_t start) {
        rel_pos_ = 0;
        start_i_ = start;
        end_i_ = start + lengthHit();
    }

    int16_t Process(size_t it) {
        // Generate waveform sample
        if (it > start_i_ && it < end_i_) {
            int16_t sample = GenerateSample();
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
    size_t rel_pos_;
    size_t start_i_;
    size_t end_i_;
    float out_;
    float seg_tmp_;
    uint8_t segment_;
    
    int32_t GenerateSample() {
        // Replace this with your own waveform generation logic
        int32_t sample = dis(gen);
        return sample;
    }

    float GenerateEnv(size_t rel_pos_env) {
        float rel_pos_tmp = static_cast<float>(rel_pos_env)/sample_rate_;
        switch (segment_) {
            case 1:
                out_ = 1.0f * exp(-decay_ * (rel_pos_tmp-seg_tmp_));
                if (out_ <= 1e-4) {
                    segment_ = 2;
                }
                break;
            case 2:
                out_ = 0.0f;
                break;
        }
    
        return out_;
    }

    uint16_t lengthHit() {
        uint16_t segment_1 = (static_cast<double>(log(1e-4)) / -decay_) * sample_rate_;
        uint16_t total = (segment_1) * 1.1;
        return total;
    }
};
