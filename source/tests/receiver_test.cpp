#include <stdio.h>
#include <random>
#include <time.h>
#include "test.h"
#include "../qam.h"
#include "../ofdm.h"
#include "../receiver.h"
#include "../channel.h"
#include "cmath"
#include "../log_file.h"

#define LEN 256 * 48
#define NUM_SUBCARRIERS 64
#define NUM_DATA_SUBCARRIERS 48
#define FC 2.4e9
#define BW 20e6
#define FS 100e9
#define DIST_KM 20.0
#define TX_POWER_DBM 30.0
#define TX_GAIN_DB 0.0
#define RX_GAIN_DB 15.0
static_assert(LEN % 8 == 0, "LEN must be a multiple of 8");
static_assert(LEN % NUM_DATA_SUBCARRIERS == 0, "LEN must be a multiple of NUM_DATA_SUBCARRIERS");

void test_receiver()
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
    Transmitter transmitter(FC, BW, qam::QPSK, NUM_SUBCARRIERS, data_locs, pilot_locs);
    Channel channel(TX_POWER_DBM, TX_GAIN_DB, RX_GAIN_DB, DIST_KM, FC, BW);
    Receiver receiver(&channel, FC, BW, FS, qam::QPSK, NUM_SUBCARRIERS, data_locs, pilot_locs);
    FileManager log_file_manager;

    receiver.track(transmitter, data, LEN);
}