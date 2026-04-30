#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stddef.h>


/* explicit sizes matching the struct field declarations */
#define BWT_TYPE_SIZE      32
#define INPUT_DIR_SIZE    256

typedef struct {
    /* [General] */
    int         block_size;
    int         rle1_enabled;     /* 0 or 1 */
    char        bwt_type[BWT_TYPE_SIZE]; /* "matrix" or "suffix_array" */
    int         mtf_enabled;
    int         rle2_enabled;
    int         huffman_enabled;

    /* [Performance] */
    int         benchmark_mode;
    int         output_metrics;

    /* [Paths] */
    char        input_directory[INPUT_DIR_SIZE];
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
#endif /* CONFIG_H */