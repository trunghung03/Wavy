#include <stdio.h>
#include <stdint.h>

#include <SDL.h>
#include <SDL_image.h>
#include "image.h"

#define FRAME_PATH "./asset/frame.png"

void setupSDL();
void setupFrame();
inline void drawLine(float prevX, float prevY, float x, float y);
inline float map(float v, float a, float b, float c, float d);
void cleanup();

// SDL
int window_width = 500;
int window_height = 500;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* img_texture;

struct Frame* frame;


void setupSDL() {
    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Demo",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    if (window == NULL) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        exit(-1);
    }

    int render_flags = 0;
    render_flags |= SDL_RENDERER_ACCELERATED;
    render_flags |= SDL_RENDERER_PRESENTVSYNC;

    renderer = SDL_CreateRenderer(window, -1, render_flags);

    if (renderer == NULL) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        exit(-1);
    }

    /* scale the renderer output for High-DPI displays */
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(renderer, &render_w, &render_h);
        SDL_GetWindowSize(window, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(renderer, scale_x, scale_y);
    }
}

// image setup
void setupFrame() {
    // Set up display frame on SDL
    img_texture = IMG_LoadTexture(renderer, FRAME_PATH);
    SDL_QueryTexture(img_texture, NULL, NULL, &window_width, &window_height);
    window_height *= 2; // make space for drawing sine waves
    // Added some padding to prevent cutoff
    SDL_SetWindowSize(window, window_width, window_height + 5);

    // read from png image with stb_image
    frame = load_frame(FRAME_PATH);
}

int main(int argc, char* argv[]) {
    setupSDL();

    setupFrame();

    SDL_Rect canvas = { 0, 0, window_width, window_height / 2 };

    int frame = 31410;
    int running = 1;
    while (running) {
        /* Input */
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) cleanup();
        }

        // Draw background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
        SDL_RenderClear(renderer);

        // Draw image
        SDL_RenderCopy(renderer, img_texture, NULL, &canvas);

        // Draw wavy lines
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
        
        {
            float prevX = -150;
            float prevY = (float) canvas.h / 2;
            int i;
            for (i = 0; i < window_width; i++) {
                float angle = map((float) i, 0, (float) window_width, 0, (float) M_PI * 2);
                float frequency = 30;
                float phase = (float) frame / 50;
                float sinValue = sinf(phase + angle * frequency);
                //float sinValue = sinf(angle * frequency);

                float amp = map((float) sinf(angle * (frequency / 7)), -1, 1, 0, (float) canvas.h / 4);


                float y = (float) canvas.h / 2 + sinValue * amp + canvas.h;
                drawLine(prevX, prevY, (float) i, y);

                prevX = (float) i;
                prevY = y;
            }
            
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);

        SDL_RenderPresent(renderer);

        frame = (frame + 1) % 31415; // Prevent overflow, the 314 is about pi
    }

    return 1;
}

// convert normalized x, y coord to real coord
// Used drawLine instead of point because points can have gaps
inline void drawLine(float prevX, float prevY, float x, float y) {
    SDL_RenderDrawLineF(renderer, prevX, prevY, x, y);
}

inline float map(float v, float a, float b, float c, float d) {
    return (v - a) / (b - a) * (c + (d - c));
}

void cleanup() {
    free_frame(frame);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}

