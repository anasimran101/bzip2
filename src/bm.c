#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "bm.h"

/*
 * Reads input file and divides into blocks
 * @param filename: Input file path
 * @param block_size: Size of each block in bytes
 * @return: BlockManager structure containing all blocks
 */
BlockManager *divide_into_blocks(const char *filename, size_t block_size)  {
    
    FILE *fp = fopen(filename, "br");
    if(fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }
    BlockManager *bm = (BlockManager* )malloc(sizeof(BlockManager));
    if(bm == NULL) {
        fprintf(stderr, "Error allocating memory for BlockManager\n");
        fclose(fp);
        return NULL;
    }

    //calculate number of blocks and allocate memory for them
    bm->block_size = block_size;
    fseek(fp, 0, SEEK_END);
    bm->num_blocks = ftell(fp) / block_size + (ftell(fp) % block_size != 0); // ceil(file_size / block_size) 
    fseek(fp, 0, SEEK_SET);
    
    bm->blocks = (Block*)malloc(bm->num_blocks * sizeof(Block));
    if(bm->blocks == NULL) {
        fprintf(stderr, "Error allocating memory for blocks\n");
        free(bm);
        fclose(fp);
        return NULL;
    }

    size_t i = 0;
    while(1)
    {
        bm->blocks[i].data = (unsigned char*)malloc(block_size);
        fread(bm->blocks[i].data, 1, block_size, fp);
        bm->blocks[i].size = block_size;
        if(i == bm->num_blocks - 1) {
            bm->blocks[i].size = ftell(fp) % (block_size +1);
            break;
        }
        bm->blocks[i].original_size = bm->blocks[i].size;
        i++;
    }

    fclose(fp);
    return bm;
}

/*
 * Reassembles blocks back into original file
 * @param manager: BlockManager containing processed blocks
 * @param output_filename: Path for output file
 * @return: 0 on success, -1 on failure
 */
int reassemble_blocks(BlockManager *manager, const char *output_filename) {

    if(manager == NULL) return -1;
    FILE *fp = fopen(output_filename, "wb");
    if(fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", output_filename);
        return -1;
    }

    for (size_t i = 0; i < manager->num_blocks; i++)
        fwrite(manager->blocks[i].data, 1, manager->blocks[i].size, fp);
    
    return 0;
}

/*
 * Frees memory allocated for BlockManager
 * @param manager: Pointer to BlockManager to free
 */
void free_block_manager(BlockManager *manager) {
    if(manager == NULL) return;
    for (size_t i = 0; i < manager->num_blocks; i++)
    {
        free(manager->blocks[i].data);
    }
    free(manager->blocks);
    free(manager);
    manager = NULL;
}

