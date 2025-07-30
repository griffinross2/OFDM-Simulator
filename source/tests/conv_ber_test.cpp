#include <stdio.h>
#include "../convolutional_coder.h"
#include <random>
#include <time.h>
#include "test.h"

// static uint64_t s_polys[] = {0b1001111, 0b1101101};
static uint64_t s_polys[] = {0b1111001, 0b1011011};
// static uint64_t s_polys[] = {0b10110101, 0b11001111};

#define TRIALS 100000
#define TER 0.001 // Transmission bit error rate

// Each chunk =
//  2*188 byte TS packets
//  104 bytes of other data (e.g. IP)
//  total of 480 bytes
// Each encoded block =
//  96 bits = 12 bytes
//  representing 48 bits of the original data
//  meaning 80 blocks compose a chunk
// Interleaving
//  Put each of the 12 bytes in a row then read down columns
void test_conv_ber()
{
    ConvCoder coder(7, 2, s_polys, 2);

    int num_bit_errors = 0;

    srand(time(NULL));
    // srand(1);
    for (int trial = 0; trial < TRIALS; trial++)
    {
        // Random data
        uint8_t data[6];
        for (int i = 0; i < 6; i++)
        {
            data[i] = rand() % 0xFF;
        }
        data[5] &= 0x7F;

        uint8_t data_enc[12];
        coder.encode(data, data_enc, 47, 1);

        // Interleave
        uint8_t data_inlv[12];
        coder.interleave(data_enc, data_inlv, 96, 12, 8);

        // Introduce noise
        for (int i = 0; i < 12; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if (drand48() < TER)
                {
                    data_inlv[i] ^= (1 << j);
                }
            }
        }

        // Deinterleave
        uint8_t data_dnlv[12];
        coder.deinterleave(data_inlv, data_dnlv, 96, 12, 8);

        uint8_t data_out[6];
        coder.decode(data_dnlv, data_out, 48, 1);

        data_out[5] &= 0x7F;

        for (int i = 0; i < 6; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                if ((data[i] ^ data_out[i]) & (1 << j))
                {
                    num_bit_errors++;
                    printf("t:%d, by:%d, %02X -> %02X\n", trial, i, data[i], data_out[i]);
                }
            }
        }
    }

    printf("TER: %3.10f%%\n", TER * 100.0);
    printf("BER: %3.10f%%\n", (double)num_bit_errors / (TRIALS * 6 * 8) * 100.0);
}