#ifndef _IMGDAT_H_
#define _IMGDAT_H_

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} __attribute__((packed)) ID_Pixel32;

#define COMPONENTS sizeof(ID_Pixel32)

// For some reason this doesn't want to compile
// static_assert(COMPONENTS == 4);

typedef struct {
    size_t off;
    size_t size;
    ID_Pixel32* pixels;
    uint8_t xor_key;
} ID_Processor;

extern uint8_t imgdat_xor_key;

ID_Processor idp_create(int w, int h, ID_Pixel32 pixels[w * h]);
void idp_encode(ID_Processor* p, void* data, size_t size);
void idp_decode(ID_Processor* p, void* buffer, size_t size);

void imgdat_encode(int w, int h, ID_Pixel32 pixels[w * h], void* data, uint32_t size);
void* imgdat_decode(int w, int h, ID_Pixel32 pixels[w * h], uint32_t* size);

void imgdat_encode_file(const char* input_filename, const char* template_filename, const char* output_filename);
void imgdat_decode_to_stream(const char* input_filename, FILE* stream);
void imgdat_decode_file(const char* input_filename, const char* output_filename);

#endif // _IMGDAT_H_
