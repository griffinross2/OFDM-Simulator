#include "convolutional_coder.h"
#include <stdexcept>
#include <string.h>
#include <cmath>
#include <stdio.h>
#include "log_file.h"

ConvCoder::ConvCoder(int k, int rate_denominator, uint64_t *poly, uint64_t poly_num)
{
    this->k = k;
    this->rate = rate_denominator;

    if (rate < 2)
    {
        throw std::invalid_argument("Rate must be greater than 1!");
    }

    if (poly_num != rate_denominator)
    {
        throw std::invalid_argument("Rate must be equal to number of polynomials!");
    }

    this->poly = (uint64_t *)malloc(poly_num * sizeof(uint64_t));

    if (this->poly == nullptr)
    {
        throw std::runtime_error("Failed to allocate!");
    }

    memcpy(this->poly, poly, poly_num * sizeof(uint64_t));
}

ConvCoder::~ConvCoder()
{
    if (this->poly)
    {
        free(this->poly);
    }
}

int ConvCoder::encode(uint8_t *data_in, uint8_t *data_out, int len_bits, int pad_bits)
{
    uint64_t reg = 0;
    uint64_t mask = ((int)pow(2.0, (double)this->k)) - 1;
    uint8_t output_byte = 0;
    int output_byte_count = 0;
    int output_byte_bits = 0;

    for (int i = 0; i < len_bits; i++)
    {
        // Insert new bit
        reg |= ((data_in[i / 8] >> (i % 8)) & 0x1);
        // Get result
        for (int r = 0; r < this->rate; r++)
        {
            output_byte |= ((__builtin_parity(reg & this->poly[r])) << output_byte_bits);
            output_byte_bits += 1;

            if (output_byte_bits >= 8)
            {
                // Write this out
                data_out[output_byte_count] = output_byte;
                output_byte_count++;
                output_byte = 0;
                output_byte_bits = 0;
            }
        }
        // Shift reg
        reg = (reg << 1) & mask;
    }

    // Tail bits
    for (int j = 0; j < pad_bits; j++)
    {
        // Dont insert a bit (insert 0)
        // Get result
        for (int r = 0; r < this->rate; r++)
        {
            output_byte |= ((__builtin_parity(reg & this->poly[r])) << output_byte_bits);
            output_byte_bits += 1;

            if (output_byte_bits >= 8)
            {
                // Write this out
                data_out[output_byte_count] = output_byte;
                output_byte_count++;
                output_byte = 0;
                output_byte_bits = 0;
            }
        }
        // Shift reg
        reg = (reg << 1) & mask;
    }

    // Write remaining unfinished byte
    if (output_byte_bits > 0)
    {
        // Write this out
        data_out[output_byte_count] = output_byte;
    }

    return 0;
}

int ConvCoder::decode(uint8_t *data_in, uint8_t *data_out, int block_len_bits, int pad_bits)
{
    // Allocate rows for each timestep in the history of the convolutional register
    unsigned int **metrics = (unsigned int **)malloc(sizeof(unsigned int *) * (block_len_bits + 1));
    if (metrics == nullptr)
    {
        throw std::runtime_error("Failed to allocate memory!");
    }

    for (int i = 0; i < block_len_bits + 1; i++)
    {
        // Allocate columns for each possible state
        metrics[i] = (unsigned int *)malloc(sizeof(unsigned int) * ((int)pow(2, this->k - 1)));
        // Initialize each with max value
        memset(metrics[i], 0xFF, sizeof(unsigned int) * ((int)pow(2, this->k - 1)));
    }

    // Initialize 0,0 state to 0
    metrics[0][0] = 0;

    // Keep track of input
    int input_byte = 0;
    int input_bit = 0;
    uint8_t curr_input = data_in[0];
    uint64_t curr_symbol = 0;

    // Mask for state
    uint64_t mask = ((int)pow(2, this->k - 1)) - 1;

    // Trace the paths
    for (int i = 0; i < block_len_bits; i++)
    {
        // Get the next encoded symbol
        for (int b = 0; b < this->rate; b++)
        {
            // Get next bit of the input datastream
            curr_symbol |= ((curr_input & 0x1) << this->rate);
            curr_symbol >>= 1;

            input_bit++;
            curr_input >>= 1;

            if (input_bit >= 8)
            {
                input_byte++;
                input_bit = 0;
                curr_input = data_in[input_byte];
            }
        }

        // For each next state, find the lowest metric needed to get there based on all previous states, inputs,
        // and the actual encoded symbol.
        for (int j = 0; j < (int)pow(2, this->k - 1); j++)
        {
            // Skip starting states with maximum metric
            if (metrics[i][j] == (unsigned int)-1)
            {
                continue;
            }

            // Input 0 case
            uint64_t temp = j << 1;
            uint64_t output = 0;
            // Calculate hypothetical output
            for (int r = 0; r < this->rate; r++)
            {
                output |= (__builtin_parity(temp & this->poly[r]) << r);
            }
            // Hamming distance from received symbol
            int dist = 0;
            uint64_t bits_xor = output ^ curr_symbol;
            for (int r = 0; r < this->rate; r++)
            {
                if ((bits_xor >> r) & 0x1)
                {
                    dist++;
                }
            }
            // Best metric?
            if (metrics[i + 1][temp & mask] > metrics[i][j] + dist)
            {
                metrics[i + 1][temp & mask] = metrics[i][j] + dist;
            }

            // Input 1 case
            temp |= 1;
            output = 0;
            // Calculate hypothetical output
            for (int r = 0; r < this->rate; r++)
            {
                output |= (__builtin_parity(temp & this->poly[r]) << r);
            }
            // Hamming distance from received symbol
            dist = 0;
            bits_xor = output ^ curr_symbol;
            for (int r = 0; r < this->rate; r++)
            {
                if ((bits_xor >> r) & 0x1)
                {
                    dist++;
                }
            }
            // Best metric?
            if (metrics[i + 1][temp & mask] > metrics[i][j] + dist)
            {
                metrics[i + 1][temp & mask] = metrics[i][j] + dist;
            }
        }
    }

    // Traceback through best path
    int best_state = 0;
    unsigned int best_metric = (unsigned int)-1;
    // Clear output
    memset(data_out, 0, (block_len_bits + 7) / 8);
    // Starting best
    for (int j = 0; j < (int)pow(2, this->k - 1); j++)
    {
        if (metrics[block_len_bits][j] < best_metric)
        {
            best_metric = metrics[block_len_bits][j];
            best_state = j;
        }
    }

    // // Print
    // for (int j = 0; j < (int)pow(2, this->k-1); j++) {
    //     for (int i = 0; i < block_len_bits+1; i++) {
    //         if (metrics[i][j] != 0) {
    //             printf("%11u ", metrics[i][j]);
    //         } else {
    //             printf("==== 0 ==== ");
    //         }
    //     }
    //     printf("\n");
    // }

    // Trace backward through best path
    for (int i = block_len_bits - 1; i >= 0; i--)
    {
        // Store bit, if necessary
        if (pad_bits)
        {
            pad_bits--;
        }
        else
        {
            data_out[i / 8] |= (best_state & 0x1) << (i % 8);
        }

        // 0 was just shifted out?
        uint64_t prev_state = (best_state >> 1);
        best_metric = metrics[i][prev_state];
        best_state = prev_state;

        // 1 was just shifted out?
        prev_state |= (1 << (this->k - 2));
        if (metrics[i][prev_state] < best_metric)
        {
            best_metric = metrics[i][prev_state];
            best_state = prev_state;
        }
    }

    // Free memory
    for (int i = 0; i < block_len_bits + 1; i++)
    {
        free(metrics[i]);
    }
    free(metrics);

    return 0;
}

int ConvCoder::interleave(uint8_t *data_in, uint8_t *data_out, int block_len_bits, int rows, int cols)
{
    uint8_t curr_output = 0;

    // Clear output buffer
    memset(data_out, 0, (block_len_bits + 7) / 8);

    // Construct the output, taking the input from the right place
    for (int i = 0; i < block_len_bits; i++)
    {
        int input_idx = (i / rows) + ((i % rows) * cols);
        uint8_t input = (data_in[input_idx / 8] >> (input_idx % 8)) & 0x1;
        data_out[i / 8] |= (input << (i % 8));
    }

    return 0;
}

int ConvCoder::deinterleave(uint8_t *data_in, uint8_t *data_out, int block_len_bits, int rows, int cols)
{
    uint8_t curr_output = 0;

    // Clear output buffer
    memset(data_out, 0, (block_len_bits + 7) / 8);

    // Construct the output, taking the input from the right place
    for (int i = 0; i < block_len_bits; i++)
    {
        int input_idx = (i / cols) + ((i % cols) * rows);
        uint8_t input = (data_in[input_idx / 8] >> (input_idx % 8)) & 0x1;
        data_out[i / 8] |= (input << (i % 8));
    }

    return 0;
}