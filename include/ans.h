#ifndef ANS_H
#define ANS_H
#include <stddef.h>


struct et_entry {
    size_t next_state;
    size_t bits;
};


/*
 * Adaptive Number Coding (ANS) encode
 * @param input: Input byte array
 * @param len: Length of input
 * @param output: Output buffer for ANS encoded data
 * @param output_len: Pointer to store length of encoded data
 */
void ans_encode(unsigned char *input, size_t len, unsigned char *output, size_t *output_len);

/*
 * Adaptive Number Coding (ANS) decode
 * @param input: ANS encoded data
 * @param len: Length of encoded data
 * @param output: Output buffer for decoded data
 * @param output_len: Pointer to store length of decoded data
 */
void ans_decode(unsigned char *input, size_t len, unsigned char *output, size_t *output_len);
#endif /* ANS_H */