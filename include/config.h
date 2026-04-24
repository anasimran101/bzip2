#include <stdint.h>
#include <stddef.h>

typedef enum
{
    BWT_MATRIX,
    BWT_SUFFIX_ARRAY
} bwt_type_t;

typedef struct
{
    /* General */
    size_t block_size;
    int rle1_enabled;
    bwt_type_t bwt_type;
    int mtf_enabled;
    int rle2_enabled;
    int huffman_enabled;

    /* Performance */
    int benchmark_mode;
    int output_metrics;

    /* Paths */
    uint8_t input_directory[256];

} config_t;

/*
* Loads config from ini file, applying defaults for missing values
* @param filepath: Path to ini file
* @param cfg: Pointer to config struct to populate
* @return: 0 on success, -1 on failure (file not found or parse error)
*/
int config_load(const char* filepath, config_t* cfg);
/*
* Prints the current config values
* @param cfg: Pointer to config struct to print
*/
void config_print(const config_t* cfg);
