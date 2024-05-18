#include <cstdint>

class FX {
public:
    FX(
        uint16_t sample_rate,
        uint16_t in_samples,
        mt19937& gen)
        :
        sample_rate_(sample_rate),
        in_samples_(in_samples),
        gen_(gen),
        
        in_buffer_(in_samples, 0), 
        buffer_index_(0)
        // dis(-1,1)
        {
            // in_samples = 480;
        }
    ~FX() {}

    int16_t Process(int16_t sample) {
        int16_t out;
        // out = bitCrush(sample, 20);
        out = combFilter(sample);
        return out;
    }

private:
    const uint16_t sample_rate_, in_samples_;
    mt19937& gen_;
    std::vector<int16_t> in_buffer_;
    uint16_t buffer_index_;
    // uniform_int_distribution<int32_t> dis;

    int16_t bitCrush(int16_t sample, uint8_t depth) {
        // depth between 1-50
        int32_t out;
        out = (sample + 32768) * depth / 65536;
        // printf("%i, %i, ", sample, out);
        out = (out * 65536) / depth;
        // printf("%i, ", out);
        out = static_cast<int16_t>(out - 32768);
        // printf("%i\n", out);
        return out;
    }

    int16_t combFilter(int16_t sample) {
        int16_t read_index = (buffer_index_ + 1) % in_samples_;
        int16_t in_prev = in_buffer_[read_index];
        uint8_t g_1 = 2; // inverse of 0.5 // 0.125 ;// 0.5^3
        int16_t out = sample + in_prev / g_1;// - g_2 * y_prev;
        in_buffer_[buffer_index_] = sample;
        buffer_index_ = (buffer_index_ + 1) % in_samples_;

        return out;
    }
};
