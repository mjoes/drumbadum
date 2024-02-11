#include <cstdint>
#include <cmath>
#include <stdio.h>

class BassDrum {
public:
    BassDrum() {}
    ~BassDrum() {}

    void Init(uint16_t sample_rate) {
        sample_rate_ = sample_rate;
        start_ = 0;
        out_ = 0.0f;
        seg_tmp_ = 0.0f;
        segment_ = 1;
    }

    void set_frequency(uint16_t frequency) {
        frequency_ = frequency;
        phase_ = static_cast<float>(frequency_) * (2.0f * 3.14159f) / sample_rate_;
    }

    void set_decay(uint16_t decay) {
        decay_ = decay;
    }

    void Process(const uint8_t gate_flag, int16_t* output, size_t size) {
        for (size_t i = 0; i < size; ++i) {
            // Generate waveform sample
            int16_t sample = GenerateSample(gate_flag, i);
            float env = GenerateEnv(gate_flag, i);
            start_ += 1;
            // Clip sample if necessary
            output[i] = static_cast<int16_t>(sample * env);
        }
    }

private:
    uint16_t frequency_;
    float phase_;
    uint16_t decay_;
    uint16_t sample_rate_;
    size_t start_;
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
};
