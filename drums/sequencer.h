#include "rhythmic_pattern.h"
#include "global.h"
#include <random>

class Sequencer {
public:
    Sequencer() = default;
    ~Sequencer() = default;
    bool hits[3] = {};
    bool accent[3] = {};
    bool FX_flag = false;
    bool LED_flag = false;
    uint32_t stutter_samples[2] = {};

    void reset_hits() {
        for (int i = 0; i < 3; ++i) {
            hits[i] = 0; // Access each element using array subscript notation
        }
    }

    void set_stutter_samples(uint32_t steps_sample) {
        stutter_samples[0] = steps_sample;
        stutter_samples[1] = steps_sample / 2;
    }

    void run_sequencer(bool sync, bool runnn, uint16_t steps_sample){
        bool active_seq = true;
        FX_flag = false;
        if (sync == true){
            if ((step + 1) % 4 == 1 && reset_step_sample == false){
                // Stop sequencer until it gets the start flag
                active_seq = false;
            }
            if (reset_step_sample == true){ 
                step_sample = steps_sample;
                reset_step_sample = false;
            }
        }

        if (step_sample % stutter_sample == 0 && stutter_flag == true) {
            hits[0] = stutter[0];
            hits[1] = stutter[1];
            hits[2] = stutter[2];
            stutters_left--;
            if (stutters_left == 0) {
                stutter_flag = false;
            }
        }

        if (step_sample == steps_sample && active_seq == true){
            if (pot_seq_turing < 30) {
                turing_hit_low(pot_seq_turing);
            } else if (pot_seq_turing > 70) {
                turing_hit_high(pot_seq_turing);
            } else if (stutter_flag == false) {
                normal_hit();
            }

            set_stutter(step, runnn);

            step_sample = 0;
            ++step;
            if (step > 15) {
                step = 0;
            }
            if ((rand() % 100) < pot_xtra ) {
                FX_flag = true;
            }
        }
        ++step_sample;
    }

private:
    uint16_t stutter_sample = 1;
    uint8_t stutters_left = 0;
    bool stutter[3] = {};
    bool stutter_flag;
    int16_t seq_buffer[3][16] = {};

    // Stutter & LED
    void set_stutter(uint8_t step, bool runn) {
        if ((step + 1) % 4 == 1 && runn == true) {
        	LED_flag = true; // For the BPM led
        	hits[1] = 1;

            // // pot_xtra defines probability of stutter between 0 and 0.1 based on pot_xtra
            stutter_flag = (rand() % 100) < (pot_xtra / 7);

            if (stutter_flag){
                stutters_left = (rand() % 4) + 1; // How many stutters in next cycle
                uint16_t index = (rand() % 2); // either 16 or 32th stutters
                stutter_sample = stutter_samples[index];
                for (int j = 0; j < 3; ++j) {
                    stutter[j] = hits[j]; // Save current hit for the stutter
                }
            }
        } else {
        	LED_flag = false;
        }
    }

    inline void turing_hit_low(uint8_t val) {
        if ((rand() % 100) > (val * 3 / 5)) {
            for (int i = 0; i < 3; ++i) {
                hits[i] = seq_buffer[i][step];
            }
        } else {
            normal_hit();
        }
    }

    inline void turing_hit_high(uint8_t val) {
        if ((rand() % 100) > ((100 - val) * 3 / 5)) {
            for (int i = 0; i < 3; ++i) {
                hits[i] = seq_buffer[i][step];
            }
        } else {
            normal_hit();
        }
    }

    inline void normal_hit() {
        if (rhythms[pot_seq_1][step] == true){
            drum_hit(pot_seq_2,pot_seq_3,step, hits, accent);
        }
        else {
            chance_drum_hit(pot_seq_2, pot_seq_3, pot_seq_rd, step, hits, accent);
        }
        artifacts_hit(pot_seq_2, pot_seq_rd, pot_seq_art, step, hits, accent);

        // Save hits for "turing machine"
        for (int i = 0; i < 3; ++i) {
            seq_buffer[i][step] = hits[i];
        }
    }

    void drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t step, bool* hits, bool* accent) {
        // Probability of hits based on difference between pattern 1 and 2 is not equally distributed:
        // 4/9 for kick
        // 3/9 for fm hit
        // 2/9 for hihat 
        uint8_t hit_1 = patterns[knob_1][step];
        uint8_t hit_2 = patterns[50 - knob_2][step];
        int16_t output = abs(hit_1 * 3 / 100 - hit_2 * 3 / 100);
        if (bernoulli_draw(patterns[knob_1][step])) { 
            uint8_t hit_2_id = hit_2 * 3 / 100;
            hits[hit_2_id] = 1;
            accent[hit_2_id] = 0;
        }
        hits[output] = 1;
        accent[output] = 1;
    }

    void chance_drum_hit(uint8_t knob_1, uint8_t knob_2, uint8_t knob_rd, uint8_t step, bool* hits, bool* accent) {
        uint8_t prob = patterns[knob_1][step];
        if (rand() % 100 < prob - 10) { // tweak the prob-10 for how bare bones the beat should be
            uint8_t hit = patterns[knob_2][step] / 5;
            uint8_t output = prob_hat[hit]; // Bias to hat in spite of FM hit
            if (rand() % 100 < knob_rd) { 
                output += rand() % 3;
                output %= 3;
            }
            hits[output] = 1;
            accent[output] = 0;
        } 
    }

    void artifacts_hit(uint8_t knob_1, uint8_t knob_rd, uint8_t knob_art, uint8_t step, bool* hits, bool* accent) {
        uint8_t sample = rand() % 100;
        if (sample < knob_art) {
            uint8_t output = patterns[knob_1][16-step] * 3 / 100;
            uint8_t sample_rd = rand() % 100;
            if (sample_rd < knob_rd) { 
                output += rand() % 3;
                output %= 3;
            }
            hits[output] = 1;
            accent[output] = 0;
        }
    }
    
};
