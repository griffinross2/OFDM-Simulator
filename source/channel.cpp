#include "channel.h"
#include <random>
#include "tracy/Tracy.hpp"

Channel::Channel(double tx_power_dbm, double tx_gain_db, double rx_gain_db, double dist_km, double fc, double bw)
    : tx_power_dbm(tx_power_dbm), tx_gain_db(tx_gain_db), rx_gain_db(rx_gain_db), dist_km(dist_km), fc(fc), bw(bw)
{
    // Free space path loss (FSPL) in dB
    double fspl = 20.0 * log10(dist_km * 1000.0) + 20.0 * log10(fc) + 20.0 * log10(4.0 * M_PI / 3.0e8);

    // Calculate signal gain in dB
    signal_gain = tx_power_dbm + tx_gain_db + rx_gain_db - fspl;

    // Noise power from bandwidth
    noise_power = -174.0 + 10.0 * log10(bw);

    gen = std::mt19937(rd());
    dist = std::normal_distribution<>(0.0, std::sqrt(std::pow(10.0, noise_power / 10.0)));

    // Display Stats
    printf("Channel Parameters:\n");
    printf("  FSPL: %.2f dB\n", fspl);
    printf("  Noise Power: %.2f dBm\n", noise_power);
    printf("  SNR: %.2f dB\n", signal_gain - noise_power);
}

Channel::~Channel()
{
}

double Channel::apply_channel(double signal)
{
    ZoneScoped;

    // Generate noise voltage
    double noise = dist(gen);

    // Apply signal gain and noise
    return (signal * std::pow(10.0, signal_gain / 20.0)) +
           noise;
}