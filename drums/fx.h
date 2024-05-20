#include <cstdint>

class FX {
public:
    FX(
        uint16_t sample_rate,
        uint16_t nr_samples_l,
        uint16_t nr_samples_r,
        mt19937& gen)
        :
        sample_rate_(sample_rate),
        in_samples_l(nr_samples_l),
        in_samples_r(nr_samples_r),
        gen_(gen),
        
        in_buffer_l(in_samples_l, 0), 
        in_buffer_r(in_samples_r, 0), 
        buffer_index_l(0),
        buffer_index_r(0)
        // dis(-1,1)
        {
            // in_samples = 480;
        }
    ~FX() {}

    void Process(int16_t &out_l, int16_t &out_r, int16_t sample) {
        int16_t out;
        out = sample;
        // out = bitCrush(sample, 20);
        combFilter(out_l, out_r, out);
    }

private:
    const uint16_t sample_rate_, in_samples_l, in_samples_r;
    mt19937& gen_;
    std::vector<int16_t> in_buffer_l;
    std::vector<int16_t> in_buffer_r;
    uint16_t buffer_index_l, buffer_index_r;
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

    void combFilter(int16_t &out_l, int16_t &out_r, int16_t sample) {
        int16_t read_index_l = (buffer_index_l + 1) % in_samples_l;
        int16_t read_index_r = (buffer_index_r + 1) % in_samples_r;
        int16_t in_prev_l = in_buffer_l[read_index_l];
        int16_t in_prev_r = in_buffer_r[read_index_r];
        uint8_t g_1 = 2; 

        out_l = sample + in_prev_l / g_1;
        out_r = sample + in_prev_r / g_1;

        in_buffer_l[buffer_index_l] = sample;
        in_buffer_r[buffer_index_r] = sample;
        buffer_index_l = (buffer_index_l + 1) % in_samples_l;
        buffer_index_r = (buffer_index_r + 1) % in_samples_r;
    }
};
