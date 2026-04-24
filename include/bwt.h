#include <stddef.h>


/*
 * Forward BWT transform
 * @param input: Input byte array
 * @param len: Length of input
 * @param output: Output buffer for BWT result
 * @param primary_index: Pointer to store primary index
 */
void bwt_encode(unsigned char *input, size_t len,
                unsigned char *output, int *primary_index);

/*
 * Inverse BWT transform
 * @param input: BWT encoded data
 * @param len: Length of encoded data
 * @param primary_index: Primary index from encoding
 * @param output: Output buffer for original data
 */
void bwt_decode(unsigned char *input, size_t len,
                int primary_index, unsigned char *output);


/*
* Builds suffix array for efficient BWT
* @param text: Input text
* @param n: Length of text
* @return: Suffix array
*/
int *build_suffix_array(unsigned char *text , int n);

