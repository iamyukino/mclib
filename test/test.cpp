/*
    color.h     ok
    display.h   ok
    draw.h      X   <- TODO 
    event.h     X
    font.h      X
    image.h     X
    key.h       X
    mixer.h     X
    mouse.h     X
    surface.h   X   <- WAIT 
    timer.h     ok
    music.h     ok
    mclib.h     ok

    pyobj.h
    clog4m.h
    mcl_base.h
    mcl_control.h
*/

#include "../src/mclib.h"
// #include <windows.h>
using namespace mcl;

int main()
{
    clog4m.init("log\\").enable_event_level(cll4m.All);
    display.set_mode(nullptr, dflags.Resizable)
           .set_window_alpha(.3);
    register_quit([] { clog4m[cll4m.Off] << "Hello, World!"; });

    auto r1 = draw.rect(display.get_surface(), rgba(0, 255, 255, .6f), { 280, 100, -300, 150 });
    auto r2 = draw.rect(display.get_surface(), rgba(255, 181, 181, .5f), { 80, 150, 300, 150 }, 0, 30, -1, 10);
    auto r3 = draw.rect(display.get_surface(), rgba(181, 144, 0, .2f), { 100, 80, 300, 150 });
    display.update({ r1, r2, r3 });

    while (1) timer.wait (1);
    return 0;
}
