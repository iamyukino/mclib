/*
    color.h       ok
    display.h     ok
    draw.h        X   <- WAIT 
    event.h       X
    font.h        X
    image.h       X
    key.h         X
    mixer.h       X
    mouse.h       X
    surface.h     ok
    timer.h       ok
    music.h       ok
    mclib.h       ok
    bufferproxy.h ok

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
    // test clog4m
    clog4m.init("log\\").enable_event_level(cll4m.All);
    
    // test display.init
    display.set_mode(nullptr, dflags.Resizable);
    display.set_window_alpha(.3);
    
    // test register_quit
    register_quit([] { clog4m[cll4m.Off] << "Hello, World!"; });
  
    POINT point{ 0, 0 };
    blend_t bl = blend.Ovl_rgba;
    while (1) {
        GetCursorPos (&point);
        ScreenToClient (display.get_wm_info()["window"], &point);
        
        // test surface.copy_constructor
        surface_t surface(display.get_surface());
        
        // test surface.fill
        auto r0 = surface.fill(white);

        // test surface.get_buffer (bufferproxy)
        {
            auto pbuf = surface.get_buffer();
            pbuf = 120000 + pbuf;
            for (auto i = pbuf.begin(); i != pbuf.end() - 120000; ++i)
                *i = purple;
            
            surface = pbuf.get_surface (); // no need
        }
        
        // test surface.fill
        auto r1 = surface.fill(rgba(0, 255, 255, .6f), { 280, 100, -300, 150 }, bl);
        auto r2 = surface.fill(rgba(255, 181, 181, .5f), { 80, 150, 300, 150 }, bl);

        // test surface.bilt
        display.get_surface().bilt(surface, 0, 0, blend.Copy);
        display.get_surface().fill(rgba(255, 255, 255, .8f), 0, blend.Ovl_rgba);
        display.get_surface().bilt(surface, { point.x - 150, point.y - 75 }, { point.x - 150, point.y - 75, 300, 150 }, blend.Copy);
        
        // test display.update
        display.update({ r0 });
        timer.wait(20);
    }
    
    // while(1) timer.wait(1);
    return 0;
}
