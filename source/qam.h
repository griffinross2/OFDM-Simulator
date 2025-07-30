#pragma once

#include <complex>
#include <stdint.h>

namespace qam
{
    typedef enum
    {
        BPSK,
        QPSK,
        QAM16,
    } modulation_t;

    // QAM-modulate the input bitstream according the mod_type, and return the number of input symbols that were leftover.
    int modulate(modulation_t mod_type, uint8_t *input, std::complex<double> *output, int input_len_bits);
}