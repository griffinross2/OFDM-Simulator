#pragma once

#include <complex>
#include <random>

class Channel
{
public:
    Channel(double tx_power_dbm, double tx_gain_db, double rx_gain_db, double dist_km, double fc, double bw);
    ~Channel();
    double apply_channel(double signal);

private:
    double tx_power_dbm;
    double tx_gain_db;
    double rx_gain_db;
    double dist_km;
    double fc;
    double bw;
    double signal_gain;
    double noise_power;
    std::random_device rd;
    std::mt19937 gen;
    std::normal_distribution<> dist;
};