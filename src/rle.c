#include <stddef.h>
#include <stdint.h>
#include "rle.h"


void rle1_encode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len)
{
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len) {
        unsigned char symbol = input[in_pos];
        size_t run = 1;

        while ((in_pos + run < len) &&
               (input[in_pos + run] == symbol) &&
               (run < 255)) {
            run++;
        }

        output[out_pos++] = symbol;
        output[out_pos++] = (unsigned char)run;

        in_pos += run;
    }

    *out_len = out_pos;
}

void rle1_decode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len)
{
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos + 1 < len) {
        unsigned char symbol = input[in_pos++];
        unsigned char run = input[in_pos++];

        for (size_t i = 0; i < run; i++) {
            output[out_pos++] = symbol;
        }
    }

    *out_len = out_pos;
}

static void encode_zero_run(size_t run, unsigned char *output, size_t *out_pos)
{
    run--;

    do {
        if (run & 1) {
            output[(*out_pos)++] = 1;
        } else {
            output[(*out_pos)++] = 0;
        }

        run = (run >> 1);

    } while (run != 0);
}

void rle2_encode(unsigned char *input, size_t len, unsigned char *output, size_t *out_len)
{
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len) {

        if (input[in_pos] == 0) {

            size_t run = 0;

            while ((in_pos < len) &&
                   (input[in_pos] == 0)) {
                run++;
                in_pos++;
            }

            encode_zero_run(run, output, &out_pos);
        }
        else {
            output[out_pos++] = input[in_pos] + 1;
            in_pos++;
        }
    }

    *out_len = out_pos;
}

void rle2_decode(unsigned char *input, size_t len,
                 unsigned char *output, size_t *out_len)
{
    size_t in_pos = 0;
    size_t out_pos = 0;

    while (in_pos < len) {

        if (input[in_pos] == 0 ||
            input[in_pos] == 1) {

            size_t run = 0;
            size_t power = 1;

            while (in_pos < len &&
                   (input[in_pos] == 0 ||
                    input[in_pos] == 1)) {

                if (input[in_pos] == 1) {
                    run += power;
                }

                power <<= 1;
                in_pos++;
            }

            run += (power >> 1);

            for (size_t i = 0; i < run; i++) {
                output[out_pos++] = 0;
            }
        }
        else {
            output[out_pos++] = input[in_pos] - 1;
            in_pos++;
        }
    }

    *out_len = out_pos;
}