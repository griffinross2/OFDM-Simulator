#include "receiver.h"

#include <complex>
#include "ofdm.h"
#include <cmath>
#include <gnuradio/filter/fir_filter.h>
#include <gnuradio/filter/firdes.h>
#include <limits>
#include "tracy/Tracy.hpp"

Receiver::Receiver(double fc, double bw, double fs, qam::modulation_t mod_type, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs)
    : fc(fc), bw(bw), fs(fs), mod_type(mod_type), num_subcarriers(num_subcarriers), data_locs(data_locs), pilot_locs(pilot_locs)
{
    i_filter.setup(fs, fc * 0.5);
    q_filter.setup(fs, fc * 0.5);
    channel = nullptr; // Initialize channel pointer to null
}

Receiver::Receiver(Channel *channel, double fc, double bw, double fs, qam::modulation_t mod_type, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs)
    : channel(channel), fc(fc), bw(bw), fs(fs), mod_type(mod_type), num_subcarriers(num_subcarriers), data_locs(data_locs), pilot_locs(pilot_locs)
{
    i_filter.setup(fs, fc * 0.5);
    q_filter.setup(fs, fc * 0.5);
}

Receiver::~Receiver()
{
}

std::complex<double> Receiver::get_baseband_sample(double sample, double time)
{
    ZoneScoped;
    // Vector demodulation and filtering
    std::complex<double> baseband;
    double baseband_i;
    double baseband_q;
    baseband = sample * std::complex<double>(std::cos(2 * M_PI * fc * time), -1 * std::sin(2 * M_PI * fc * time));
    baseband_i = baseband.real();
    baseband_q = baseband.imag();
    baseband_i = i_filter.filter(baseband_i);
    baseband_q = q_filter.filter(baseband_q);
    baseband.real(baseband_i);
    baseband.imag(baseband_q);
    return baseband;
}

void Receiver::track(Transmitter &transmitter, uint8_t *input, int input_len_bits)
{
    ZoneScoped;

    double time = 0.0;

    double sample;
    std::complex<double> baseband;
    double baseband_i;
    double baseband_q;
    bool sampled = false;
    std::complex<double> input_data[num_subcarriers];
    std::complex<double> output_data[data_locs.size()];
    std::complex<double> output_pilot[pilot_locs.size()];
    int input_idx = 0;

    while (1)
    {
        FrameMark;
        try
        {
            sample = transmitter.get_signal(input, input_len_bits, time);

            // Simulate the channel if it exists
            if (channel)
            {
                sample = channel->apply_channel(sample);
            }

            // AGC
            sample *= gain;
            sample_mag.push(std::abs(sample) > 0.5 ? 1 : 0);
            sum += (std::abs(sample) > 0.5 ? 1 : 0);

            // Scale gain by ratio to setpoint
            double err = 220 - sum;
            gain_int += err * 0.00001; // Integral gain adjustment
            gain = gain_int + err * 0.00001;
            if (gain < 1.0)
            {
                gain = 1.0; // Prevent gain from going too low
            }
            else if (gain > 10.0e6)
            {
                gain = 10.0e6; // Prevent gain from going too high
            }

            // printf("Sample: %g, Gain: %g, Sum: %d\n", sample, gain, sum);

            if (sample_mag.size() >= 8192)
            {
                sum -= sample_mag.front();
                sample_mag.pop();
            }

            // Vector demodulation and filtering
            baseband = get_baseband_sample(sample, time);
        }
        catch (const std::out_of_range &e)
        {
            // Handle the case where the transmitter does not have more samples
            break;
        }

        if (!sampled && ((time * bw) - std::floor(time * bw)) > 0.5)
        {
            // Sample the input data
            input_data[input_idx] = baseband;

            // Demodulate once 64 samples are collected
            if (input_idx == (num_subcarriers - 1))
            {

                // OFDM demodulation
                ofdm::demodulate(input_data, output_data, output_pilot, num_subcarriers, data_locs, pilot_locs, std::complex<double>(1.0, 0.0));

                std::complex<double> pilot = output_pilot[0];

                double angle = std::arg(pilot);
                double err = angle; // - M_PI / 4; // Adjust for QPSK phase offset
                // fc -= 100 * err;
                // bw = fc * (20.0 / 2400.0);

                printf("Pilot value: %g + %gi, angle: %g, error: %g\n", pilot.real(), pilot.imag(), angle, err);
            }

            sampled = true;
            input_idx = (input_idx + 1) % num_subcarriers;
        }
        else if (((time * bw) - std::floor(time * bw)) <= 0.5)
        {
            // Reset the sample flag if we have not sampled this time slot yet
            sampled = false;
        }

        time += 1.0 / fs;

    }
}