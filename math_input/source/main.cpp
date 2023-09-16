/*Copyright 2014 Francisco Alvaro

 This file is part of SESHAT.

    SESHAT is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SESHAT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SESHAT.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <seshat/seshat.hpp>
#include <SDL.h>

struct FILEDeleter {
    void operator()(FILE* fd)
    {
        fclose(fd);
    }
};
using FILEPtr = std::unique_ptr<FILE, FILEDeleter>;

struct WindowDeleter {
    void operator()(SDL_Window* ptr)
    {
        SDL_DestroyWindow(ptr);
    }
};
using WindowPtr = std::unique_ptr<SDL_Window, WindowDeleter>;

struct RendererDeleter {
    void operator()(SDL_Renderer* ptr)
    {
        SDL_DestroyRenderer(ptr);
    }
};
using RendererPtr = std::unique_ptr<SDL_Renderer, RendererDeleter>;

static seshat::sample loadSCGInk(const char* path)
{
    seshat::sample out;

    FILEPtr fd_holder(fopen(path, "r"));
    auto fd = fd_holder.get();
    if (!fd) {
        fprintf(stderr, "Error loading SCGInk file '%s'\n", path);
        return out;
    }

    char line[256];
    fgets(line, 256, fd);
    if (strcmp(line, "SCG_INK\n")) {
        fprintf(stderr, "Error: input file format is not SCG_INK\n");
        return out;
    }

    int nstrokes, npoints;
    fscanf(fd, "%d", &nstrokes);
    out.strokes.resize(nstrokes);
    for (auto& out_stroke : out.strokes) {
        fscanf(fd, "%d", &npoints);
        out_stroke.points.resize(npoints);
        for (auto& out_point : out_stroke.points) {
            fscanf(fd, "%f %f", &out_point.x, &out_point.y);
        }
    }

    return out;
}

#define SCREEN_WIDTH    800
#define SCREEN_HEIGHT   600

static void surfaceDrawSquare(SDL_Surface* surf, unsigned x, unsigned y, unsigned size)
{
    SDL_LockSurface(surf);

    SDL_Rect rect;
    rect.x = x - size / 2;
    rect.y = y - size / 2;
    rect.w = size;
    rect.h = size;
    SDL_FillRect(surf, &rect, SDL_MapRGBA(surf->format, 0, 0, 0, 255));

    SDL_UnlockSurface(surf);
}
static void surfaceFill(SDL_Surface* surf, uint32_t fillwith)
{
    SDL_LockSurface(surf);

    SDL_FillRect(surf, nullptr, fillwith);

    SDL_UnlockSurface(surf);
}
static void surfaceRedraw(SDL_Surface* surf, const seshat::sample& s)
{
    surfaceFill(surf, 0xffffffff);
    for(const auto& strk : s.strokes)
    {
        for(const auto& pt : strk.points)
        {
            surfaceDrawSquare(surf, pt.x, pt.y, 5);
        }
    }
}

static void workSDL()
{
    // Create window
    WindowPtr window_ptr{SDL_CreateWindow("seshat math input panel",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN)};
    const auto window = window_ptr.get();
    if(!window)
    {
        std::cout << "Window could not be created!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Create renderer
    RendererPtr renderer_ptr{SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC)};
    const auto renderer = renderer_ptr.get();
    if(!renderer)
    {
        std::cout << "Renderer could not be created!" << std::endl
                    << "SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }
    
    // Declare rect of square
    SDL_Rect squareRect;

    // Square dimensions: Half of the min(SCREEN_WIDTH, SCREEN_HEIGHT)
    squareRect.w = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;
    squareRect.h = std::min(SCREEN_WIDTH, SCREEN_HEIGHT) / 2;

    // Square position: In the middle of the screen
    squareRect.x = SCREEN_WIDTH / 2 - squareRect.w / 2;
    squareRect.y = SCREEN_HEIGHT / 2 - squareRect.h / 2;


    // Event loop exit flag
    bool quit = false;
    auto time_of_last_up = std::chrono::system_clock::now();
    bool drawing = false;
    bool handled_last_draw = true;
    seshat::sample s;
    seshat::math_expression recog;

    auto surf = SDL_CreateRGBSurface(0, squareRect.w, squareRect.h, 32,
                                        0x00FF0000,
                                        0x0000FF00,
                                        0x000000FF,
                                        0xFF000000);
    auto sdlTexture = SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_ARGB8888,
                               SDL_TEXTUREACCESS_STREAMING,
                               squareRect.w, squareRect.h);

    surfaceFill(surf, 0xffffffff);
    // Event loop
    while(!quit)
    {
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            switch(e.type) {
            case SDL_MOUSEBUTTONDOWN: {
                const auto& button = e.button;
                if(button.x >= squareRect.x && button.x < (squareRect.x + squareRect.w)
                && button.y >= squareRect.y && button.y < (squareRect.y + squareRect.h))
                {
                    if(button.button == SDL_BUTTON_LEFT && !drawing) // ACTION: BEGIN STROKE
                    {
                        drawing = true;
                        handled_last_draw = true;
                        s.strokes.emplace_back();
                        const auto& pt = s.strokes.back().points.emplace_back(button.x - squareRect.x, button.y - squareRect.y);
                        surfaceDrawSquare(surf, pt.x, pt.y, 5);
                    }
                    else if(button.button == SDL_BUTTON_RIGHT && !drawing) // ACTION: REMOVE LAST STROKE
                    {
                        if(!s.strokes.empty())
                        {
                            s.total_points -= s.strokes.back().points.size();
                            s.strokes.pop_back();
                            time_of_last_up = std::chrono::system_clock::now();
                            handled_last_draw = !s.strokes.empty();
                            surfaceRedraw(surf, s);
                        }
                    }
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                const auto& motion = e.motion;
                if(drawing)
                {
                    if(motion.x >= squareRect.x && motion.x < (squareRect.x + squareRect.w)
                    && motion.y >= squareRect.y && motion.y < (squareRect.y + squareRect.h)) // ACTION: ADD NEW POINT TO LAST STROKE
                    {
                        s.total_points += 1;
                        const auto& pt = s.strokes.back().points.emplace_back(motion.x - squareRect.x, motion.y - squareRect.y);
                        surfaceDrawSquare(surf, pt.x, pt.y, 5);
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                const auto& button = e.button;
                if(button.button == SDL_BUTTON_LEFT && drawing) // ACTION: END STROKE
                {
                    time_of_last_up = std::chrono::system_clock::now();
                    drawing = false;
                    handled_last_draw = false;
                }
                break;
            }
            case SDL_KEYDOWN: {
                const auto& key = e.key;
                if(!drawing && key.keysym.sym == SDLK_ESCAPE) // ACTION: REMOVE ALL STROKES
                {
                    s.total_points = 0;
                    s.strokes.clear();
                    surfaceFill(surf, 0xffffffff);
                    handled_last_draw = true;
                }
                break;
            }
            case SDL_QUIT:
                quit = true;
                break;
            }
        }

        const auto time_since_last_release = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_of_last_up);
        if(!drawing && !handled_last_draw && time_since_last_release.count() >= 1000 && !s.strokes.empty())
        {
            handled_last_draw = true;

            printf("Parsing %zd strokes with %zd total points\n", s.strokes.size(), s.total_points);
            auto hyps = recog.parse_sample(s);
            printf("Found %zd hypothesis\n", hyps.size());
        }
        // Initialize renderer color red for the background
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        // Clear screen
        SDL_RenderClear(renderer);

        // Set renderer color white to draw the square
        SDL_UpdateTexture(sdlTexture, nullptr, surf->pixels, surf->pitch);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderCopy(renderer, sdlTexture, nullptr, &squareRect);

        // Update screen
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(sdlTexture);
}

int main(int argc, char* argv[])
{
    // Because some of the feature extraction code uses std::cout/std::cin
    std::ios_base::sync_with_stdio(true);

    // Load sample and system configuration
    // seshat::sample s = loadSCGInk("SampleMathExps/ex.scgink");
    // seshat::math_expression recog;
    // auto hyps = recog.parse_sample(s);
    // printf("Found %zd hypothesis\n", hyps.size());

    // Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could not be initialized!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    workSDL();

    // Quit SDL
    SDL_Quit();
    return 0;
}
