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
#include <SDL.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ranges>
#include <seshat/seshat.hpp>

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

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#ifdef SESHAT_HYPOTHESIS_TREE
static void printTree(const seshat::hypothesis& hyp, const std::size_t idx = 0, const std::string& prefix = "", bool haveAnyLeft = false)
{
    printf("%s", prefix.c_str());
    printf("%s", (haveAnyLeft ? "├──" : "└──"));

    const auto& node = hyp.tokens[idx];
    // print the value of the node
    fputc(' ', stdout);
    fwrite(node.data.data(), 1, node.data.size(), stdout);
    fputc('\n', stdout);

    std::size_t i = 0;
    const auto children = hyp.tree[idx];
    const auto nextprefix = prefix + (haveAnyLeft ? "│   " : "    ");
    for (const auto child : children) {
        printTree(hyp, child.child_id, nextprefix, ++i != children.size());
    }
}
#endif

static void surfaceDrawRectangle(SDL_Surface* surf, unsigned x, unsigned y, unsigned w, unsigned h)
{
    SDL_LockSurface(surf);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_FillRect(surf, &rect, SDL_MapRGBA(surf->format, 0, 0, 0, 255));

    SDL_UnlockSurface(surf);
}
static void surfaceDrawSquare(SDL_Surface* surf, unsigned x, unsigned y, unsigned size)
{
    surfaceDrawRectangle(surf, x - size / 2, y - size / 2, size, size);
}
static void surfaceDrawLine(SDL_Surface* surf, int x1, int y1, int x2, int y2, unsigned size)
{
    // https://github.com/dschmenk/Bresen-Span/blob/master/src/fastline.c
    int dx2, dy2, err, sx, sy, ps;
    int shorterr, shortlen, longerr, longlen, halflen;

    auto hspan = [surf, size](int xbeg, int xend, int y) {
        surfaceDrawRectangle(surf, xbeg - size / 2, y - size / 2, (xend - (xbeg - size / 2)) + size, size);
    };
    auto vspan = [surf, size](int x, int ybeg, int yend) {
        surfaceDrawRectangle(surf, x - size / 2, ybeg - size / 2, size, (yend - (ybeg - size / 2)) + size);
    };

    sx = sy = 1;
    if ((dx2 = (x2 - x1) * 2) < 0)
    {
        sx  = -1;
        dx2 = -dx2;
    }
    if ((dy2 = (y2 - y1) * 2) < 0)
    {
        sy  = -1;
        dy2 = -dy2;
    }
    if (dx2 >= dy2)
    {
        if (sx < 0)
        {
            ps = x1; x1 = x2; x2 = ps;
            ps = y1; y1 = y2; y2 = ps;
            sy = -sy;
        }
        if (dy2 == 0)
        {
            hspan(x1, x2, y1);
            return;
        }
        ps  = x1;
        err = dy2 - dx2 / 2;
        while (err < 0) // Find first half-span length and error
        {
            err += dy2;
            x1++;
        }
        longlen = (x1 - ps + 1) * 2; // Long-span length = half-span length * 2
        longerr = err * 2;
        if (longerr >= dy2)
        {
            longerr -= dy2;
            longlen--;
        }
        shortlen = longlen - 1; // Short-span length = long-span length - 1
        shorterr = longerr - dy2;
        err     += shorterr; // Do a short-span step
        while (x1 < x2)
        {
            hspan(ps, x1, y1);
            y1 += sy;     // Move to next span
            ps  = x1 + 1; // Start of next span = end of previous span + 1
            if (err >= 0) // Short span
            {
                err += shorterr;
                x1  += shortlen;
            }
            else          // Long span
            {
                err += longerr;
                x1  += longlen;
            }
        }
        hspan(ps, x2, y2); // Final span
    }
    else
    {
        if (sy < 0)
        {
            ps = x1; x1 = x2; x2 = ps;
            ps = y1; y1 = y2; y2 = ps;
            sx = -sx;
        }
        if (dx2 == 0)
        {
            vspan(x1, y1, y2);
            return;
        }
        ps  = y1;
        err = dx2 - dy2 / 2;
        while (err < 0)
        {
            err += dx2;
            y1++;
        }
        longlen = (y1 - ps + 1) * 2;
        longerr = err * 2;
        if (longerr >= dx2)
        {
            longerr -= dx2;
            longlen--;
        }
        shortlen = longlen - 1;
        shorterr = longerr - dx2;
        err     += shorterr;
        while (y1 < y2)
        {
            vspan(x1, ps, y1);
            x1 += sx;
            ps  = y1 + 1;
            if (err >= 0) // Short span
            {
                err += shorterr;
                y1  += shortlen;
            }
            else          // Long span
            {
                err += longerr;
                y1  += longlen;
            }
        }
        vspan(x2, ps, y2); // Final span
    }
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
    for (const auto& strk : s.strokes) {
        for (const auto& [oldpt, newpt] : strk.points | std::views::adjacent<2>) {
            surfaceDrawLine(surf, oldpt.x, oldpt.y, newpt.x, newpt.y, 4);
        }
    }
}

static void workSDL()
{
    // Create window
    WindowPtr window_ptr{ SDL_CreateWindow("seshat math input panel",
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SDL_WINDOWPOS_UNDEFINED,
                                           SCREEN_WIDTH, SCREEN_HEIGHT,
                                           SDL_WINDOW_SHOWN) };
    const auto window = window_ptr.get();
    if (!window) {
        std::cout << "Window could not be created!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Create renderer
    RendererPtr renderer_ptr{ SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
    const auto renderer = renderer_ptr.get();
    if (!renderer) {
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
    while (!quit) {
        SDL_Event e;

        while (SDL_PollEvent(&e)) {
            switch (e.type) {
            case SDL_MOUSEBUTTONDOWN: {
                const auto& button = e.button;
                if (button.x >= squareRect.x && button.x < (squareRect.x + squareRect.w)
                    && button.y >= squareRect.y && button.y < (squareRect.y + squareRect.h)) {
                    if (button.button == SDL_BUTTON_LEFT && !drawing) // ACTION: BEGIN STROKE
                    {
                        drawing = true;
                        handled_last_draw = true;
                        s.total_points += 1;
                        s.strokes.emplace_back();
                        const auto& pt = s.strokes.back().points.emplace_back(button.x - squareRect.x, button.y - squareRect.y);
                        surfaceDrawSquare(surf, pt.x, pt.y, 4);
                    }
                    else if (button.button == SDL_BUTTON_RIGHT && !drawing) // ACTION: REMOVE LAST STROKE
                    {
                        if (!s.strokes.empty()) {
                            s.total_points -= s.strokes.back().points.size();
                            s.strokes.pop_back();
                            time_of_last_up = std::chrono::system_clock::now();
                            handled_last_draw = s.strokes.empty(); // if empty, don't try parsing
                            surfaceRedraw(surf, s);
                        }
                    }
                }
                break;
            }
            case SDL_MOUSEMOTION: {
                const auto& motion = e.motion;
                if (drawing) {
                    if (motion.x >= squareRect.x && motion.x < (squareRect.x + squareRect.w)
                        && motion.y >= squareRect.y && motion.y < (squareRect.y + squareRect.h)) // ACTION: ADD NEW POINT TO LAST STROKE
                    {
                        const auto prev_pt = s.strokes.back().points.back();
                        const auto dx = prev_pt.x - (motion.x - squareRect.x);
                        const auto dy = prev_pt.y - (motion.y - squareRect.y);
                        if ((dx * dx + dy * dy) >= 16.0f) // add the point if far away enough
                        {
                            s.total_points += 1;
                            const auto& pt = s.strokes.back().points.emplace_back(motion.x - squareRect.x, motion.y - squareRect.y);
                            surfaceDrawLine(surf, prev_pt.x, prev_pt.y, pt.x, pt.y, 4);
                        }
                    }
                }
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                const auto& button = e.button;
                if (button.button == SDL_BUTTON_LEFT && drawing) // ACTION: END STROKE
                {
                    time_of_last_up = std::chrono::system_clock::now();
                    drawing = false;
                    handled_last_draw = false;
                }
                break;
            }
            case SDL_KEYDOWN: {
                const auto& key = e.key;
                if (!drawing && key.keysym.sym == SDLK_ESCAPE) // ACTION: REMOVE ALL STROKES
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
        if (!drawing && !handled_last_draw && time_since_last_release.count() >= 1000 && !s.strokes.empty()) {
            handled_last_draw = true;

            printf("Parsing %zd strokes with %zd total points\n", s.strokes.size(), s.total_points);
            auto hyps = recog.parse_sample(s);
            printf("Found %zd hypothesis\n", hyps.size());
            for (const auto& hyp : hyps) {
#ifdef SESHAT_HYPOTHESIS_TREE
                if (hyp.tokens.empty())
                    continue;
                printTree(hyp);
#else
                printf("%s\n", hyp.repr.c_str());
#endif
            }
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

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not be initialized!" << std::endl
                  << "SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    workSDL();

    // Quit SDL
    SDL_Quit();
    return 0;
}
