#pragma once

#include "qam.h"
#include <vector>
#include <complex>

class Transmitter
{
public:
    Transmitter(double fc, double bw, qam::modulation_t mod_type, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs);
    ~Transmitter();
    double get_signal(uint8_t *input, int input_len_bits, double time);

private:
    double fc;                                    // Carrier frequency
    double bw;                                    // Bandwidth
    qam::modulation_t mod_type;                   // Modulation type
    int num_subcarriers;                          // Number of subcarriers
    std::vector<int> data_locs;                   // Data subcarrier locations
    std::vector<int> pilot_locs;                  // Pilot subcarrier locations
    std::vector<std::complex<double>> ofdm_block; // Cached OFDM block
    int ofdm_index;                               // Index for the current OFDM block
};