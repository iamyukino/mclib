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
    surface.h     ok  <- set_colorkey and set_alpha 
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
    display.set_mode(0, dflags.Resizable | dflags.DoubleBuf | dflags.AllowDropping);
    // display.set_window_alpha(.3);
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
    tclock_t ck;
    
    while (1) {
        ck.tick (24);
        
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

                // test transform.flip
                if (ev.key.key == key.VkUp || ev.key.key == key.VkDown)
                    bki = transform.flip (bki, 0, 1);
                if (ev.key.key == key.VkLeft || ev.key.key == key.VkRight)
                    bki = transform.flip (bki, 1, 0);
                
                // test transform.scale_by
                static float t = 1;
                static short m = 2;
                static surface_t ss = bki;
                if (ev.key.key >= key.Vk0 && ev.key.key <= key.Vk9) {
                    m = ev.key.key - key.Vk0;
                    bki = transform.scale_by (ss, t, t, m);
                }
                if (ev.key.key == key.VkOEMPlus) {
                    t += .1f;
                    bki = transform.scale_by (ss, t, t, m);
                }
                if (ev.key.key == key.VkOEMMinus) {
                    t -= .1f;
                    bki = transform.scale_by (ss, t, t, m);
                }
                break;
            }
            }
        }

        // test surface.fill & bilt
        display.get_surface().fill(white);
        display.get_surface().bilt(bki, 0, 0, blend.Ovl_rgba);
        display.get_surface().fill(rgba(255, 181, 181, .5f), { point.x - 150, point.y - 75, 300, 150 }, blend.Ovl_rgba);
        key.set_text_input_rect({ point.x - 150, point.y - 75, 300, 150 });
        display.flip ();

        // event.set_grab_mouse (key.get_async_mods (key.ModNum));
        event.set_grab_key (key.get_async_mods (key.ModNum));
        key.set_repeat (key.get_async_mods (key.ModCaps));

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
