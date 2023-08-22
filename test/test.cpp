/*
legend = {
    "ok" : "ok",
    "no" : "not_contain",
    "."  : "todo", 
    "?"  : "not_checked",
    "="  : "todo_wait",
    "-"  : "not_a_concern",
}
Most useful stuff:
    color.h       ok
    display.h     =
    draw.h        no <- TODO 
    event.h       =
    font.h        no
    image.h       =
    key.h         =
    mixer.h       no
    mouse.h       =
    surface.h     =
    timer.h       ok
    music.h       ok
    mclib.h       =

Advanced stuff:
    cursors.h     =
    mask.h        no
    sprite.h      no
    transform.h   =
    bufferproxy.h =
    gfxdraw.h     no
    math.h        no

Other:
    camera.h      no
    scrap.h       no
    touch.h       no

Private stuff:
    pyobj.h       -
    clog4m.h      =
    mcl_base.h    -
    mcl_control.h -
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
    surface_t bg = image.load("..\\test\\bg.bmp");
    if (!bg)  bg = image.load("..\\bg.bmp");
    // bg.convert_alpha (false);
    surface_t bki = image.load("..\\test\\test.bmp");
    if (!bki) bki = image.load("..\\test.bmp"), is_test = true;
    bki.set_alpha (0xcc);
    bki.set_colorkey (0xffffff);
    // bki = bki.convert_alpha (true);

    // test display.set_icon
    surface_t cpy = bki;
    cpy.resize({ 512, 512 });
    display.set_icon(cpy);

    // test tclock_t
    point2d_t point{ 0, 0 }, pbki{ 0, 0 }, sz{ 300, 150 };
    bool binput = false; key.stop_text_input ();
    tclock_t ck; color_t bkic = 0xffbcbc;

    blend_t bdt = blend.Alpha, bdt2 = blend.Alpha;
    display.get_surface().convert_alpha(true);
    // display.get_surface().set_colorkey(bkic);
    display.flip();
    display.get_surface().convert_alpha(true);
    // display.get_surface().set_colorkey(bkic);

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
            case event.Active: {
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << ev.active.state;
                break;
            }
            case event.ClipboardUpdate: {
                clog4m[cll4m.Info] << event.event_name(ev.type);
                break;
            }
            case event.KeyDown: {
                // test textinput
                clog4m[cll4m.Info] << event.event_name(ev.type) << ":"
                    << key.name (ev.key.key);

                // test transform
                static surface_t ss = bki;
                point2d_t off{ 0, 0 };

                if (ev.key.key == key.VkDelete) {
                    -- sz.x;
                    surface_t ccd = transform.clip(bki, { point.x, point.y, sz.x, sz.y });
                    mouse.set_cursor(cursor_t({0, 0}, ccd));
                }
                if (ev.key.key == key.VkUp)
                    bki = transform.flip (bki, 0, 1);
                if (ev.key.key == key.VkDown)
                    bki = transform.flip (bki, 1, 0);
                if (ev.key.key == key.VkLeft)
                    bki = transform.rotate (bki, 5, &off);
                if (ev.key.key == key.VkRight)
                    bki = transform.rotate (bki, -5, &off);
                if (ev.key.key == key.VkZ)
                    bki = transform.rotozoom (bki, 5, 1.2f, &off);
                if (ev.key.key == key.VkX)
                    bki = transform.rotozoom (bki, -5, 1 / 1.2f, &off);
                if (ev.key.key == key.VkC)
                    bki = transform.scale2x (bki, &off);
                if (ev.key.key == key.VkOEMPeriod)
                    bkic = transform.average_color (bki);
                if (ev.key.key == key.VkR)
                    bki = transform.grayscale (bki);
                if (ev.key.key == key.VkF)
                    bki = transform.laplacian (bki);
                if (ev.key.key == key.VkE) {
                    off.x = point.x - 150 - pbki.x;
                    off.y = point.y - 75 - pbki.y;
                    bki = transform.clip (bki, {off.x, off.y, 300, 150 });
                }
                if (ev.key.key == key.VkQ)
                    bki = transform.chop (bki, { point.x - 150 - pbki.x, point.y - 75 - pbki.y, 300, 150 });
                if (ev.key.key == key.Vk0)
                    bki = ss, pbki = { 0, 0 }, bkic = 0xffbcbc, bdt = blend.Alpha_rgb, bdt2 = blend.Alpha_rgb, sz.x = 300, mouse.set_cursor();
                if (ev.key.key == key.VkOEMPlus)
                    bki = transform.scale_by (bki, 1.2f, 1.2f, &off, 2);
                if (ev.key.key == key.VkOEMMinus)
                    bki = transform.scale_by (bki, 1.f / 1.2f, 1.f / 1.2f, &off, 2);
                if (ev.key.key == key.VkRControl)
                    key.get_async_mods(key.ModShift) ?
                        (bdt2 = bdt2 & blend.PreMultiplied ? bdt2 & ~blend.PreMultiplied : bdt2 | blend.PreMultiplied) :
                        (bdt = bdt & blend.PreMultiplied ? bdt & ~blend.PreMultiplied : bdt | blend.PreMultiplied); 
                if (ev.key.key == key.VkSpace) {
                    surface_t tbki = bki;
                    tbki.set_colorkey(bki.get_at({ 0,0 }));
                    rect_t rc = tbki.get_bounding_rect(), rc0 = tbki.get_rect();
                    clog4m[cll4m.Fatal] << "(" << rc.x << ", " << rc.y << ", " << rc.w << ", " << rc.h << ")";
                    if (rc.x != rc0.x || rc.y != rc0.y || rc.w != rc0.w || rc.h != rc0.h) {
                        tbki = transform.clip (bki, rc);
                        if (tbki) {
                            bki = tbki;
                            off.x = rc.x, off.y = rc.y;
                        }
                    }
                }
                if (ev.key.key == key.VkAdd)
                    key.get_async_mods(key.ModShift) ? (++ bdt2, bdt2 %= 8) : (++ bdt, bdt %= 8);
                if (ev.key.key == key.VkSubtract)
                    key.get_async_mods(key.ModShift) ? (bdt2 = (bdt2 ? 7 : (bdt2 % 8) - 1)) : (bdt = (bdt ? 7 : (bdt % 8) - 1));
                if (ev.key.key >= key.VkNumpad0 && ev.key.key <= key.VkNumpad9) {
                    int sw = ev.key.key - key.VkNumpad0;
                    surface_t ts(bki.get_size(), sf_nullptr.SrcAlpha);
                    ts.fill (rgba(128, 255, 64, 100));
                    clog4m[cll4m.Error] << ">> " << transform.threshold(ts, bki, 0xffffffff, 0xff222222, 0xff000000, sw, 0, false);
                    bki = ts;
                }

                pbki.x += off.x;
                pbki.y += off.y;
                if (ev.key.key == key.VkW) pbki.y -= 40;
                if (ev.key.key == key.VkS) pbki.y += 40;
                if (ev.key.key == key.VkA) pbki.x -= 40;
                if (ev.key.key == key.VkD) pbki.x += 40;

                // test toggle_dynamic_wallpaper
                if (ev.key.key == key.Vk1)
                    display.toggle_dynamic_wallpaper ();
                if (ev.key.key == key.Vk2)
                    display.iconify ();
                if (ev.key.key == key.Vk3)
                    display.hide(!(display.get_flags () & dflags.Hidden));
                if (ev.key.key == key.Vk4)
                    display.maximize ();
                if (ev.key.key == key.Vk5)
                    display.toggle_fullscreen ();
                break;
            }
            }
        }

        // test surface.fill & blit
        display.get_surface().fill(bkic);
        display.get_surface().blit(bg, 0, {0, 0, 200, 200}, blend.Alpha_rgba);

        surface_t sc = bdt & blend.PreMultiplied ? bki.premul_alpha() : bki;
        display.get_surface().blit(sc, pbki, 0, bdt | (key.get_async_mods(key.ModNum) ? 0x200 : 0x200));
        
        color_t csc = rgba(255, 181, 181, 127);
        csc = bdt2 & blend.PreMultiplied ? premul_alpha(csc) : csc;
        display.get_surface().fill(csc, { point.x - 150, point.y - 75, 300, 150 }, bdt2 | (key.get_async_mods(key.ModNum) ? 0x200 : 0x200));
        key.set_text_input_rect({ point.x - 150, point.y - 75, 300, 150 });
        display.flip ();
        
        event.set_grab_mouse (key.get_async_mods (key.ModNum) != key.ModNum, false);
        event.set_grab_key (key.get_async_mods (key.ModNum) != key.ModNum, false);
        key.set_repeat (key.get_async_mods (key.ModCaps) == key.ModCaps);

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
