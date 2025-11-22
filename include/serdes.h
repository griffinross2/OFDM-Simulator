#pragma once

#include "constellation.h"

#include <vector>
#include <array>

// Take a bitstream and output the values for each subcarrier

class SerDes {

public:
	SerDes(unsigned int numSubcarriers, std::vector<bool> dataCarrierMask, unsigned int bitsPerSymbol);

	void serializeNextFrame(bool** inStream, std::vector<unsigned int>& outFrame);

private:
	unsigned int bitsPerSymbol;
	unsigned int numSubcarriers;
	std::vector<bool> dataCarrierMask;
};