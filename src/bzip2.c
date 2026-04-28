#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include "bzip2.h"
#include "bm.h"
#include "bwt.h"
#include "mtf.h"
#include "rle.h"
#include "ans.h"

#define NUM_THREADS 8
#define BLOCK_MAGIC 0x425A6832  // "BZh2"

#pragma pack(1)
typedef struct {
    uint32_t magic;
    uint32_t original_size;
    int32_t  primary_index;
} block_header_t;
#pragma pack()

/* Thread argument: one block index to process */
typedef struct {
    BlockManager *bm;
    int block_idx;
} thread_arg_t;


/*
 * Encodes a single block in-place inside the BlockManager
 * Writes header + compressed payload back into block->data
 */
static void encode_block(BlockManager *bm, int idx) {
    Block *blk = &bm->blocks[idx];
    size_t len = blk->size;

    /* Step 1: BWT */
    unsigned char *bwt_out = (unsigned char*)malloc(len);
    int primary_index;
    bwt_encode(blk->data, len, bwt_out, &primary_index);

    /* Step 2: MTF */
    unsigned char *mtf_out = (unsigned char*)malloc(len);
    mtf_encode(bwt_out, len, mtf_out);
    free(bwt_out);

    /* Step 3: RLE2 */
    unsigned char *rle_out = (unsigned char*)malloc(len * 2);
    size_t rle_len;
    rle2_encode(mtf_out, len, rle_out, &rle_len);
    free(mtf_out);

    /* Step 4: ANS */
    unsigned char *ans_out = (unsigned char*)malloc(rle_len * 2);
    size_t ans_len;
    ans_encode(rle_out, rle_len, ans_out, &ans_len);
    free(rle_out);

    /* Write header + payload back into block */
    size_t total = sizeof(block_header_t) + ans_len;
    unsigned char *final = (unsigned char*)malloc(total);

    block_header_t hdr;
    hdr.magic         = BLOCK_MAGIC;
    hdr.original_size = (uint32_t)len;
    hdr.primary_index = primary_index;
    memcpy(final, &hdr, sizeof(block_header_t));
    memcpy(final + sizeof(block_header_t), ans_out, ans_len);
    free(ans_out);

    free(blk->data);
    blk->data          = final;
    blk->original_size = len;
    blk->size          = total;
}


/*
 * Decodes a single block in-place inside the BlockManager
 */
static void decode_block(BlockManager *bm, int idx) {
    Block *blk = &bm->blocks[idx];

    /* Read and validate header */
    block_header_t hdr;
    memcpy(&hdr, blk->data, sizeof(block_header_t));
    if (hdr.magic != BLOCK_MAGIC) {
        fprintf(stderr, "Block %d: bad magic 0x%08X\n", idx, hdr.magic);
        return;
    }

    unsigned char *payload     = blk->data + sizeof(block_header_t);
    size_t         payload_len = blk->size  - sizeof(block_header_t);

    /* Step 1: ANS decode */
    unsigned char *ans_out = (unsigned char*)malloc(hdr.original_size * 2);
    size_t ans_len;
    ans_decode(payload, payload_len, ans_out, &ans_len);

    /* Step 2: RLE2 decode */
    unsigned char *rle_out = (unsigned char*)malloc(hdr.original_size * 2);
    size_t rle_len;
    rle2_decode(ans_out, ans_len, rle_out, &rle_len);
    free(ans_out);

    /* Step 3: MTF decode */
    unsigned char *mtf_out = (unsigned char*)malloc(rle_len);
    mtf_decode(rle_out, rle_len, mtf_out);
    free(rle_out);

    /* Step 4: BWT decode — primary_index comes from the header */
    unsigned char *final = (unsigned char*)malloc(hdr.original_size);
    bwt_decode(mtf_out, rle_len, hdr.primary_index, final);
    free(mtf_out);

    free(blk->data);
    blk->data          = final;
    blk->size          = hdr.original_size;
    blk->original_size = hdr.original_size;
}


/* Thread workers */
static void *encode_thread(void *arg) {
    thread_arg_t *t = (thread_arg_t*)arg;
    encode_block(t->bm, t->block_idx);
    return NULL;
}

static void *decode_thread(void *arg) {
    thread_arg_t *t = (thread_arg_t*)arg;
    decode_block(t->bm, t->block_idx);
    return NULL;
}


/*
 * Encode all blocks using up to NUM_THREADS threads
 */
void encode_bzip2(BlockManager *bm) {
    pthread_t    threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];
    int i = 0;

    while (i < bm->num_blocks) {
        int batch = 0;

        /* launch up to NUM_THREADS at a time */
        while (batch < NUM_THREADS && i < bm->num_blocks) {
            args[batch].bm        = bm;
            args[batch].block_idx = i;
            pthread_create(&threads[batch], NULL, encode_thread, &args[batch]);
            batch++;
            i++;
        }

        /* wait for this batch */
        for (int j = 0; j < batch; j++)
            pthread_join(threads[j], NULL);
    }
}


/*
 * Decode all blocks using up to NUM_THREADS threads
 */
void decode_bzip2(BlockManager *bm) {
    pthread_t    threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];
    int i = 0;

    while (i < bm->num_blocks) {
        int batch = 0;

        while (batch < NUM_THREADS && i < bm->num_blocks) {
            args[batch].bm        = bm;
            args[batch].block_idx = i;
            pthread_create(&threads[batch], NULL, decode_thread, &args[batch]);
            batch++;
            i++;
        }

        for (int j = 0; j < batch; j++)
            pthread_join(threads[j], NULL);
    }
}