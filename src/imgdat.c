#include "imgdat.h"

#include <assert.h>
#include <endian.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "stb_image.h"
#include "stb_image_write.h"

uint8_t imgdat_xor_key = 0;

static void encode_byte(ID_Pixel32* pixel, uint8_t b) {
    pixel->r = ((pixel->r >> 2) << 2) | ((b >> 0) & 0b11);
    pixel->g = ((pixel->g >> 2) << 2) | ((b >> 2) & 0b11);
    pixel->b = ((pixel->b >> 2) << 2) | ((b >> 4) & 0b11);
    pixel->a = ((pixel->a >> 2) << 2) | ((b >> 6) & 0b11);
}

static uint8_t decode_byte(const ID_Pixel32* pixel) {
    return
        ((pixel->a & 0b11) << 6) |
        ((pixel->b & 0b11) << 4) |
        ((pixel->g & 0b11) << 2) |
        ((pixel->r & 0b11) << 0);
}

static uint8_t* read_file(const char* filename, size_t* size) {
    FILE* file = fopen(filename, "rb");
    if(!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);

    uint8_t* buffer = malloc(*size);
    if(!buffer)
        return NULL;

    fread(buffer, *size, 1, file);
    fclose(file);
    return buffer;
}

ID_Processor idp_create(int w, int h, ID_Pixel32 pixels[w * h]) {
    return (ID_Processor) {
        .off = 0,
        .size = w * h,
        .pixels = pixels,
        .xor_key = imgdat_xor_key
    };
}

void idp_encode(ID_Processor* p, void* data, size_t size) {
    assert(p->off + size <= p->size);
    for(size_t i = 0; i < size; i++) {
        uint8_t b = ((uint8_t*)data)[i] ^ p->xor_key;
        encode_byte(&p->pixels[p->off + i], b);
    }

    p->off += size;
}

void idp_decode(ID_Processor* p, void* buffer, size_t size) {
    assert(p->size - p->off > size);
    for(size_t i = 0; i < size; i++) {
        uint8_t b = decode_byte(&p->pixels[p->off + i]);
        ((uint8_t*)buffer)[i] = b ^ p->xor_key;
    }

    p->off += size;
}

void imgdat_encode(int w, int h, ID_Pixel32 pixels[w * h], void* data, uint32_t size) {
    ID_Processor p = idp_create(w, h, pixels);
    idp_encode(&p, &size, sizeof(size));
    idp_encode(&p, data, size);
}

void* imgdat_decode(int w, int h, ID_Pixel32 pixels[w * h], uint32_t* size) {
    ID_Processor p = idp_create(w, h, pixels);

    idp_decode(&p, size, sizeof(*size));
    void* buffer = malloc(*size);
    assert(buffer);

    idp_decode(&p, buffer, *size);
    return buffer;
}

void imgdat_encode_file(const char* input_filename, const char* template_filename, const char* output_filename) {
    size_t input_size;
    uint8_t* input = read_file(input_filename, &input_size);
    assert(input);

    int w, h;
    ID_Pixel32* pixels = (ID_Pixel32*)stbi_load(template_filename, &w, &h, NULL, COMPONENTS);
    assert(pixels);

    imgdat_encode(w, h, pixels, input, input_size);
    stbi_write_png(output_filename, w, h, COMPONENTS, pixels, w * COMPONENTS);
    stbi_image_free(pixels);
    free(input);
}

void imgdat_decode_to_stream(const char* input_filename, FILE* stream) {
    int w, h;
    ID_Pixel32* pixels = (ID_Pixel32*)stbi_load(input_filename, &w, &h, NULL, COMPONENTS);
    assert(pixels);

    uint32_t size;
    void* data = imgdat_decode(w, h, pixels, &size);

    fwrite(data, size, 1, stream);
    fflush(stream);
    free(data);
}

void imgdat_decode_file(const char* input_filename, const char* output_filename) {
    FILE* output = fopen(output_filename, "wb");
    assert(output);
    imgdat_decode_to_stream(input_filename, output);
    fclose(output);
}
