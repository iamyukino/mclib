/*
Most useful stuff:
    color.h       ok
    display.h     ok
    draw.h        X 
    event.h       ok  <- set_grab_key 
    font.h        X
    image.h       ok  <- extended type, mask, low-bit image 
    key.h         ok  <- font of IME editor 
    mixer.h       X
    mouse.h       ok
    surface.h     ok
    timer.h       ok
    music.h       ok
    mclib.h       ok

Advanced stuff:
    cursors.h     ok
    mask.h        X
    transform.h   X   <- TODO 
    bufferproxy.h ok

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
    display.set_mode(nullptr, dflags.Resizable | dflags.DoubleBuf | dflags.AllowDropping);
    display.set_window_alpha(.3);
    if (!display) return 0;

    // test image.load
    bool is_test = false;
    surface_t bki = image.load("..\\test\\bg.bmp");
    if (!bki) bki = image.load("..\\bg.bmp"), is_test = true;

    // test display.set_icon
    surface_t cpy = bki;
    cpy.resize({ 512, 512 });
    display.set_icon(cpy);

    // test tclock_t
    point2d_t point{ 0, 0 };
    bool binput = true;
    tclock_t ck; ck.tick();
    
    while (1) {
        // test event.get
        for (auto&& ev : event.get ()) {
            switch (ev.type) {
            case event.Quit:        { ::exit(0); }
            case event.MouseMotion: { point = ev.mouse.pos; break; }
            case event.KeyUp: {
                if (ev.key.key == key.VkEscape) ::exit(0);
                else if (ev.key.key == key.VkLControl) binput = !binput,
                    binput ? key.start_text_input() : key.stop_text_input();
                break;
            }

            // test text editing event
            case event.TextInput: {
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << event.get_details_textinput();
                break;
            }
            case event.TextEditing: {
                size_t sz = 0;
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << event.get_details_textediting(&sz) << " (" << sz << ")";
                break;
            }
            }
        }

        // test surface.fill & bilt
        display.get_surface().fill(lightblue);
        display.get_surface().bilt(bki, 0, 0, blend.Ovl_rgba);
        display.get_surface().fill(rgba(255, 181, 181, .5f), { point.x - 150, point.y - 75, 300, 150 }, blend.Ovl_rgba);
        key.set_text_input_rect({ point.x - 150, point.y - 75, 300, 150 });

        display.flip ();
        ck.tick (24);

        // test clock.get_fps
        static char str[100];
# ifdef _MSC_VER
        sprintf_s (str, 100, "%dfps", int(ck.get_fps() + 0.5));
# else
        sprintf (str, "%dfps", int(ck.get_fps() + 0.5));
# endif
        display.set_caption(str);
    }

    return 0;
}
