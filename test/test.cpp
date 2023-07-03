/*
Most useful stuff:
    color.h       ok
    display.h     ok
    draw.h        X 
    event.h       ok
    font.h        X
    image.h       ok  <- extended type, mask, low-bit image
    key.h         X   <- TODO 
    mixer.h       X
    mouse.h       ok
    surface.h     ok
    timer.h       ok
    music.h       ok
    mclib.h       ok

Advanced stuff:
    cursors.h     ok
    mask.h        X
    transform.h   X
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

    char const* ctest =
        "XX                      "
        "XXX                     "
        "XXXX                    "
        "XX.XX                   "
        "XX..XX                  "
        "XX...XX                 "
        "XX....XX                "
        "XX.....XX               "
        "XX..oo..XX              "
        "XX.oooo..XX             "
        "XX.oooo...XX            "
        "XX..oo....XXX           "
        "XX......XXXXX           "
        "XX.XXX..XX              "
        "XXXX XX..XX             "
        "XX   XX..XX             "
        "     XX..XX             "
        "      XX..XX            "
        "      XX..XX            "
        "       XXXX             "
        "       XX               "
        "                        "
        "                        "
        "                        ";
    auto tup = cursors.compile (ctest);
    cursor_t arrow({ 24, 24 }, { 0, 0 }, *tup);

    mixer.music.load("..\\test\\sakura.mp3");
    mixer.music.play(2, 265);
    mixer.music.set_endevent(event.custom_type());
    event.set_grab_mouse (true);

    tclock_t ck; ck.tick_busy_loop();
    point2d_t point{0, 0};
    while (1) {
        // test event.get
        for (auto&& ev : event.get ()) {
            if (ev.type >= event.UserEventMin) {
                clog4m[cll4m.Info] << event.event_name(ev.type) << ": " << (ev.type);
                continue;
            }
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
                    clog4m[cll4m.Info] << event.event_name(ev.type) << ": " << key.name(ev.key.key)
                        << " (" << ev.key.unicode << ", " << int(ev.key.scancode) << ")";
                    break;
                }
                case event.ActiveEvent: {
                    clog4m[cll4m.Info] << event.event_name(ev.type) << ": " << (ev.active.state);
                    break;
                }
                case event.MouseButtonUp: {
                    if (ev.mouse.buttons & mouse.BtnCtrl)
                        display.toggle_fullscreen();
                    // mouse.set_pos ({20, 20});
                    break;
                }
                case event.KeyUp: {
                    if (ev.key.key == key.VkEscape)
                        ::exit(0);
                    if (ev.key.mod & key.ModMode)
                        exit(0);

                    static cursor_t cur;
                    if (ev.key.key == key.VkH)
                        mouse.set_visible (false);
                    else if (ev.key.unicode == 's' || ev.key.unicode == 'S')
                        mouse.set_visible (true);
                    else if (ev.key.unicode == 'r')
                        cur = mouse.get_cursor ();
                    else if (ev.key.unicode == 'w')
                        mouse.set_cursor (cur);
                    else if (ev.key.key >= key.Vk1 && ev.key.key <= key.Vk9) {
                        if ((key.get_mods() & key.ModLShift) || key.get_pressed()[key.VkLControl]) {
                            surface_t sur = display.get_surface();
                            sur.resize ({25, 33});
                            static sys_cursor_t test = 0;
                            if (ev.key.key == key.Vk1)
                                mouse.set_cursor(arrow);
                            else if (ev.key.key == key.Vk2)
                                mouse.set_cursor({ 0, 0 }, sur);
                            else if (ev.key.key == key.Vk3) {
                                if (!mouse.set_cursor(++ test))
                                    mouse.set_cursor(test = 0);
                            }
                            else if (ev.key.key == key.Vk4)
                                mouse.set_cursor();
                        }
                        else {
                            if (ev.key.key == key.Vk8) {
                                key.set_repeat(true);
                                continue;
                            } else if (ev.key.key == key.Vk9) {
                                key.set_repeat(false);
                                continue;
                            }
                            int ct = ev.key.key - key.Vk0;
                            static eventtype_t un = event.custom_type();
                            if (mouse.get_visible())
                                timer.set_timer(un, ct * 500, ct - 1);
                            else
                                timer.set_timer(un, 0);
                        }
                    }
                    break;
                }
                default: {
                    clog4m[cll4m.Info] << event.event_name (ev.type) << " ("
                        << ev.window.wParam << ", " << ev.window.lParam << ")   ";
                    break;
                }
            }
        }

        // test surface.fill & bilt
        display.get_surface().fill(lightblue);
        display.get_surface().bilt(bki, 0, 0, blend.Ovl_rgba);
        display.get_surface().fill(rgba(255, 181, 181, .5f), { point.x - 150, point.y - 75, 300, 150 }, blend.Ovl_rgba);
        
        // test surface.resize
        cpy.resize(display.get_window_size());
        cpy.bilt(display.get_surface());
        display.get_surface() = cpy;

        // same as display.flip();
        auto sz = display.get_window_size();
        display.update({ 0, 0, sz.x, sz.y });
        
        ck.tick(24);
        // clog4m[cll4m.Trace] << timer.get_ticks() << "(" << ck.get_time_us() << ", " << ck.get_fps() << ")";

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
