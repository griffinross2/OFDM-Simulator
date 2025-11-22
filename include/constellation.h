#pragma once

#include <complex>
#include <vector>

typedef enum {
	BPSK = 1,
	QPSK = 2,
	/*QAM16 = 4,
	QAM64 = 6,*/
} Constellation;

void getConstellationPoints(Constellation constellation, std::vector<unsigned int>& inData, std::vector<std::complex<float>>& outSymbols);