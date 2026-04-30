#ifndef RLE_H
#define RLE_H
#include <stddef.h>

/*
 * Encodes data using Run-Length Encoding
 * @param input: Input byte array
 * @param len: Length of input array
 * @param output: Output buffer (must be pre-allocated)
 * @param out_len: Pointer to store output length
 */
void rle1_encode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len);

/*
 * Decodes RLE-1 encoded data
 * @param input: Encoded byte array
 * @param len: Length of encoded data
 * @param output: Output buffer for decoded data
 * @param out_len: Pointer to store decoded length
 */
void rle1_decode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len);

/*
 * Encodes MTF output using specialized RLE
 * @param input: MTF output array
 * @param len: Length of input
 * @param output: Output buffer
 * @param out_len: Pointer to store output length
 */
void rle2_encode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len);

/*
 * Decodes RLE-2 encoded data
 * @param input: RLE-2 encoded data
 * @param len: Length of encoded data
 * @param output: Output buffer for MTF data
 * @param out_len: Pointer to store output length
 */
void rle2_decode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len);

#endif /* RLE_H */