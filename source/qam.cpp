#include "qam.h"
#include "tracy/Tracy.hpp"

int qam::modulate(modulation_t mod_type, uint8_t *input, std::complex<double> *output, int input_len_bits)
{
    ZoneScoped;

    // Check for valid input
    if (mod_type < BPSK || mod_type > QAM16)
    {
        throw std::invalid_argument("qam::modulate: Unknown modulation type!");
    }

    // Return necessary output buffer length
    if (output == nullptr)
    {
        switch (mod_type)
        {
        case BPSK:
            return input_len_bits / 1;
        case QPSK:
            return input_len_bits / 2;
        case QAM16:
            return input_len_bits / 4;
        }
    }

    // Perform modulation
    int i = 0;
    switch (mod_type)
    {
    case BPSK:
        // BPSK modulation
        while (i < input_len_bits)
        {
            output[i] = (input[i / 8] & (1 << (i % 8))) ? std::complex<double>(1, 0) : std::complex<double>(-1, 0);
            i += 1;
        }
        return input_len_bits - i;

    case QPSK:
        // QPSK modulation
        while (i < input_len_bits - 1)
        {
            int symbol = (input[i / 8] >> (i % 8)) & 0x3;
            switch (symbol)
            {
            case 0:
                output[i / 2] = std::complex<double>(0, 1);
                break;
            case 1:
                output[i / 2] = std::complex<double>(1, 0);
                break;
            case 2:
                output[i / 2] = std::complex<double>(0, -1);
                break;
            case 3:
                output[i / 2] = std::complex<double>(-1, 0);
                break;
            }
            i += 2;
        }
        return input_len_bits - i;

    case QAM16:
        // QAM16 modulation
        while (i < input_len_bits - 3)
        {
            int symbol = (input[i / 8] >> (i % 8)) & 0xF;
            switch (symbol)
            {
            case 0:
                output[i / 4] = std::complex<double>(-1, 1);
                break;
            case 1:
                output[i / 4] = std::complex<double>(-1.0 / 3.0, 1);
                break;
            case 2:
                output[i / 4] = std::complex<double>(1, 1);
                break;
            case 3:
                output[i / 4] = std::complex<double>(1.0 / 3.0, 1);
                break;
            case 4:
                output[i / 4] = std::complex<double>(-1, 1.0 / 3.0);
                break;
            case 5:
                output[i / 4] = std::complex<double>(-1.0 / 3.0, 1.0 / 3.0);
                break;
            case 6:
                output[i / 4] = std::complex<double>(1, 1.0 / 3.0);
                break;
            case 7:
                output[i / 4] = std::complex<double>(1.0 / 3.0, 1.0 / 3.0);
                break;
            case 8:
                output[i / 4] = std::complex<double>(-1, -1);
                break;
            case 9:
                output[i / 4] = std::complex<double>(-1.0 / 3.0, -1);
                break;
            case 10:
                output[i / 4] = std::complex<double>(1, -1);
                break;
            case 11:
                output[i / 4] = std::complex<double>(1.0 / 3.0, -1);
                break;
            case 12:
                output[i / 4] = std::complex<double>(-1, -1.0 / 3.0);
                break;
            case 13:
                output[i / 4] = std::complex<double>(-1.0 / 3.0, -1.0 / 3.0);
                break;
            case 14:
                output[i / 4] = std::complex<double>(1, -1.0 / 3.0);
                break;
            case 15:
                output[i / 4] = std::complex<double>(1.0 / 3.0, -1.0 / 3.0);
                break;
            }
            i += 4;
        }
        return input_len_bits - i;
    }

    return 0;
}