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

    draw.rect(display.get_surface(), { 20, 100, 100, 500 });

    timer.delay(10000);
    return 0;
}
