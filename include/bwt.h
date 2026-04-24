
#include <stddef.h>

/*
 * Structure for rotation in BWT
 */
typedef struct {
    char *rotation; // Rotation string
    int index;      // Original index
} Rotation;

/*
 * Compares two rotations for sorting
 * @param a: First rotation
 * @param b: Second rotation
 * @return: Comparison result (-1, 0, 1)
 */
int compare_rotations(const void *a, const void *b);

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

