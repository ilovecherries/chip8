#include <iostream>
#include <memory>
#include <iterator>
#include <fstream>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "chip8.h"

constexpr int ZOOM = 16;

int grp[DISPLAY_HEIGHT * ZOOM][DISPLAY_WIDTH * ZOOM];

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

    bool processEvent()
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
                // case KeyPress:;
                //     KeySym key = XLookupKeysym(&ev.key, 0);
                //     if (!key)
                //         break;
                //     if (key == XK_space)
                //     {
                //         space();
                //     }
            }
        }
    }
};

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "chip8 [filename]" << std::endl;
        return 0;
    }
    auto game = Game();
    auto chip8 = Chip8();
    // this is an example for drawing to the vram i suppose
    for (int i = 0; i < DISPLAY_HEIGHT; i++)
    {
        chip8.vram[i][i] = true;
    }
    // load the test file
    std::fstream is(argv[1]);
    std::istream_iterator<uint8_t> start(is), end;
    std::vector<uint8_t> file(start, end);
    chip8.load(file);
    // we should load the example file i guess
    while (game.processEvent())
    {
        std::cout << "Hello, World!" << std::endl;
        chip8.cycle();
        game.renderVram(chip8.vram);
        game.redraw();
    }

    return 0;
}