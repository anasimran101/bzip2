#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "bwt.h"


typedef struct {
    int index;      //original position in string 
    int rank[2];    // current and next gap rank
} SuffixRank;

static int cmp_rank(const void *a, const void *b) {
    const SuffixRank *x = (const SuffixRank *)a;
    const SuffixRank *y = (const SuffixRank *)b;
    if (x->rank[0] != y->rank[0]) return x->rank[0] - y->rank[0];
    return x->rank[1] - y->rank[1];
}

int *build_suffix_array(unsigned char *text, int n) {
    SuffixRank *sr  = malloc(n * sizeof(SuffixRank));
    int        *sa  = malloc(n * sizeof(int));
    int        *tmp = malloc(n * sizeof(int));  /* rank lookup by position */

    /* Round 0: rank by first 2 characters */
    for (int i = 0; i < n; i++) {
        sr[i].index   = i;
        sr[i].rank[0] = text[i];
        sr[i].rank[1] = (i + 1 < n) ? text[i + 1] : -1;
    }

    qsort(sr, n, sizeof(SuffixRank), cmp_rank);

    for (int gap = 2; gap < n; gap *= 2) {

        /* assign integer ranks to sorted order */
        tmp[sr[0].index] = 0;
        for (int i = 1; i < n; i++) {
            tmp[sr[i].index] = tmp[sr[i-1].index];
            /* new rank only if pair differs from previous */
            if (sr[i].rank[0] != sr[i-1].rank[0] ||
                sr[i].rank[1] != sr[i-1].rank[1])
                tmp[sr[i].index]++;
        }

        /* all ranks unique → suffix array is complete */
        if (tmp[sr[n-1].index] == n - 1) break;

        /* build new rank pairs for next round */
        for (int i = 0; i < n; i++) {
            sr[i].rank[0] = tmp[sr[i].index];
            sr[i].rank[1] = (sr[i].index + gap < n) ? tmp[sr[i].index + gap] : -1; //-1 = past end, sorts first 
        }

        qsort(sr, n, sizeof(SuffixRank), cmp_rank);
    }

    for (int i = 0; i < n; i++) sa[i] = sr[i].index;

    free(sr);
    free(tmp);
    return sa;
}

void bwt_encode(unsigned char *input, size_t len, unsigned char *output, int *primary_index)
{
    int n  = (int)len;
    int *sa = build_suffix_array(input, n);

    for (int i = 0; i < n; i++) {
        if (sa[i] == 0) {
            *primary_index = i;
            output[i] = input[n - 1];       /* wrap: char before start */
        } else {
            output[i] = input[sa[i] - 1];   /* char just before suffix */
        }
    }

    free(sa);
}


void bwt_decode(unsigned char *input, size_t len,
                 int primary_index, unsigned char *output)
{
    size_t freq[256] = {0};
    size_t occ[256]  = {0};

    size_t *cumulative = malloc(256 * sizeof(size_t));
    size_t *next       = malloc(len * sizeof(size_t));

    /* 1. frequency of characters */
    for (size_t i = 0; i < len; i++) {
        freq[input[i]]++;
    }

    /* 2. cumulative frequency (F column start positions) */
    cumulative[0] = 0;
    for (size_t i = 1; i < 256; i++) {
        cumulative[i] = cumulative[i - 1] + freq[i - 1];
    }

    /* 3. build LF mapping in L-space consistently */
    for (size_t i = 0; i < len; i++) {
        unsigned char c = input[i];
        size_t f_pos = cumulative[c] + occ[c];

        /* F[f_pos] corresponds to L[i] */
        next[i] = f_pos;

        occ[c]++;
    }

    /* 4. invert permutation starting from primary_index */
    size_t idx = (size_t)primary_index;

    for (size_t i = len; i-- > 0;) {
        output[i] = input[idx];

        /* move in L-space using inverse LF */
        idx = next[idx];
    }

    free(next);
    free(cumulative);
}