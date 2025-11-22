#include "serdes.h"
#include "constellation.h"
#include "ofdm.h"

#include <print>
#include <random>

#include <matplot/matplot.h>

#define INPUT_LEN 256
#define NUM_SUBCARRIERS 16

int main(int argc, char** argv) {

	SerDes serdes(NUM_SUBCARRIERS, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 }, QPSK);

	// Input bitstream
	std::array<bool, INPUT_LEN> inStream;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::bernoulli_distribution d;
	
	for (int b = 0; b < INPUT_LEN; b++) {
		bool bit = d(gen);
		inStream[b] = bit;
	}

	bool* inStreamPtr = inStream.data();
	std::vector<unsigned int> outFrame;
	std::vector<std::complex<float>> symbolFrame;
	std::vector<std::complex<float>> allSymbolsFFT;
	std::vector<float> allSymbolsFFTReal;
	std::vector<float> allSymbolsFFTImag;

	for (unsigned int i = 0; i < inStream.size(); i += NUM_SUBCARRIERS) {
		serdes.serializeNextFrame(&inStreamPtr, outFrame);
		getConstellationPoints(QPSK, outFrame, symbolFrame);
		OFDMModulate(symbolFrame, allSymbolsFFT, 4);
	}

	std::print("\n");
	for (auto& sym : allSymbolsFFT) {
		std::print("{}+({}j)\n", sym.real(), sym.imag());
		allSymbolsFFTReal.push_back(sym.real());
		allSymbolsFFTImag.push_back(sym.imag());
	}

	matplot::figure();
	matplot::plot(allSymbolsFFTReal);

	matplot::figure();
	matplot::plot(allSymbolsFFTImag);
	matplot::show();

	return 0;
}