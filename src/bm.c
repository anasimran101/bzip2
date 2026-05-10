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
BlockManager *divide_into_blocks(const char *filename, size_t block_size) {

    if (filename == NULL || block_size == 0) return NULL;

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return NULL;
    }

    BlockManager *bm = (BlockManager*)malloc(sizeof(BlockManager));
    if (bm == NULL) {
        fclose(fp);
        return NULL;
    }

    bm->block_size = block_size;

    fseek(fp, 0, SEEK_END);
    long file_size_l = ftell(fp);

    if (file_size_l < 0) {
        fclose(fp);
        free(bm);
        return NULL;
    }

    size_t file_size = (size_t)file_size_l;
    fseek(fp, 0, SEEK_SET);

    if (file_size == 0) {
        bm->num_blocks = 0;
        bm->blocks = NULL;
        fclose(fp);
        return bm;
    }

    bm->num_blocks = file_size / block_size +
                     ((file_size % block_size) != 0);

    bm->blocks = (Block*)malloc(bm->num_blocks * sizeof(Block));
    if (bm->blocks == NULL) {
        free(bm);
        fclose(fp);
        return NULL;
    }

    size_t i = 0;
    while (i < bm->num_blocks) {

        bm->blocks[i].data = (unsigned char*)malloc(block_size);
        if (bm->blocks[i].data == NULL) {
            fclose(fp);
            return NULL; 
        }

        size_t read_bytes = fread(bm->blocks[i].data, 1, block_size, fp);

        bm->blocks[i].size = read_bytes;
        bm->blocks[i].original_size = read_bytes;

        if (i == bm->num_blocks - 1) {
            break;
        }

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
    fclose(fp);
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


