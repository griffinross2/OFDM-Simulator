#include "fft.h"

#include <exception>
#include <print>

#define PI 3.14159265359f

void fftCreatePlan(FFTPlan& plan, unsigned int N) {
    plan.N = N;
	for (int k = 0; k < N / 2; k++) {
		plan.twiddles.push_back(std::exp(std::complex<float>(0, -2.0f * PI * k / N)));
	}
}

static void fftWork(FFTPlan& plan, std::vector<std::complex<float>>& input, std::vector<std::complex<float>>& output, unsigned int k_start, unsigned int N, unsigned int stride) {
    if (N == 1) {
        // Base case
        output[0] = input[k_start];
    }
    else {
        std::vector<std::complex<float>> even(N / 2);
        std::vector<std::complex<float>> odd(N / 2);
        fftWork(plan, input, even, k_start, N / 2, 2 * stride);
        fftWork(plan, input, odd, k_start + stride, N / 2, 2 * stride);

        for (int k = 0; k < N / 2; k++) {
            std::complex<float> even_k = even[k];
            std::complex<float> odd_k = odd[k] * plan.twiddles[k*(plan.N/N)];
            output[k] = even_k + odd_k;
            output[k + N / 2] = even_k - odd_k;
        }
    }
}

void fftExecute(FFTPlan& plan, std::vector<std::complex<float>>& input, std::vector<std::complex<float>>& output) {
    unsigned int N = input.size();
    while ((N & 0x1) == 0) {
        N >>= 1;
    }
    if (N != 1) {
        throw std::invalid_argument("Input size must be a power of 2");
    }

    N = input.size();
    fftWork(plan, input, output, 0, N, 1);
}

void ifftCreatePlan(FFTPlan& plan, unsigned int N) {
    plan.N = N;
	for (int k = 0; k < N / 2; k++) {
		plan.twiddles.push_back(std::exp(std::complex<float>(0, 2.0f * PI * k / N)));
	}
}

void ifftExecute(FFTPlan& plan, std::vector<std::complex<float>>& input, std::vector<std::complex<float>>& output) {
    unsigned int N = input.size();
    while ((N & 0x1) == 0) {
        N >>= 1;
    }
    if (N != 1) {
        throw std::invalid_argument("Input size must be a power of 2");
    }

    N = input.size();
    fftWork(plan, input, output, 0, N, 1);

    for (int k = 0; k < N; k++) {
        output[k] /= N;
    }
}

