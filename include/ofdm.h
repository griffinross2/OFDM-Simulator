#pragma once

#include <vector>
#include <complex>

void OFDMModulate(std::vector<std::complex<float>>& inFrame, std::vector<std::complex<float>>& outSamples, unsigned int numGuardSymbols);