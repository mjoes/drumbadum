#include <cstdint>
#include <cmath>
#include <stdio.h>

class BassDrum {
public:
    BassDrum() {}
    ~BassDrum() {}

    void Init(
        uint16_t sample_rate, 
        size_t start = 0, 
        float out = 0.0f, 
        float seg_tmp = 0.0f, 
        uint8_t segment = 1) 
    {
        sample_rate_ = sample_rate;
        start_ = start;
        out_ = out;
        seg_tmp_ = seg_tmp;
        segment_ = segment;
    }

    void set_frequency(uint16_t frequency) {
        frequency_ = frequency;
        phase_ = static_cast<float>(frequency_) * (2.0f * 3.14159f) / sample_rate_;
    }

    void set_decay(uint16_t decay) {
        decay_ = decay;
    }

    void set_start(size_t start) {
        start_ = 0;
        start_i_ = start;
        end_i_ = start + lengthHit();
    }

    void Process(const uint8_t gate_flag, int16_t* output, size_t size, size_t it) {
        for (size_t i = 0; i < size; ++i) {
            // Generate waveform sample
            if (it > start_i_ && it < end_i_) {
                int16_t sample = GenerateSample(gate_flag, i);
                float env = GenerateEnv(gate_flag, i);
                start_ += 1;
                // Clip sample if necessary
                output[i] = static_cast<int16_t>(sample * env);
            }
        }
    }

private:
    uint16_t frequency_;
    float phase_;
    uint16_t decay_;
    uint16_t sample_rate_;
    size_t start_;
    size_t start_i_;
    size_t end_i_;
    float out_;
    float seg_tmp_;
    uint8_t segment_;
    
    int32_t GenerateSample(uint8_t gate_flag, size_t size) {
        int32_t sample = 0;
        if (gate_flag) {
            // Replace this with your own waveform generation logic
            sample = static_cast<int32_t>(32767.0f * sin(phase_ * start_)); // 32767 is for PCM waves
        }
        return sample;
    }

    float GenerateEnv(uint8_t gate_flag, size_t size) {
        float start_tmp = static_cast<float>(start_)/sample_rate_;
        if (gate_flag) {
            switch (segment_) {
                case 1: 
                    out_ = 10.0f * start_tmp;
                    if (out_ >= 1) {
                        segment_ = 2;
                        seg_tmp_ = start_tmp;
                    }
                    break;
                case 2:
                    out_ = 1.0f * exp(-decay_ * (start_tmp-seg_tmp_));
                    if (out_ <= 1e-4) {
                        segment_ = 3;
                    }
                    break;
                case 3:
                    out_ = 0.0f;
                    break;
            }
        }
        return out_;
    }

    uint16_t lengthHit() {
        uint16_t segment_1= (1.0f/10.0) * sample_rate_;
        uint16_t segment_2= (static_cast<double>(log(1e-4)) / -decay_) * sample_rate_;
        uint16_t total = (segment_1+segment_2) * 1.1;
        return total;
    }
};
