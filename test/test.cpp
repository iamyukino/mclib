/*
    color.h     ok
    display.h   ok
    draw.h      X   <- WAIT 
    event.h     X
    font.h      X
    image.h     X
    key.h       X
    mixer.h     X
    mouse.h     X
    surface.h   X   <- TODO 
    timer.h     ok
    music.h     ok
    mclib.h     ok

    pyobj.h
    clog4m.h
    mcl_base.h
    mcl_control.h
*/


#if 0
#include "../src/mclib.h"
// #include <windows.h>
using namespace mcl;
#else
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4365 5039 4464)
# endif
# define WIN32_LEAN_AND_MEAN
# include "../src/mclib.h"
# include <windows.h>
# include <windowsx.h>
# include <process.h>
# undef WIN32_LEAN_AND_MEAN
using namespace mcl;
# ifdef _MSC_VER
#  pragma warning(pop)
# endif
#endif

int main()
{
    clog4m.init("log\\").enable_event_level(cll4m.All);
    display.set_mode(nullptr, dflags.Resizable);
    display.set_window_alpha(.3);
    register_quit([] { clog4m[cll4m.Off] << "Hello, World!"; });
  
    POINT point{ 0, 0 };
    blend_t bl = blend.Over_rgba;
    while (1) {
        GetCursorPos (&point);
        ScreenToClient (display.get_wm_info()["window"], &point);
        
        auto r0 = display.get_surface().fill(white);
        auto r1 = display.get_surface().fill(rgba(0, 255, 255, .6f), { 280, 100, -300, 150 }, bl);
        auto r2 = display.get_surface().fill(rgba(255, 181, 181, .5f), { 80, 150, 300, 150 }, bl);
        auto r3 = display.get_surface().fill(rgba(181, 144, 0, .2f), { point.x - 150, point.y - 75, 300, 150 }, bl);

        display.update({ r0, r1, r2, r3 });
        timer.wait(1);
    }
    
    while(1) timer.wait(1);
    return 0;
}
