#pragma once

#include "stdint.h"

class ConvCoder
{
public:
    ConvCoder(int k, int rate_denominator, uint64_t *poly, uint64_t poly_num);
    ~ConvCoder();
    int encode(uint8_t *data_in, uint8_t *data_out, int len_bits, int pad_bits);
    int decode(uint8_t *data_in, uint8_t *data_out, int block_len_bits, int pad_bits);
    int interleave(uint8_t *data_in, uint8_t *data_out, int block_len_bits, int rows, int cols);
    int deinterleave(uint8_t *data_in, uint8_t *data_out, int block_len_bits, int rows, int cols);

private:
    int k;
    int rate;
    uint64_t *poly;
};