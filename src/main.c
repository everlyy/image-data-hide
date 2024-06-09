#include "imgdat.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define ARRAY_LENGTH(x) (sizeof(x) / (sizeof(*x)))
#define FLAGS_COUNT 255

const char* parse_args(
    int argc, char** argv,
    const char* flags[FLAGS_COUNT],
    size_t positional_cap, const char* positional[positional_cap])
{
    if(argc < 1)
        return NULL;

    const char* program = argv[0];
    size_t pos_i = 0;

    for(int i = 1; i < argc; i++) {
        const char* arg = argv[i];
        size_t len = strlen(arg);

        if(len < 1)
            continue;

        if(len == 2 && arg[0] == '-') {
            flags[(uint8_t)arg[1]] = argv[++i];
        } else {
            positional[pos_i++] = arg;
        }
    }

    return program;
}

size_t count_positional(const char* positional[]) {
    size_t len = 0;
    while(*positional++)
        len++;
    return len;
}

int main(int argc, char** argv) {
    const char* flags[FLAGS_COUNT] = {0};
    const char* positional[100] = {0};
    const char* program = parse_args(
        argc, argv, flags,
        ARRAY_LENGTH(positional), positional
    );

    if(!*positional || (strcmp(*positional, "encode") && strcmp(*positional, "decode"))) {
        printf("Usage: %s <encode|decode>\n", program);
        return 1;
    }

    if(flags['x'])
        imgdat_xor_key = atoi(flags['x']);

    if(!strcmp(*positional, "encode")) {
        if(count_positional(positional) != 3) {
            printf("Usage: %s encode <input> <template> [-o output.png]\n", program);
            return 1;
        }

        const char* input_filename = positional[1];
        const char* template_filename = positional[2];

        const char* output_filename = "encoded.png";
        if(flags['o'])
            output_filename = flags['o'];

        imgdat_encode_file(input_filename, template_filename, output_filename);
    } else {
        if(count_positional(positional) != 2) {
            printf("Usage: %s decode <input> [-o output]\n", program);
            return 1;
        }

        const char* input_filename = positional[1];

        FILE* output = stdout;
        if(flags['o']) {
            output = fopen(flags['o'], "wb");
            assert(output);
        }

        imgdat_decode_to_stream(input_filename, output);
    }

    return 0;
}
