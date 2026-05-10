#ifndef BZIP2_H
#define BZIP2_H

#include <stddef.h>
#include "bwt.h"
#include "mtf.h"
#include "bm.h"
#include "rle.h"
/**
 * Encode data using Bzip2 algorithm
 * @param bm: BlockManager instance containing input and output buffers
 */
void encode_bzip2(BlockManager *bm);

/**
 * Decode data using Bzip2 algorithm
 * @param bm: BlockManager instance containing input and output buffers
 */
void decode_bzip2(BlockManager *bm);
#endif /* BZIP2_H */