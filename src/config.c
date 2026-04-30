
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "ini.h"





static int parse_bool(const char* value) {
    return strcmp(value, "true") == 0 || strcmp(value, "1") == 0;
}

static int parse_int_comment(const char* value) {
    char buf[64];
    strncpy(buf, value, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';
    char* hash = strchr(buf, '#');
    if (hash) *hash = '\0';
    return atoi(buf);
}

static void parse_str_comment(const char* value, char* dest, size_t dest_size) {
    strncpy(dest, value, dest_size);
    dest[dest_size - 1] = '\0';
    char* hash = strchr(dest, '#');
    if (hash) {
        *hash = '\0';
        char* end = hash - 1;
        while (end >= dest && *end == ' ') *end-- = '\0';
    }
}

/*
 * Handler function for ini parsing, called for each name=value pair parsed.
 * Returns 1 on success, 0 on error - unknown section name or invalid value
 */
static int config_handler(void* user, const char* section, const char* name, const char* value)
{
    config_t* cfg = (config_t*)user;

    #define EQUAL(s, n) (strcmp(section, s) == 0 && strcmp(name, n) == 0)

    /* [General] */
    if      (EQUAL("General", "block_size"))      cfg->block_size      = parse_int_comment(value);
    else if (EQUAL("General", "rle1_enabled"))    cfg->rle1_enabled    = parse_bool(value);
    else if (EQUAL("General", "bwt_type"))        parse_str_comment(value, cfg->bwt_type, BWT_TYPE_SIZE);
    else if (EQUAL("General", "mtf_enabled"))     cfg->mtf_enabled     = parse_bool(value);
    else if (EQUAL("General", "rle2_enabled"))    cfg->rle2_enabled    = parse_bool(value);
    else if (EQUAL("General", "huffman_enabled")) cfg->huffman_enabled = parse_bool(value);

    /* [Performance] */
    else if (EQUAL("Performance", "benchmark_mode")) cfg->benchmark_mode = parse_bool(value);
    else if (EQUAL("Performance", "output_metrics")) cfg->output_metrics = parse_bool(value);

    /* [Paths] */
    else if (EQUAL("Paths", "input_directory"))
        parse_str_comment(value, cfg->input_directory, INPUT_DIR_SIZE);

    else return 0;   /* unknown key -> signal error */

    return 1;        /* all good, keep parsing */

    #undef EQUAL
}

/*
 * Loads config from ini file, applying defaults for missing values.
 * @param filepath: Path to ini file
 * @param cfg: Pointer to config struct to populate
 * @return: 0 on success, -1 on failure (file not found or parse error)
 */
int config_load(const char* filepath, config_t* cfg) {

    /* sensible defaults before parsing */
    cfg->block_size      = 500000;
    cfg->rle1_enabled    = 1;
    strncpy(cfg->bwt_type, "matrix", BWT_TYPE_SIZE);
    cfg->mtf_enabled     = 1;
    cfg->rle2_enabled    = 1;
    cfg->huffman_enabled = 1;
    cfg->benchmark_mode  = 0;
    cfg->output_metrics  = 1;
    strncpy(cfg->input_directory, "./", INPUT_DIR_SIZE);

    int result = ini_parse(filepath, config_handler, cfg);

    if (result == -1) {
        fprintf(stderr, "[config] ERROR: file not found: %s\n", filepath);
        return -1;
    }
    if (result > 0) {
        fprintf(stderr, "[config] ERROR: parse error at line %d in %s\n", result, filepath);
        return -1;
    }

    /* validate block_size range */
    if (cfg->block_size < 100000 || cfg->block_size > 900000) {
        fprintf(stderr, "[config] WARNING: block_size %d out of range [100000,900000], clamping\n",
                cfg->block_size);
        if (cfg->block_size < 100000) cfg->block_size = 100000;
        if (cfg->block_size > 900000) cfg->block_size = 900000;
    }

    return 0;
}


void config_print(const config_t* cfg) {
    printf("=== Config ===\n");
    printf("[General]\n");
    printf("  block_size      = %d\n",  cfg->block_size);
    printf("  rle1_enabled    = %s\n",  cfg->rle1_enabled    ? "true" : "false");
    printf("  bwt_type        = %s\n",  cfg->bwt_type);
    printf("  mtf_enabled     = %s\n",  cfg->mtf_enabled     ? "true" : "false");
    printf("  rle2_enabled    = %s\n",  cfg->rle2_enabled    ? "true" : "false");
    printf("  huffman_enabled = %s\n",  cfg->huffman_enabled ? "true" : "false");
    printf("[Performance]\n");
    printf("  benchmark_mode  = %s\n",  cfg->benchmark_mode  ? "true" : "false");
    printf("  output_metrics  = %s\n",  cfg->output_metrics  ? "true" : "false");
    printf("[Paths]\n");
    printf("  input_directory = %s\n",  cfg->input_directory);
}