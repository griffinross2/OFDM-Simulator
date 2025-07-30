#include "ofdm.h"

#include <fftw3.h>
#include "log_file.h"

FileManager g_log_file_manager;

namespace ofdm
{
    int modulate(std::complex<double> *input, std::complex<double> *output, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs, std::complex<double> pilot_value)
    {
        if (!input || !output || num_subcarriers <= 0)
        {
            throw std::invalid_argument("ofdm::modulate: Invalid input or output buffer or number of subcarriers!");
        }

        std::complex<double> freq_domain[num_subcarriers];

        // Initialize frequency domain with zeros
        for (int i = 0; i < num_subcarriers; i++)
        {
            freq_domain[i] = std::complex<double>(0, 0);
        }

        // Add data and pilot symbols to the frequency domain
        for (size_t i = 0; i < data_locs.size(); i++)
        {
            int idx = data_locs[i];
            if (idx >= 0 && idx < num_subcarriers)
            {
                freq_domain[idx] = input[i];
            }
            else
            {
                throw std::out_of_range("ofdm::modulate: Data location index out of range!");
            }
        }

        for (size_t i = 0; i < pilot_locs.size(); i++)
        {
            int idx = pilot_locs[i];
            if (idx >= 0 && idx < num_subcarriers)
            {
                freq_domain[idx] = pilot_value;
            }
            else
            {
                throw std::out_of_range("ofdm::modulate: Pilot location index out of range!");
            }
        }

        // Perform IFFT to convert frequency domain to time domain

        fftw_plan plan;
        plan = fftw_plan_dft_1d(num_subcarriers, reinterpret_cast<fftw_complex *>(freq_domain), reinterpret_cast<fftw_complex *>(output), FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(plan);
        fftw_destroy_plan(plan);

        for (int i = 0; i < num_subcarriers; i++)
        {
            // Normalize the output by the number of subcarriers
            output[i] /= static_cast<double>(num_subcarriers);

            g_log_file_manager.write_log("ofdm_modulate.log", "%g,%g\n", i, output[i].real(), output[i].imag());
        }

        return 0;
    }

    int demodulate(std::complex<double> *input, std::complex<double> *output_data, std::complex<double> *output_pilot, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs, std::complex<double> pilot_value)
    {
        if (!input || !output_data || !output_pilot || num_subcarriers <= 0)
        {
            throw std::invalid_argument("ofdm::demodulate: Invalid input or output buffer or number of subcarriers!");
        }

        std::complex<double> freq_domain[num_subcarriers];

        // Initialize freq domain with zeros
        for (int i = 0; i < num_subcarriers; i++)
        {
            freq_domain[i] = std::complex<double>(0, 0);
        }

        // Perform FFT to convert time domain to frequency domain
        fftw_plan plan;
        plan = fftw_plan_dft_1d(num_subcarriers, reinterpret_cast<fftw_complex *>(input), reinterpret_cast<fftw_complex *>(freq_domain), FFTW_FORWARD, FFTW_ESTIMATE);
        fftw_execute(plan);
        fftw_destroy_plan(plan);

        // Extract data and pilot symbols from the frequency domain
        for (size_t i = 0; i < data_locs.size(); i++)
        {
            int idx = data_locs[i];
            if (idx >= 0 && idx < num_subcarriers)
            {
                output_data[i] = freq_domain[idx];
            }
            else
            {
                throw std::out_of_range("ofdm::demodulate: Data location index out of range!");
            }
        }

        for (size_t i = 0; i < pilot_locs.size(); i++)
        {
            int idx = pilot_locs[i];
            if (idx >= 0 && idx < num_subcarriers)
            {
                output_pilot[i] = freq_domain[idx];
            }
            else
            {
                throw std::out_of_range("ofdm::demodulate: Pilot location index out of range!");
            }
        }

        for (int i = 0; i < num_subcarriers; i++)
        {
            g_log_file_manager.write_log("ofdm_demodulate.log", "%g,%g\n", i, freq_domain[i].real(), freq_domain[i].imag());
        }

        return 0;
    }
}