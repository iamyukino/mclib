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
    display.set_mode(nullptr, dflags.Resizable);
    display.set_window_alpha(.3);

    // test image.load
    bool is_test = false;
    surface_t bki = image.load("..\\test\\bg.bmp");
    if (!bki)
        bki = image.load("..\\bg.bmp"), is_test = true;

    // test register_quit
    register_quit([is_test] { image.save(display.get_surface(), is_test ? "test.bmp" : "..\\test\\debug\\test.bmp"); });
    register_quit([] { clog4m[cll4m.Off] << "Hello, World!"; });

    // test image.frombuffer
    color_t* buf = new color_t[100 * 100];
    for (color_t i = 0; i < 100; ++i)
        for (color_t j = 0; j < 100; ++j)
            buf[j * 100 + i] = black;
    surface_t s4buf = image.frombuffer(buf, { 100, 100 });
    for (color_t i = 50; i < 90; ++i)
        for (color_t j = 20; j < 60; ++j)
            buf[j * 100 + i] = rgba(i, j, 0);
    
    POINT point{ 0, 0 };
    blend_t bl = blend.Ovl_rgba;
    while (1) {
        ::GetCursorPos (&point);
        ::ScreenToClient (display.get_wm_info()["window"], &point);
        
        // test surface.copy_constructor
        surface_t surface(display.get_surface());
        
        // test surface.fill
        auto r0 = surface.fill(white);

        // test surface.get_buffer (bufferproxy)
        do {
            auto pbuf = surface.get_buffer();
            if (!pbuf) break;
            pbuf = 120000 + pbuf;
            for (auto i = pbuf.begin(); i != pbuf.end() - 120000; ++i)
                *i = purple;
            
            surface = pbuf.get_surface (); // no need
        } while (0);
        
        // test surface.fill
        auto r1 = surface.fill(rgba(0, 255, 255, .6f), { 280, 100, -300, 150 }, bl);
        auto r2 = surface.fill(rgba(255, 181, 181, .5f), { 80, 150, 300, 150 }, bl);

        // test surface.bilt
        surface_t su_c = display.get_surface();
        surface.bilt(bki, { 300, 5 }, 0, blend.Copy);
        surface.bilt(s4buf, { 20, 10 });
        su_c.bilt(surface, 0, 0, blend.Copy);
        su_c.fill(rgba(255, 255, 255, .4f), 0, blend.Ovl_rgba);
        su_c.bilt(surface, { point.x - 150, point.y - 75 }, { point.x - 150, point.y - 75, 300, 150 }, blend.Copy);
        display.get_surface().bilt(su_c);
        
        // test display.update
        display.update({ r0 });
        timer.wait(20);
    }
    
    // while(1) timer.wait(1);
    return 0;
}
