#include <stdio.h>
#include <random>
#include <time.h>
#include "test.h"
#include "../qam.h"
#include "../ofdm.h"
#include "cmath"

#define LEN 1024 * 48
#define NUM_SUBCARRIERS 64
#define NUM_DATA_SUBCARRIERS 48
static_assert(LEN % 8 == 0, "LEN must be a multiple of 8");
static_assert(LEN % NUM_DATA_SUBCARRIERS == 0, "LEN must be a multiple of NUM_DATA_SUBCARRIERS");

void test_ofdm_modulate()
{
    srand(time(NULL));
    // srand(1);
    // Random data
    uint8_t data[LEN / 8];
    for (int i = 0; i < LEN / 8; i++)
    {
        data[i] = rand() % 0xFF;
    }

    int len_qam = qam::modulate(qam::QPSK, data, nullptr, LEN);
    std::complex<double> qam_output[len_qam];
    qam::modulate(qam::QAM16, data, qam_output, LEN);

    std::complex<double> ofdm_output[len_qam / 48 * 64];
    std::vector<int> data_locs = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26, 38, 39, 40, 41, 42, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63};
    std::vector<int> pilot_locs = {7, 21, 43, 57};

    int i = 0;
    int o = 0;
    while (i < len_qam)
    {
        ofdm::modulate(qam_output + i, ofdm_output + o, NUM_SUBCARRIERS, data_locs, pilot_locs, std::complex<double>(1, 1));
        i += NUM_DATA_SUBCARRIERS;
        o += NUM_SUBCARRIERS;
    }
}