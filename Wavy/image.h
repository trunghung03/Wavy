#pragma once
#define STB_IMAGE_IMPLEMENTATION   
#include "stb_image.h"
#include <stdlib.h>
#define FRAME_PATH "./asset/frame.png"

struct Frame {
    int x, y, n;
    unsigned char* data;
};

struct Frame* load_frame(char* path) {
    int x, y, n;
    unsigned char* data = stbi_load(path, &x, &y, &n, 1);

    struct Frame* result = (struct Frame *) malloc(sizeof(struct Frame));

    if (result == NULL) exit(1);

    result->x = x;
    result->y = y;
    result->data = data;

    return result;
}

void free_frame(struct Frame* frame) {
    stbi_image_free(frame->data);
    free(frame);
}

unsigned char getPixel(struct Frame* frame, int x, int y) {
    return frame->data[y * frame->x + x];
}