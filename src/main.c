#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "types.h"

f64 calcFPS_SDL() {
    static f64 lastDelta;
    static u64 lastTime;
    u64 currentTime = SDL_GetTicks64();
    f64 delta = (f64) (currentTime - lastTime) / 1000.0;
    if (delta == 0) { delta = lastDelta; }
    f64 fps = 1.0 / delta;
    lastDelta = delta;
    lastTime = currentTime;

    return fps;
}

c8* f32ToString(f32 number) {
    usize len = SDL_snprintf(NULL, 0, "%.5f", number);
    c8* result = SDL_malloc(len + 1);
    SDL_snprintf(result, len + 1, "%.5f", number);

    return result;
}

void drawF32(SDL_Renderer* renderer, s32 x, s32 y, TTF_Font* font, SDL_Color color, f32 number) {
    SDL_Surface* surface;
    SDL_Texture* texture;
    SDL_Rect rect;
    c8* text = f32ToString(number);
    surface = TTF_RenderText_Solid(font, text, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_free(text);
}

void drawSprites(SDL_Renderer* renderer, SDL_Texture* texture) {
    // spritesheet
    const u16 spriteSheetSizeX = 128;
    const u16 spriteSheetSizeY = 128;
    const u16 spriteSheetSpritesNumber = 64;
    // source
    const u16 spriteSourceSizeX = 16;
    const u16 spriteSourceSizeY = 16;
    // target
    const u16 spriteTargetSizeX = 64;
    const u16 spriteTargetSizeY = 64;
    // sprite count
    const u16 maxSprites = 2048;
    const u16 maxSpriteSheets = maxSprites / spriteSheetSpritesNumber;

    for (u16 i = 0; i < maxSpriteSheets; i += 1) {
        u16 column = 0;
        u16 line = 0;
        for (u16 j = 0; j <= spriteSheetSpritesNumber; j += 1) {
            SDL_Rect textureRect = {
                .w = 16,
                .h = 16,
                .x = column,
                .y = line,
            };
            SDL_Rect rect = {
                .w = spriteTargetSizeX,
                .h = spriteTargetSizeY,
                .x = rand() % (1920 - spriteTargetSizeX),
                .y = rand() % (1080 - spriteTargetSizeY),
            };

            column += spriteSourceSizeX;
            if (column >= spriteSheetSizeX) {
                column = 0;
                line += spriteSourceSizeY;
            }

            SDL_RenderCopy(renderer, texture, &textureRect, &rect);
        }
    }
}

void processEvents(b8* running, SDL_Window* window) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                *running = false;
            } break;

            case SDL_KEYDOWN: {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    u32 windowFlags;
                    if (SDL_GetWindowFlags(window) & SDL_WINDOW_FULLSCREEN) {
                        windowFlags = 0;
                    } else {
                        windowFlags = SDL_WINDOW_FULLSCREEN;   
                    }
                    SDL_SetWindowFullscreen(window, windowFlags);
                } 
            } break;

            default: {
            } break;
        }
    }
}

s32 main(s32 argc, char** argv) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
        exit(-1);
    }

    if (TTF_Init() < 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL_ttf: %s", SDL_GetError());
        exit(-1);
    }

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL_image: %s", SDL_GetError());
        exit(-1);
    }

    SDL_Window* window = SDL_CreateWindow("sdl benchmark", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    if (window == NULL) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create a window: %s", SDL_GetError());
        exit(-1);
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create a renderer: %s", SDL_GetError());
        exit(-1);
    }

    TTF_Font* font = TTF_OpenFont("OpenSans-Semibold.ttf", 20);
    if (renderer == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load a font: %s", SDL_GetError());
    }

    SDL_Surface* image = IMG_Load("sprite.png");
    if (image == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't load a image: %s", SDL_GetError());
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, image);
    SDL_FreeSurface(image);

    b8 running = true;

    u64 frequency = SDL_GetPerformanceFrequency();    
    while (running) {
        // calc delta && fps
        static u64 lastTime;
        u64 currentTime = SDL_GetPerformanceCounter();
        f32 delta = (f32) (currentTime - lastTime) / frequency;
        f32 fps = 1.0f / delta;
        lastTime = currentTime;
 
        processEvents(&running, window);

        SDL_RenderClear(renderer); {
            drawSprites(renderer, texture);
            drawF32(renderer, 0, 0, font, (SDL_Color){0, 255, 0, 255}, fps); // fps
            drawF32(renderer, 0, 20, font, (SDL_Color){0, 255, 0, 255}, delta); // frame time 
        } SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    exit(0);
}
