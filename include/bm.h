
#include <stddef.h>

/*
 * Structure to hold a single block of data
 */
typedef struct {
    unsigned char *data;   // Pointer to block data
    size_t size;           // Current size of block
    size_t original_size;  // Original size before compression
} Block;

/*
 * Structure to manage multiple blocks
 */
typedef struct {
    Block *blocks;         // Array of blocks
    int num_blocks;        // Number of blocks
    size_t block_size;     // Configurable block size
} BlockManager;

/*
 * Reads input file and divides into blocks
 * @param filename: Input file path
 * @param block_size: Size of each block in bytes
 * @return: BlockManager structure containing all blocks
 */
BlockManager *divide_into_blocks(const char *filename, size_t block_size);

/*
 * Reassembles blocks back into original file
 * @param manager: BlockManager containing processed blocks
 * @param output_filename: Path for output file
 * @return: 0 on success, -1 on failure
 */
int reassemble_blocks(BlockManager *manager, const char *output_filename);

/*
 * Frees memory allocated for BlockManager
 * @param manager: Pointer to BlockManager to free
 */
void free_block_manager(BlockManager *manager);

