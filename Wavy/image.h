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

unsigned char get_pixel(struct Frame* frame, int x, int y) {
    if (x > frame->x || x < 0) {
        printf("out of bound: x = %d, bound = %d\n", x, frame->x);
        exit(1);
    }
    else if (y > frame->y || y < 0) {
        printf("out of bound: y = %d, bound = %d\n", y, frame->y);
        exit(1);
    }
    return frame->data[y * frame->x + x];
}