#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "bwt.h"
#include "mtf.h"
#include "rle.h"
#include "bm.h"
#include "config.h"

/*Minimal test harness*/
static int tests_run    = 0;
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(label, cond)                                         \
    do {                                                            \
        tests_run++;                                                \
        if (cond) {                                                 \
            tests_passed++;                                         \
            printf("  [PASS] %s\n", label);                        \
        } else {                                                    \
            tests_failed++;                                         \
            printf("  [FAIL] %s  (line %d)\n", label, __LINE__);   \
        }                                                           \
    } while (0)

#define ASSERT_MEM(label, a, b, n) \
    ASSERT(label, memcmp(a, b, n) == 0)

static void print_bytes(const char *tag, unsigned char *buf, size_t len) {
    printf("    %s: ", tag);
    for (size_t i = 0; i < len; i++) printf("%02X ", buf[i]);
    printf("\n");
}


/* BWT */
static void test_bwt(void) {
    printf("\n[BWT]\n");

    /* Known BWT transform of "banana" = "nnbaaa", primary_index = 3 */
    unsigned char input[]    = "banana";
    size_t        len        = 6;
    unsigned char expected[] = { 'n', 'n', 'b', 'a', 'a', 'a' };

    unsigned char *bwt_out = (unsigned char*)malloc(len);
    int primary_index;
    bwt_encode(input, len, bwt_out, &primary_index);

    print_bytes("bwt_out ", bwt_out, len);
    ASSERT_MEM("BWT encode: 'banana' -> 'nnbaaa'", bwt_out, expected, len);
    ASSERT("BWT encode: primary_index == 3", primary_index == 3);

    /* Round-trip */
    unsigned char *bwt_dec = (unsigned char*)malloc(len);
    bwt_decode(bwt_out, len, primary_index, bwt_dec);
    print_bytes("bwt_dec ", bwt_dec, len);
    ASSERT_MEM("BWT round-trip matches input", bwt_dec, input, len);

    free(bwt_out);
    free(bwt_dec);

    /* Edge case: single byte */
    unsigned char single_in[]  = { 0x42 };
    unsigned char *single_bwt  = (unsigned char*)malloc(1);
    int           single_idx;
    bwt_encode(single_in, 1, single_bwt, &single_idx);
    unsigned char *single_dec  = (unsigned char*)malloc(1);
    bwt_decode(single_bwt, 1, single_idx, single_dec);
    ASSERT("BWT single byte round-trip", single_dec[0] == single_in[0]);
    free(single_bwt);
    free(single_dec);

    /* Longer string with repeated pattern */
    unsigned char rep_in[] = "aaaaabbbbb";
    size_t        rep_len  = 10;
    unsigned char *rep_bwt = (unsigned char*)malloc(rep_len);
    int            rep_idx;
    bwt_encode(rep_in, rep_len, rep_bwt, &rep_idx);
    unsigned char *rep_dec = (unsigned char*)malloc(rep_len);
    bwt_decode(rep_bwt, rep_len, rep_idx, rep_dec);
    print_bytes("rep bwt ", rep_bwt, rep_len);
    ASSERT_MEM("BWT repeated pattern round-trip", rep_dec, rep_in, rep_len);
    free(rep_bwt);
    free(rep_dec);
}


/* 2. MTF */
static void test_mtf(void) {
    printf("\n[MTF]\n");

    /*
     * Alphabet starts as 0x00..0xFF.
     * Input: 'a'(0x61) 'a' 'b'(0x62) 'a'
     * Expected MTF indices:
     *   'a' = 0x61 -> index 97  (first occurrence, move to front)
     *   'a'        -> index 0   (now at front)
     *   'b' = 0x62 -> index 98  (original pos, one step back because 'a' moved)
     *                           actual = 98 since 'a'(97) moved to 0, 'b' was at 98 -> now 98
     *   'a'        -> index 1   ('b' is now at front after 'a' moved? No:
     *                            list: [a, 0,1..60,62..96,b,98..] after first 'a'
     *                            after second 'a': list unchanged (already front)
     *                            'b' original slot: list is [a,0,1..97 skipping a, b...]
     *                            We just test round-trip here for clarity)
     */
    unsigned char input[]  = { 'a', 'a', 'b', 'a' };
    size_t        len      = 4;

    unsigned char *mtf_out = (unsigned char*)malloc(len);
    mtf_encode(input, len, mtf_out);
    print_bytes("mtf_out", mtf_out, len);

    /* First 'a' must NOT be 0 (it wasn't at front yet) */
    ASSERT("MTF: first 'a' index > 0", mtf_out[0] > 0);
    /* Second 'a' must be 0 (moved to front) */
    ASSERT("MTF: second 'a' index == 0", mtf_out[1] == 0);
    /* 'a' again after 'b' must be 1 ('b' is now at front) */
    ASSERT("MTF: third 'a' index == 1", mtf_out[3] == 1);

    /* Round-trip */
    unsigned char *mtf_dec = (unsigned char*)malloc(len);
    mtf_decode(mtf_out, len, mtf_dec);
    print_bytes("mtf_dec", mtf_dec, len);
    ASSERT_MEM("MTF round-trip", mtf_dec, input, len);
    free(mtf_out);
    free(mtf_dec);

    /* Edge case: all-zero input (already at front every time) */
    unsigned char zeros[8] = { 0 };
    unsigned char *z_out   = (unsigned char*)malloc(8);
    mtf_encode(zeros, 8, z_out);
    /* Every output index must be 0 */
    int all_zero = 1;
    for (int i = 0; i < 8; i++) if (z_out[i] != 0) all_zero = 0;
    ASSERT("MTF: all-zero input -> all-zero indices", all_zero);
    free(z_out);

    /* Longer round-trip */
    unsigned char long_in[] = "abracadabra";
    size_t long_len = 11;
    unsigned char *long_mtf = (unsigned char*)malloc(long_len);
    unsigned char *long_dec = (unsigned char*)malloc(long_len);
    mtf_encode(long_in, long_len, long_mtf);
    mtf_decode(long_mtf, long_len, long_dec);
    ASSERT_MEM("MTF 'abracadabra' round-trip", long_dec, long_in, long_len);
    free(long_mtf);
    free(long_dec);
}


/* 3. RLE2*/
static void test_rle2(void) {
    printf("\n[RLE2]\n");

    /* RLE2 operates on MTF output: runs of 0s are encoded specially.
       Input with a long run of zeros should compress. */
    unsigned char zeros[16];
    memset(zeros, 0, 16);

    unsigned char *rle_out = (unsigned char*)malloc(64);
    size_t rle_len;
    rle2_encode(zeros, 16, rle_out, &rle_len);
    print_bytes("rle2_out(zeros)", rle_out, rle_len);
    ASSERT("RLE2: 16 zeros compress to fewer bytes", rle_len < 16);

    unsigned char *rle_dec = (unsigned char*)malloc(64);
    size_t rle_dec_len;
    rle2_decode(rle_out, rle_len, rle_dec, &rle_dec_len);
    ASSERT("RLE2: decoded length == 16", rle_dec_len == 16);
    ASSERT_MEM("RLE2: zeros round-trip", rle_dec, zeros, 16);
    free(rle_out);
    free(rle_dec);

    /* Mixed input — non-zero values should pass through */
    unsigned char mixed[] = { 0, 0, 0, 5, 0, 0, 7, 0 };
    size_t mixed_len = 8;
    unsigned char *m_out = (unsigned char*)malloc(32);
    size_t m_out_len;
    rle2_encode(mixed, mixed_len, m_out, &m_out_len);
    print_bytes("rle2_out(mixed)", m_out, m_out_len);

    unsigned char *m_dec = (unsigned char*)malloc(32);
    size_t m_dec_len;
    rle2_decode(m_out, m_out_len, m_dec, &m_dec_len);
    print_bytes("rle2_dec(mixed)", m_dec, m_dec_len);
    ASSERT("RLE2: mixed decoded length == 8", m_dec_len == 8);
    ASSERT_MEM("RLE2: mixed round-trip", m_dec, mixed, 8);
    free(m_out);
    free(m_dec);

    /* No zeros — output should be same length or close */
    unsigned char no_zeros[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    unsigned char *nz_out = (unsigned char*)malloc(32);
    size_t nz_len;
    rle2_encode(no_zeros, 8, nz_out, &nz_len);
    unsigned char *nz_dec = (unsigned char*)malloc(32);
    size_t nz_dec_len;
    rle2_decode(nz_out, nz_len, nz_dec, &nz_dec_len);
    ASSERT("RLE2: no-zero input round-trip length", nz_dec_len == 8);
    ASSERT_MEM("RLE2: no-zero round-trip", nz_dec, no_zeros, 8);
    free(nz_out);
    free(nz_dec);
}


/* 4. RLE1  (pre-BWT pass) */
static void test_rle1(void) {
    printf("\n[RLE1]\n");

    /* Four or more identical bytes get run-length encoded */
    unsigned char run[] = { 'a','a','a','a','a', 'b' };  /* 5 a's + b */
    size_t run_len = 6;
    unsigned char *out = (unsigned char*)malloc(32);
    size_t out_len;
    rle1_encode(run, run_len, out, &out_len);
    print_bytes("rle1_out", out, out_len);
    ASSERT("RLE1: run-of-5 compresses", out_len < run_len);

    unsigned char *dec = (unsigned char*)malloc(32);
    size_t dec_len;
    rle1_decode(out, out_len, dec, &dec_len);
    print_bytes("rle1_dec", dec, dec_len);
    ASSERT("RLE1: decoded length == 6", dec_len == 6);
    ASSERT_MEM("RLE1: round-trip", dec, run, 6);
    free(out);
    free(dec);

    /* No long run — should pass through unchanged */
    unsigned char no_run[] = { 'a','b','c','d' };
    unsigned char *nr_out = (unsigned char*)malloc(16);
    size_t nr_len;
    rle1_encode(no_run, 4, nr_out, &nr_len);
    unsigned char *nr_dec = (unsigned char*)malloc(16);
    size_t nr_dec_len;
    rle1_decode(nr_out, nr_len, nr_dec, &nr_dec_len);
    ASSERT("RLE1: no-run round-trip length", nr_dec_len == 4);
    ASSERT_MEM("RLE1: no-run round-trip", nr_dec, no_run, 4);
    free(nr_out);
    free(nr_dec);

    /* Exactly 4 identical bytes (boundary) */
    unsigned char boundary[] = { 'x','x','x','x' };
    unsigned char *b_out = (unsigned char*)malloc(16);
    size_t b_len;
    rle1_encode(boundary, 4, b_out, &b_len);
    unsigned char *b_dec = (unsigned char*)malloc(16);
    size_t b_dec_len;
    rle1_decode(b_out, b_len, b_dec, &b_dec_len);
    ASSERT("RLE1: boundary-4 round-trip length", b_dec_len == 4);
    ASSERT_MEM("RLE1: boundary-4 round-trip", b_dec, boundary, 4);
    free(b_out);
    free(b_dec);
}


/* 5. ANS
static void test_ans(void) {
    printf("\n[ANS]\n");

    //Skewed distribution compresses well
    unsigned char skewed[32];
    memset(skewed, 'a', 28);
    skewed[28] = 'b'; skewed[29] = 'c'; skewed[30] = 'd'; skewed[31] = 'e';
    size_t skewed_len = 32;

    unsigned char *ans_out = (unsigned char*)malloc(skewed_len * 2);
    size_t ans_len;
    ans_encode(skewed, skewed_len, ans_out, &ans_len);
    print_bytes("ans_out(skewed)", ans_out, ans_len);
    ASSERT("ANS: skewed input compresses", ans_len < skewed_len);

    unsigned char *ans_dec = (unsigned char*)malloc(skewed_len * 2);
    size_t ans_dec_len;
    ans_decode(ans_out, ans_len, ans_dec, &ans_dec_len);
    ASSERT("ANS: decoded length == 32", ans_dec_len == skewed_len);
    ASSERT_MEM("ANS: skewed round-trip", ans_dec, skewed, skewed_len);
    free(ans_out);
    free(ans_dec);

    // Uniform distribution — should round-trip even if it doesn't compress 
    unsigned char uniform[16];
    for (int i = 0; i < 16; i++) uniform[i] = (unsigned char)i;
    unsigned char *u_out = (unsigned char*)malloc(64);
    size_t u_len;
    ans_encode(uniform, 16, u_out, &u_len);
    unsigned char *u_dec = (unsigned char*)malloc(64);
    size_t u_dec_len;
    ans_decode(u_out, u_len, u_dec, &u_dec_len);
    ASSERT("ANS: uniform decoded length == 16", u_dec_len == 16);
    ASSERT_MEM("ANS: uniform round-trip", u_dec, uniform, 16);
    free(u_out);
    free(u_dec);

    // Single symbol
    unsigned char single[1] = { 0xAB };
    unsigned char *s_out = (unsigned char*)malloc(16);
    size_t s_len;
    ans_encode(single, 1, s_out, &s_len);
    unsigned char *s_dec = (unsigned char*)malloc(16);
    size_t s_dec_len;
    ans_decode(s_out, s_len, s_dec, &s_dec_len);
    ASSERT("ANS: single byte round-trip", s_dec_len == 1 && s_dec[0] == 0xAB);
    free(s_out);
    free(s_dec);
} */

/* 6. Full pipeline  (BWT -> MTF -> RLE2  and back) */
static void test_full_pipeline(void) {
    printf("\n[FULL PIPELINE]\n");

    struct {
        const char *label;
        unsigned char *data;
        size_t len;
    } cases[] = {
        { "short ascii",  (unsigned char*)"hello world",         11 },
        { "repeated",     (unsigned char*)"aaaaaabbbbbbcccccc",  18 },
        { "single byte",  (unsigned char*)"\x42",                 1 }
    };

    for (int c = 0; c < 3; c++) {

        unsigned char *orig = cases[c].data;
        size_t len = cases[c].len;

        /* ── ENCODE ── */
        unsigned char *bwt_out = malloc(len);
        int primary_index;
        bwt_encode(orig, len, bwt_out, &primary_index);

        unsigned char *mtf_out = malloc(len);
        mtf_encode(bwt_out, len, mtf_out);

        unsigned char *rle_out = malloc(len * 2 + 16);
        size_t rle_len;
        rle2_encode(mtf_out, len, rle_out, &rle_len);

        /* ── DECODE ── */
        unsigned char *rle_dec = malloc(len * 2 + 16);
        size_t rle_dec_len;
        rle2_decode(rle_out, rle_len, rle_dec, &rle_dec_len);

        /* FIX 1: MTF decode must use ORIGINAL length (NOT rle_dec_len) */
        unsigned char *mtf_dec = malloc(len);
        mtf_decode(rle_dec, rle_dec_len, mtf_dec);

        unsigned char *final = malloc(len);

        /* FIX 2: BWT decode must use ORIGINAL length */
        bwt_decode(mtf_dec, len, primary_index, final);

        char label[128];
        snprintf(label, sizeof(label),
                 "Pipeline round-trip: %s (len=%zu)", cases[c].label, len);

        ASSERT_MEM(label, final, orig, len);

        free(bwt_out);
        free(mtf_out);
        free(rle_out);
        free(rle_dec);
        free(mtf_dec);
        free(final);
    }
}
/*  7. BlockManager */
static void test_block_manager(void) {
    printf("\n[BLOCK MANAGER]\n");

    /* Write a temp file, divide into blocks, reassemble, compare */
    const char *tmp_in  = "/tmp/bm_test_input.bin";
    const char *tmp_out = "/tmp/bm_test_output.bin";

    /* 1000 bytes of known data */
    unsigned char src[1000];
    for (int i = 0; i < 1000; i++) src[i] = (unsigned char)(i & 0xFF);

    FILE *fp = fopen(tmp_in, "wb");
    fwrite(src, 1, 1000, fp);
    fclose(fp);

    /* Block size 300 -> ceil(1000/300) = 4 blocks */
    BlockManager *bm = divide_into_blocks(tmp_in, 300);
    ASSERT("BM: divide_into_blocks not NULL", bm != NULL);
    ASSERT("BM: 4 blocks for 1000 bytes / 300", bm->num_blocks == 4);
    ASSERT("BM: last block size == 100", bm->blocks[3].size == 100);

    int rc = reassemble_blocks(bm, tmp_out);
    ASSERT("BM: reassemble_blocks returns 0", rc == 0);

    /* Read back and compare */
    FILE *fp2 = fopen(tmp_out, "rb");
    unsigned char dst[1000];
    size_t read_back = fread(dst, 1, 1000, fp2);
    fclose(fp2);
    ASSERT("BM: reassembled file size == 1000", read_back == 1000);
    ASSERT_MEM("BM: reassembled content matches", dst, src, 1000);

    free_block_manager(bm);

    /* Edge: file size exactly one block */
    const char *tmp_exact = "/tmp/bm_exact.bin";
    unsigned char exact[300];
    memset(exact, 0xAB, 300);
    fp = fopen(tmp_exact, "wb");
    fwrite(exact, 1, 300, fp);
    fclose(fp);
    BlockManager *bm2 = divide_into_blocks(tmp_exact, 300);
    ASSERT("BM: exact-fit -> 1 block", bm2->num_blocks == 1);
    ASSERT("BM: exact-fit block size == 300", bm2->blocks[0].size == 300);
    free_block_manager(bm2);
}


/* Entry point */
int main(void) {
    printf("=== bzip2 pipeline unit tests ===\n");

    config_t cfg;
    if (config_load("config.ini", &cfg) != 0) {
        fprintf(stderr, "Failed to load config.ini\n");
        return 1;
    }
    config_print(&cfg);
    printf("Config loaded successfully.\n");
    test_bwt();
    test_mtf();
    test_rle1();
    test_rle2();
    //test_ans();
    test_full_pipeline();
    test_block_manager();

    printf("\n=== Results: %d/%d passed", tests_passed, tests_run);
    if (tests_failed)
        printf(", %d FAILED", tests_failed);
    printf(" ===\n");

    return tests_failed ? 1 : 0;
}