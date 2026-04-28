#include <stddef.h>
#include "ans.h"
/*
 * Adaptive Number Coding (ANS) encode
 * @param input: Input byte array
 * @param len: Length of input
 * @param output: Output buffer for ANS encoded data
 * @param output_len: Pointer to store length of encoded data
 */
void ans_encode(unsigned char *input, size_t len, unsigned char *output, size_t *output_len) {
    // build freq and cummfreq

    const size_t R = 10;
    size_t L = 1 << R;

    size_t fs[256] = {0};
    size_t cs[256];
    unsigned char *sym = NULL;
    
    
    

    for (size_t i = 0; i < len; i++) 
        fs[input[i]]++;
    cs[0] = 0;
    for (size_t i = 1; i < 256; i++) 
        cs[i] = cs[i - 1] + fs[i-1];
    
    
    sym = (unsigned char*) malloc(L);
    
    // build symbol spread table

    for (size_t i = 0; i < 256; i++)
        for (size_t j = 0; j < fs[i]; j++)
            sym[((cs[i] + (j * L / fs[i])) % L)] = i;


    // encode TABLE
    struct et_entry* encode_table[256];
    for (size_t i = 0; i < 256; i++)
        encode_table[i] = (struct et_entry*) malloc(L * sizeof(struct et_entry));
    

    for (size_t i = 0; i < 256; i++)
    {
        for (size_t j = L; j < 2* L; j++)
        {
            encode_table[i][j].next_state = L;
            encode_table[i][j].bits = 0;
            while (encode_table[i][j].next_state >= j) {
                encode_table[i][j].bits++;
                encode_table[i][j].next_state <<= 1;
            }
        }
    }


    
    
    
}
/*
 * Adaptive Number Coding (ANS) decode
 * @param input: ANS encoded data
 * @param len: Length of encoded data
 * @param output: Output buffer for decoded data
 * @param output_len: Pointer to store length of decoded data
 */
void ans_decode(unsigned char *input, size_t len, unsigned char *output, size_t *output_len);