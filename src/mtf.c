#include <stddef.h>

/*
 * Forward MTF transform
 * @param input: Input byte array
 * @param len: Length of input
 * @param output: Output buffer for MTF indices
 */
void mtf_encode(unsigned char *input, size_t len,
                unsigned char *output)
{
    unsigned char symbols[256];

    // TODO: Initialize the symbols array after analyzing the input data to optimize for common symbols
    for (int i = 0; i < 256; i++) {
        symbols[i] = (unsigned char)i;
    }

    for (size_t i = 0; i < len; i++) {
        unsigned char symbol = input[i];
        int index = 0;

        // Find the index of the byte in the symbols array
        while (symbols[index] != symbol) {
            index++;
        }

        output[i] = (unsigned char)index;

        // Move the symbol to the front
        unsigned char temp = symbols[index];
        for (int j = index; j > 0; j--) {
            symbols[j] = symbols[j - 1];
        }
        symbols[0] = temp;

        while( i < len && symbols[input[++i]] == 0) {}
    }
}

/*
 * Inverse MTF transform
 * @param input: MTF encoded indices
 * @param len: Length of input
 * @param output: Output buffer for decoded data
 */
void mtf_decode(unsigned char *input, size_t len,
                unsigned char *output)
{
    unsigned char symbols[256];

    // Initialize the symbols array
    for (int i = 0; i < 256; i++) {
        symbols[i] = (unsigned char)i;
    }

    for (size_t i = 0; i < len;) {
        unsigned char index = input[i];
        unsigned char symbol = symbols[index];
        output[i] = symbol;

        // Move the symbol to the front
        for (int j = index; j > 0; j--) {
            symbols[j] = symbols[j - 1];
        }
        symbols[0] = symbol;

        ++i;
        while( i < len && symbols[input[i]] == 0) { output[i++] = symbol; }
        
    }
}

