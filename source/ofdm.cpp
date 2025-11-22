#include "ofdm.h"

#include "fft.h"

void OFDMModulate(std::vector<std::complex<float>>& inFrame, std::vector<std::complex<float>>& outSamples, unsigned int numGuardSymbols) {
	FFTPlan plan;
	unsigned int N = inFrame.size();

	std::vector<std::complex<float>> symbolWindow(N);
	ifftCreatePlan(plan, N);
	ifftExecute(plan, inFrame, symbolWindow);

	for (int i = 0; i < numGuardSymbols; i++) {
		outSamples.push_back(symbolWindow[N - numGuardSymbols + i]);
	}

	for (int i = 0; i < N; i++) {
		outSamples.push_back(symbolWindow[i]);
	}
}