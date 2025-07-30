#include <stdio.h>
#include <random>
#include <time.h>
#include "test.h"
#include "../qam.h"
#include "../ofdm.h"
#include "../transmitter.h"
#include "cmath"
#include "../log_file.h"

#define LEN 128 * 48
#define NUM_SUBCARRIERS 64
#define NUM_DATA_SUBCARRIERS 48
#define FC 2.4e9
#define BW 20e6
#define FS 100e9
static_assert(LEN % 8 == 0, "LEN must be a multiple of 8");
static_assert(LEN % NUM_DATA_SUBCARRIERS == 0, "LEN must be a multiple of NUM_DATA_SUBCARRIERS");

void test_transmitter()
{
    srand(time(NULL));
    // srand(1);
    // Random data
    uint8_t data[LEN / 8];
    for (int i = 0; i < LEN / 8; i++)
    {
        data[i] = rand() % 0xFF;
    }

    double t = 0;

    std::vector<int> data_locs = {1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26, 38, 39, 40, 41, 42, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 61, 62, 63};
    std::vector<int> pilot_locs = {7, 21, 43, 57};
    Transmitter transmitter(FC, BW, qam::QAM16, NUM_SUBCARRIERS, data_locs, pilot_locs);
    FileManager log_file_manager;

    double sample = 0.0;
    while (1)
    {
        try
        {
            sample = transmitter.get_signal(data, LEN, t);
        }
        catch (const std::out_of_range &e)
        {
            break;
        }
        log_file_manager.write_log("transmitter.log", "%g\n", sample);
        t += 1.0 / FS;
    }
}