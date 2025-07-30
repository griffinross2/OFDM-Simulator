#pragma once

#include <complex>
#include <vector>

namespace ofdm
{
    int modulate(std::complex<double> *input, std::complex<double> *output, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs, std::complex<double> pilot_value);
    int demodulate(std::complex<double> *input, std::complex<double> *output_data, std::complex<double> *output_pilot, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs, std::complex<double> pilot_value);
}