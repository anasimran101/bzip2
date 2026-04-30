#include <stdio.h>
#include "config.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <config_file>\n", argv[0]);
        return 1;
    }
    char* filename = argv[1];
    config_t cfg;
    if (config_load(filename, &cfg) != 0) {
        fprintf(stderr, "Failed to load %s\n", filename);
        return 1;
    }
    config_print(&cfg);
    printf("Config loaded successfully.\n");
    return 0;
}
