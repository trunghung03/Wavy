#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>

#include <SDL.h>
#include <SDL_image.h>
#include "image.h"

#define PATH_LENGTH 23
#define LAST_FRAME 6573

void setupSDL();
void setupFrame();
void drawLine(float prevX, float prevY, float x, float y);
inline float map(float v, float a, float b, float c, float d);
inline void setFileName(int i);
void cleanup();

// SDL
int window_width = 500;
int window_height = 500;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* img_texture;

char file_path[PATH_LENGTH] = { 0 };

void setupSDL() {
    /* SDL setup */
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_EVERYTHING);

    window = SDL_CreateWindow("Bad Apple",
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
    setFileName(1);
    img_texture = IMG_LoadTexture(renderer, file_path);
    SDL_QueryTexture(img_texture, NULL, NULL, &window_width, &window_height);
    window_height *= 2; // make space for drawing sine waves
    // Added some padding to prevent cutoff
    SDL_SetWindowSize(window, window_width, window_height + 5);
}

int main(int argc, char* argv[]) {
    setupSDL();

    setupFrame();

    SDL_Rect canvas = { 0, 0, window_width, window_height / 2 };

    // Public var
    // Load image frame
    int currentFrame = 1;
    setFileName(currentFrame);
    struct Frame* frame = load_frame(file_path);

    // Timing stuff
    int frameCount = 0;
    Uint32 time_start = 0;
    Uint32 time_between_frame = 33;

    // Waves variables
    int horizontal_div = 20;
    int horizontal_div_height = canvas.h / horizontal_div;
    float maxAmp = (float) horizontal_div_height / 2 - 2;
    float frequency = 50;

    int running = 1;
    while (running) {
        /* Input */
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) {
                free_frame(frame);
                cleanup();
            }
        }

        // Draw background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 1);
        SDL_RenderClear(renderer);

        // Draw image
        if (SDL_GetTicks() - time_start > time_between_frame && currentFrame < LAST_FRAME) {
            time_start = SDL_GetTicks();
            free_frame(frame);
            SDL_DestroyTexture(img_texture);

            currentFrame++;
            setFileName(currentFrame);
            frame = load_frame(file_path);
            img_texture = IMG_LoadTexture(renderer, file_path);
        }
        SDL_RenderCopy(renderer, img_texture, NULL, &canvas);

        // Draw waves
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 1);
        {
            //frequency = map(sinf((float)frameCount / 30), -1, 1, 40, 60);

            int horizontal;
            for (horizontal = 0; horizontal < horizontal_div; horizontal++) {
                float y = (float) horizontal * horizontal_div_height + (float) horizontal_div_height / 2;

                float prevX = -150;
                float prevY = (float) y;
                int x;
                for (x = 0; x < window_width; x++) {
                    float angle = map((float) x, 0, (float)window_width, 0, (float) M_PI * 2);
                    float phase = y * (float) M_PI / 4 + (float)frameCount / 2;
                    float sinValue = sinf(phase + angle * frequency);
                    //float sinValue = sinf(angle * frequency);

                    int grayIndex = get_pixel(frame, x, (int) y);

                    float amp = map((float) grayIndex, 0, 255, 0, maxAmp);

                    float subY = y + sinValue * amp + canvas.h;
                    drawLine(prevX, prevY, (float)x, subY);

                    prevX = (float)x;
                    prevY = subY;
                }
            }
            
        }

        SDL_RenderPresent(renderer);

        frameCount = (frameCount + 1) % 31415; // Prevent overflow, the 314 is about pi
    }

    return 1;
}

// convert normalized x, y coord to real coord
// Used drawLine instead of point because points can have gaps
void drawLine(float prevX, float prevY, float x, float y) {
    SDL_RenderDrawLineF(renderer, prevX, prevY, x, y);
}

inline float map(float v, float a, float b, float c, float d) {
    return (v - a) / (b - a) * (d - c) + c;
}

inline void setFileName(int i) {
    sprintf(file_path, "./asset/frame%04d.png", i);
}

void cleanup() {
    //free_frame(frame);
    SDL_DestroyTexture(img_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    exit(0);
}

