#include <memory>
#include <iterator>
#include <fstream>
#include <vector>
#include <chrono>
#include <fmt/format.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "chip8.h"

constexpr int ZOOM = 4;

int grp[DISPLAY_HEIGHT * ZOOM][DISPLAY_WIDTH * ZOOM];

using namespace std::chrono;

uint64_t getMillis()
{
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

class Game
{
    Display *D;
    Window win;
    XImage *image;

public:
    Game()
    {
        D = XOpenDisplay(NULL);
        auto visual = DefaultVisual(D, 0);
        win = XCreateSimpleWindow(D, RootWindow(D, 0), 0, 0, DISPLAY_WIDTH * ZOOM, DISPLAY_HEIGHT * ZOOM, 1, 0, 0);
        XSelectInput(D, win, ExposureMask | KeyPressMask | KeyReleaseMask);
        XStoreName(D, win, "CHIP-8 Interpreter");
        XMapWindow(D, win);
        image = XCreateImage(D, visual, 24, ZPixmap, 0, (char *)grp, DISPLAY_WIDTH * ZOOM, DISPLAY_HEIGHT * ZOOM, 32, 0);
    }

    void redraw()
    {
        XPutImage(D, win, DefaultGC(D, 0), image, 0, 0, 0, 0, DISPLAY_WIDTH * ZOOM, DISPLAY_HEIGHT * ZOOM);
    }

    void renderVram(Chip8Display &vram)
    {
        for (int y = 0; y < DISPLAY_HEIGHT; y++)
        {
            auto oy = y * ZOOM;
            for (int x = 0; x < DISPLAY_WIDTH; x++)
            {
                auto ox = x * ZOOM;
                for (int i = 0; i < ZOOM; i++)
                {
                    for (int j = 0; j < ZOOM; j++)
                    {
                        grp[oy + i][ox + j] = vram[y][x] ? 0xFFFFFF : 0x000000;
                    }
                }
            }
        }
    }

    bool processEvent(Chip8 &chip8)
    {
        while (1)
        {
            static union
            {
                XEvent event;
                XMotionEvent motion;
                XKeyEvent key;
                XButtonEvent button;
                XClientMessageEvent clientMessage;
            } ev;
            Bool t = XCheckWindowEvent(D, win, ~0L, &ev.event);
            if (!t)
                return true;
            switch (ev.event.type)
            {
            case Expose:
                redraw();
                break;
            // case ButtonPress:;
            //     click(ev.button.x, ev.button.y);
            case KeyPress:;
                {
                    KeySym key = XLookupKeysym(&ev.key, 0);
                    if (!key)
                        break;
                    switch (key)
                    {
                    case XK_1:
                        chip8.keyboard[1] = true;
                        break;
                    case XK_2:
                        chip8.keyboard[2] = true;
                        break;
                    case XK_3:
                        chip8.keyboard[3] = true;
                        break;
                    case XK_4:
                        chip8.keyboard[0xC] = true;
                        break;
                    case XK_q:
                        chip8.keyboard[4] = true;
                        break;
                    case XK_w:
                        chip8.keyboard[5] = true;
                        break;
                    case XK_e:
                        chip8.keyboard[6] = true;
                        break;
                    case XK_r:
                        chip8.keyboard[0xD] = true;
                        break;
                    case XK_a:
                        chip8.keyboard[7] = true;
                        break;
                    case XK_s:
                        chip8.keyboard[8] = true;
                        break;
                    case XK_d:
                        chip8.keyboard[9] = true;
                        break;
                    case XK_f:
                        chip8.keyboard[0xE] = true;
                        break;
                    case XK_z:
                        chip8.keyboard[0xA] = true;
                        break;
                    case XK_x:
                        chip8.keyboard[0] = true;
                        break;
                    case XK_c:
                        chip8.keyboard[0xB] = true;
                        break;
                    case XK_v:
                        chip8.keyboard[0xF] = true;
                        break;
                    }
                    break;
                }
            case KeyRelease:;
                {
                    KeySym key = XLookupKeysym(&ev.key, 0);
                    if (!key)
                        break;
                    switch (key)
                    {
                    case XK_1:
                        chip8.keyboard[1] = false;
                        break;
                    case XK_2:
                        chip8.keyboard[2] = false;
                        break;
                    case XK_3:
                        chip8.keyboard[3] = false;
                        break;
                    case XK_4:
                        chip8.keyboard[0xC] = false;
                        break;
                    case XK_q:
                        chip8.keyboard[4] = false;
                        break;
                    case XK_w:
                        chip8.keyboard[5] = false;
                        break;
                    case XK_e:
                        chip8.keyboard[6] = false;
                        break;
                    case XK_r:
                        chip8.keyboard[0xD] = false;
                        break;
                    case XK_a:
                        chip8.keyboard[7] = false;
                        break;
                    case XK_s:
                        chip8.keyboard[8] = false;
                        break;
                    case XK_d:
                        chip8.keyboard[9] = false;
                        break;
                    case XK_f:
                        chip8.keyboard[0xE] = false;
                        break;
                    case XK_z:
                        chip8.keyboard[0xA] = false;
                        break;
                    case XK_x:
                        chip8.keyboard[0] = false;
                        break;
                    case XK_c:
                        chip8.keyboard[0xB] = false;
                        break;
                    case XK_v:
                        chip8.keyboard[0xF] = false;
                        break;
                    }
                }
            }
        }
    }
};

int main()
{
    auto game = Game();
    auto chip8 = Chip8();
    // load the test file
    std::ifstream infile("./tetris.rom", std::ios_base::binary);
    std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(infile), {});
    chip8.load(buffer);
    // we should load the example file i guess
    while (game.processEvent(chip8))
    {
        chip8.cycle();
        chip8.timer(getMillis());
        game.renderVram(chip8.vram);
        game.redraw();
    }

    return 0;
}