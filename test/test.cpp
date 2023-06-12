/*
Most useful stuff:
    color.h       ok
    display.h     ok
    draw.h        X   <- WAIT 
    event.h       ok
    font.h        X
    image.h       ok
    key.h         X
    mixer.h       X
    mouse.h       X   <- TODO 
    surface.h     ok
    timer.h       ok
    music.h       ok
    mclib.h       ok

Advanced stuff:
    cursors.h     X   <- TODO 
    bufferproxy.h ok
    transform.h   X

Other:
    pyobj.h
    clog4m.h
    mcl_base.h
    mcl_control.h
*/


# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4464)
# endif

#include "../src/mclib.h"
using namespace mcl;

# ifdef _MSC_VER
#  pragma warning(pop)
# endif

int main()
{
    // test clog4m
    clog4m.init().enable_event_level(cll4m.All);
    
    // test display.init
    display.set_mode(nullptr, dflags.Resizable | dflags.DoubleBuf);// | dflags.NoFrame);
    display.set_window_alpha(.3);
    if (!display) return 0;

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

    point2d_t point{0, 0};
    while (1) {
        // test event.get
        for (auto&& ev : event.get ()) {
            switch (ev.type) {
                case event.Quit: {
                    ::exit(0);
                    break;
                }
                case event.MouseMotion: {
                    point = ev.mouse.pos;
                    break;
                }
                case event.KeyDown: {
                    clog4m[cll4m.Info] << event.event_name(ev.type) << ": " << ev.key.unicode;
                    break;
                }
                case event.ActiveEvent: {
                    clog4m[cll4m.Info] << event.event_name(ev.type) << ": " << (ev.active.state);
                    break;
                }

                case event.MouseButtonUp: {
                    display.toggle_fullscreen();
                    mouse.set_pos ({20, 20});
                    break;
                }
                case event.KeyUp: {
                    if (ev.key.unicode == 27)
                        ::exit(0);

                    if (ev.key.unicode == 'h' || ev.key.unicode == 'H')
                        mouse.set_visible (false);
                    else if (ev.key.unicode == 's' || ev.key.unicode == 'S')
                        mouse.set_visible (true);
                    // no break
                }
                default: {
                    clog4m[cll4m.Info] << event.event_name (ev.type) << " (" << ev.window.wParam << ", " << ev.window.lParam
                        << ")   ";
                    // for (auto i : mouse.get_pressed()) clog4m[cll4m.Info] << i << " _";
                    break;
                }
            }
        }

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
