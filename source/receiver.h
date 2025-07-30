#pragma once

#include "qam.h"
#include <vector>
#include <complex>
#include "transmitter.h"
#include "Iir.h"
#include "channel.h"
#include <queue>

class Receiver
{
public:
    Receiver(double fc, double bw, double fs, qam::modulation_t mod_type, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs);
    Receiver(Channel *channel, double fc, double bw, double fs, qam::modulation_t mod_type, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs);
    ~Receiver();
    void track(Transmitter &transmitter, uint8_t *input, int input_len_bits);

private:
    std::complex<double> get_baseband_sample(double sample, double time); // Convert sample to baseband

    Channel *channel;                       // Pointer to the channel object
    double fc;                              // Carrier frequency
    double bw;                              // Bandwidth
    double fs;                              // Sampling frequency for filter
    qam::modulation_t mod_type;             // Modulation type
    int num_subcarriers;                    // Number of subcarriers
    std::vector<int> data_locs;             // Data subcarrier locations
    std::vector<int> pilot_locs;            // Pilot subcarrier locations
    Iir::Butterworth::LowPass<5> i_filter; // FIR filter for signal processing (I)
    Iir::Butterworth::LowPass<5> q_filter; // FIR filter for signal processing (Q)
    double gain = 10000.0;                  // Current gain of the receiver
    double gain_int = 10000.0;              // Integral gain for AGC
    std::queue<int> sample_mag;             // Queue to store the magnitude of samples for gain adjustment
    int sum = 0;                            // Sum of the sample magnitudes for gain adjustment
};