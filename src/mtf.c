#include <stddef.h>
#include <stdint.h>
#include "mtf.h"
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

    for (int i = 0; i < 256; i++) {
        symbols[i] = (unsigned char)i;
    }
    for (size_t i = 0; i < len; i++) {

        unsigned char symbol = input[i];
        int index = 0;
        while (index < 256 && symbols[index] != symbol) {
            index++;
        }
        if (index == 256) {
            index = 0;
        }
        output[i] = (unsigned char)index;

        unsigned char temp = symbols[index];
        for (int j = index; j > 0; j--) {
            symbols[j] = symbols[j - 1];
        }
        symbols[0] = temp;
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
    for (int i = 0; i < 256; i++) {
        symbols[i] = (unsigned char)i;
    }
    for (size_t i = 0; i < len; i++) {

        unsigned char index = input[i];
        if (index >= 256) {
            index = 0;
        }
        unsigned char symbol = symbols[index];
        output[i] = symbol;
        for (int j = index; j > 0; j--) {
            symbols[j] = symbols[j - 1];
        }
        symbols[0] = symbol;
    }
}