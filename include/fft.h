#pragma once

#include <vector>
#include <complex>

typedef struct {
	std::vector<std::complex<float>> twiddles;
	unsigned int N;
} FFTPlan;

void fftCreatePlan(FFTPlan& plan, unsigned int N);
void fftExecute(FFTPlan& plan, std::vector<std::complex<float>>& input, std::vector<std::complex<float>>& output);

void ifftCreatePlan(FFTPlan& plan, unsigned int N);
void ifftExecute(FFTPlan& plan, std::vector<std::complex<float>>& input, std::vector<std::complex<float>>& output);