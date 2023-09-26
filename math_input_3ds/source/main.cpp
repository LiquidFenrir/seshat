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
#include <3ds.h>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <ranges>
#include <seshat/seshat.hpp>

struct color_t {
    u8 r, g, b, a;
};
struct position_t {
    u16 x, y;
};
static void fbDrawPixel(u8* fb, position_t position, color_t color)
{
    if (position.x >= 400 || position.y >= 240)
        return;

    u8* fb_ptr = &fb[(position.x * 240 + (240 - position.y - 1)) * 3];
    fb_ptr[0] = color.r;
    fb_ptr[1] = color.g;
    fb_ptr[2] = color.b;
}
static void fbDrawRectangle(u8* fb, position_t position, position_t dimensions, color_t color)
{
    for (int dx = 0; dx < dimensions.x; ++dx)
        for (int dy = 0; dy < dimensions.y; ++dy)
            fbDrawPixel(fb, position_t{ u16(position.x + dx), u16(position.y + dy) }, color);
}
static void fbDrawSquare(u8* fb, position_t position, unsigned size, color_t color)
{
    fbDrawRectangle(fb, {position.x - size / 2, position.y}, {size, size}, color);
}
static void fbDrawLine(u8* fb, position_t p1, position_t p2, unsigned size, color_t color)
{
    int x1 = p1.x;
    int y1 = p1.y;
    int x2 = p2.x;
    int y2 = p2.y;
    // https://github.com/dschmenk/Bresen-Span/blob/master/src/fastline.c
    int dx2, dy2, err, sx, sy, ps;
    int shorterr, shortlen, longerr, longlen, halflen;

    auto hspan = [fb, size, color](int xbeg, int xend, int y) {
        fbDrawRectangle(fb, {xbeg - size / 2, y}, {xend - (xbeg - size / 2) + size, size}, color);
    };
    auto vspan = [fb, size, color](int x, int ybeg, int yend) {
        fbDrawRectangle(fb, {x, ybeg - size / 2}, {size, yend - (ybeg - size / 2) + size}, color);
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

static void fbFill(u8* surf, color_t color)
{
    for (int off = 0; off < 320 * 240; ++off) {
        surf[0] = color.r;
        surf[1] = color.g;
        surf[2] = color.b;
        surf += 3;
    }
}
static void fbRedraw(u8* fb, const seshat::sample& s)
{
    const color_t fillcolor{ 255, 255, 255, 255 };
    const color_t drawcolor{ 0, 0, 0, 255 };
    fbFill(fb, fillcolor);
    for (const auto& strk : s.strokes) {
        for (const auto& [prev_pt, pt] : strk.points | std::views::adjacent<2>) {
            fbDrawLine(fb, position_t{prev_pt.x, prev_pt.y}, position_t{ pt.x, pt.y }, 4, drawcolor);
        }
    }
}

static void work3DS()
{
    // Event loop exit flag
    bool quit = false;
    auto time_of_last_up = std::chrono::system_clock::now();
    bool drawing = false;
    bool handled_last_draw = true;
    seshat::sample s;
    seshat::math_expression recog("romfs:/CONFIG");

    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    u8* const fb = gfxGetFramebuffer(GFX_BOTTOM, GFX_LEFT, nullptr, nullptr);
    fbFill(fb, { 255, 255, 255, 255 });
    std::vector<seshat::hypothesis> hyps;

    // Event loop
    while (!quit) {
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();
        const u32 kDown = hidKeysDown();
        const u32 kHeld = hidKeysHeld();
        const u32 kUp = hidKeysUp();

        if (kDown & KEY_TOUCH) {
            touchPosition button;
            hidTouchRead(&button);

            drawing = true;
            handled_last_draw = true;
            s.total_points += 1;
            s.strokes.emplace_back();
            const auto& pt = s.strokes.back().points.emplace_back(button.px, button.py);
            fbDrawSquare(fb, position_t{ pt.x, pt.y }, 4, color_t{ 0, 0, 0, 255 });
        } else if (kHeld & KEY_TOUCH) {
            touchPosition motion;
            hidTouchRead(&motion);

            if (drawing) {
                const auto prev_pt = s.strokes.back().points.back();
                const auto dx = prev_pt.x - motion.px;
                const auto dy = prev_pt.y - motion.py;
                if ((dx * dx + dy * dy) >= 16.0f) // add the point if far away enough
                {
                    s.total_points += 1;
                    const auto& pt = s.strokes.back().points.emplace_back(motion.px, motion.py);
                    fbDrawLine(fb, position_t{prev_pt.x, prev_pt.y}, position_t{ pt.x, pt.y }, 4, color_t{ 0, 0, 0, 255 });
                }
            }
        } else if (kUp & KEY_TOUCH) {
            if (drawing) // ACTION: END STROKE
            {
                time_of_last_up = std::chrono::system_clock::now();
                drawing = false;
                handled_last_draw = false;
            }
        }

        if (kDown & KEY_B) {
            if (!drawing) // ACTION: REMOVE LAST STROKE
            {
                if (!s.strokes.empty()) {
                    s.total_points -= s.strokes.back().points.size();
                    s.strokes.pop_back();
                    time_of_last_up = std::chrono::system_clock::now();
                    handled_last_draw = s.strokes.empty(); // if empty, don't try parsing
                    fbRedraw(fb, s);
                }
            }
        }

        if (kDown & KEY_X) {
            if (!drawing) // ACTION: REMOVE ALL STROKES
            {
                s.total_points = 0;
                s.strokes.clear();
                fbFill(fb, { 255, 255, 255, 255 });
                handled_last_draw = true;
            }
        }

        if (kDown & KEY_START) {
            quit = true;
        }

        const auto time_since_last_release = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - time_of_last_up);
        if (!drawing && !handled_last_draw && time_since_last_release.count() >= 1000 && !s.strokes.empty()) {
            handled_last_draw = true;

            printf("Parsing %zd strokes with %zd total points\n", s.strokes.size(), s.total_points);
            hyps.clear();
            std::cerr << "Starting a big think at " << std::chrono::system_clock::now() << std::endl;
            recog.parse_sample(s, hyps);
            std::cerr << "Done with thinking at " << std::chrono::system_clock::now() << std::endl;
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
    }
}

int main(int argc, char* argv[])
{
    // Because some of the feature extraction code uses std::cout/std::cin
    std::ios_base::sync_with_stdio(true);
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);
    consoleDebugInit(debugDevice_SVC);
    osSetSpeedupEnable(true);
    romfsInit();

    work3DS();

    romfsExit();
    osSetSpeedupEnable(false);
    gfxExit();
    return 0;
}
