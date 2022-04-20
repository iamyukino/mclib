/*
    color.h     ok
    display.h   ok
    draw.h      X
    event.h     X   <- TODO 
    font.h      X
    image.h     X
    key.h       X
    locals.h    X
    mixer.h     X
    mouse.h     X
    Rect.h      X
    Surface.h   X
    time.h      ok
    music.h     ok
    mclib.h     ok

    pyobj.h
    mcl_base.h
    mcl_control.h
*/


#include "../src/mclib.h"
// #include <windows.h>
using namespace mcl;

int main()
{
    clog4m.init("log\\");// .enable_event_level(cll4m.All);
    display.set_mode(nullptr, dflags.Movable | dflags.NoMinimizeBox).set_window_alpha(.3);

    register_quit([] { display.toggle_fullscreen(); mcl::time.delay(1000); });
    register_quit([] { clog4m[cll4m.Off] << "Hello, World!"; });

    // mcl::time.delay(10000);
    return 0;
}
