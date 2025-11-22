#include "serdes.h"

SerDes::SerDes(unsigned int numSubcarriers, std::vector<bool> dataCarrierMask, unsigned int bitsPerSymbol)
	: numSubcarriers(numSubcarriers), dataCarrierMask(dataCarrierMask), bitsPerSymbol(bitsPerSymbol) {
}

void SerDes::serializeNextFrame(bool** inStream, std::vector<unsigned int>& outFrame) {
	int inIndex = 0;
	outFrame.clear();

	for (int sc = 0; sc < numSubcarriers; sc++) {
		if (!dataCarrierMask[sc]) {
			outFrame.push_back(0);
			continue;
		}

		// Read bitsPerSymbol bits from inStream
		unsigned int symbolValue = 0;
		for (int b = 0; b < bitsPerSymbol; b++) {
			symbolValue <<= 1;
			if ((*inStream)[inIndex]) {
				symbolValue |= 1;
				(*inStream)++;
			}
			inIndex++;
		}

		outFrame.push_back(symbolValue);
	}
}