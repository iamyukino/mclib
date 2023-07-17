/*
Most useful stuff:
    color.h       ok
    display.h     ok
    draw.h        X 
    event.h       ok
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
    clog4m.h          <- use tchar 
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
                else if (ev.key.key == key.VkTab) binput = !binput,
                    binput ? key.start_text_input() : key.stop_text_input();
                break;
            }
            
            // test text editing event
            case event.KeyDown: {
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << key.name (ev.key.key);
                break;
            }
            case event.TextInput: {
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << (ev.text.unicode ? std::wstring {ev.text.unicode, 0} : event.get_details_textinput());
                break;
            }
            case event.TextEditing: {
                size_t sz = 0;
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << event.get_details_textediting(&sz) << " (" << sz << ")";
                break;
            }
            case event.Active: {
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << ev.active.gain << ", " << ev.active.state;
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

        // event.set_grab_mouse (key.get_async_mods (key.ModNum));
        event.set_grab_key (key.get_async_mods (key.ModNum));
        // key.set_repeat (key.get_async_mods (key.ModCaps));

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
