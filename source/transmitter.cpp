#include "transmitter.h"
#include "ofdm.h"
#include "qam.h"
#include <cmath>
#include "log_file.h"

static FileManager s_log_file_manager;

Transmitter::Transmitter(double fc, double bw, qam::modulation_t mod_type, int num_subcarriers, std::vector<int> &data_locs, std::vector<int> &pilot_locs)
    : fc(fc), bw(bw), mod_type(mod_type), num_subcarriers(num_subcarriers), data_locs(data_locs), pilot_locs(pilot_locs)
{
    // Reserve space for one OFDM fft output (to cache it)
    ofdm_block.reserve(num_subcarriers);
    ofdm_index = -1;
}

Transmitter::~Transmitter()
{
}

double Transmitter::get_signal(uint8_t *input, int input_len_bits, double time)
{
    // Determine the number of the OFDM block corresponding to the desired sample timestamp
    int block_number = static_cast<int>(time * bw / num_subcarriers);

    // If the OFDM block is not cached, generate it
    if (ofdm_index != block_number)
    {
        // Determine the index of input data to start from and number of input bits
        int index = block_number * data_locs.size() / 8;
        int len_bits = data_locs.size();
        switch (mod_type)
        {
        case qam::BPSK:
            index *= 1; // BPSK uses 1 bit per symbol
            len_bits *= 1;
            break;
        case qam::QPSK:
            index *= 2; // QPSK uses 2 bits per symbol
            len_bits *= 2;
            break;
        case qam::QAM16:
            index *= 4; // QAM16 uses 4 bits per symbol
            len_bits *= 4;
            break;
        }

        // Check range
        if (index * 8 + len_bits > input_len_bits)
        {
            throw std::out_of_range("Transmitter::get_signal: Input data length exceeds available bits!");
        }

        // QAM modulate then OFDM modulate
        std::complex<double> qam_output[data_locs.size()];
        qam::modulate(mod_type, input + index, qam_output, len_bits);
        ofdm::modulate(qam_output, ofdm_block.data(), num_subcarriers, data_locs, pilot_locs, mod_type > qam::QPSK ? std::complex<double>(1, 1) : std::complex<double>(1, 0));

        ofdm_index = block_number;
    }

    // Find the index in the OFDM block corresponding to the sample timestamp, return the sample
    int sample_index = static_cast<int>(time * bw) % num_subcarriers;
    double i_part = ofdm_block[sample_index].real() * std::cos(2 * M_PI * fc * time);
    double q_part = ofdm_block[sample_index].imag() * -1 * std::sin(2 * M_PI * fc * time);

    s_log_file_manager.write_log("transmitter.log", "%g,%g\n", time, i_part + q_part);

    return i_part + q_part;
}