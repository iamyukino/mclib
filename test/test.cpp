/*
Most useful stuff:
    color.h       ok
    display.h     ok
    draw.h        X   <- WAIT 
    event.h       X   <- TODO 
    font.h        X
    image.h       ok
    key.h         X
    mixer.h       X
    mouse.h       X
    surface.h     ok
    timer.h       ok
    music.h       ok
    mclib.h       ok

Advanced stuff:
    cursors.h     X
    bufferproxy.h ok
    transform.h   X

Other:
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
    display.set_mode(nullptr, dflags.Resizable | dflags.DoubleBuf);
    display.set_window_alpha(.3);

    // test image.load
    bool is_test = false;
    surface_t bki = image.load("..\\test\\bg.bmp");
    if (!bki)
        bki = image.load("..\\bg.bmp"), is_test = true;

    // test display.set_icon
    surface_t cpy = bki;
    cpy.resize({ 512, 512 });
    display.set_icon(cpy);

    // test register_quit
    register_quit([is_test] {
        display.set_mode (0, display.get_flags() & ~dflags.DoubleBuf);
        image.save(display.get_surface(), is_test ? "test.bmp" : "..\\test\\debug\\test.bmp");
    });
    register_quit([] { clog4m[cll4m.Off] << "Hello, World!"; });
    
    POINT point{ 0, 0 };
    while (1) {
        ::GetCursorPos(&point);
        ::ScreenToClient(display.get_wm_info()["window"], &point);

        // test surface.fill & bilt
        display.get_surface().fill(white);
        display.get_surface().bilt(bki);
        display.get_surface().fill(rgba(255, 181, 181, .5f), { point.x - 150, point.y - 75, 300, 150 }, blend.Ovl_rgba);
        
        // test surface.resize
        cpy.resize(display.get_window_size());
        cpy.bilt(display.get_surface());
        display.get_surface() = cpy;

        // same as display.flip();
        auto sz = display.get_window_size();
        display.update({ 0, 0, sz.x, sz.y });
        
        timer.wait(20);
    }

    return 0;
}
