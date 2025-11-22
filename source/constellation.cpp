#include "constellation.h"

#include <array>

const std::array<std::complex<float>, 2> constellationTableBPSK = {
	std::complex<float>(1.0f, 0.0f),
	std::complex<float>(-1.0f, 0.0f)
};

const std::array<std::complex<float>, 4> constellationTableQPSK = {
	std::complex<float>(1.0f, 0.0f),
	std::complex<float>(-1.0f, 0.0f),
	std::complex<float>(0.0f, 1.0f),
	std::complex<float>(0.0f, -1.0f)
};

void getConstellationPoints(Constellation constellation, std::vector<unsigned int>& inData, std::vector<std::complex<float>>& outSymbols) {
	outSymbols.clear();

	switch (constellation) {
	case BPSK:
		for (auto val : inData) {
			outSymbols.push_back(constellationTableBPSK[val]);
		}
		break;
	case QPSK:
		for (auto val : inData) {
			outSymbols.push_back(constellationTableQPSK[val]);
		}
		break;
	default:
		// Unsupported constellation
		break;
	}
}