/*
    mclib (Multi-Canvas Library)
    Copyright (C) 2021-2022  Yukino Amamiya
  
    This file is part of the mclib Library. This library is
    a graphics library for desktop applications only and it's
    only for windows.

    This library is free software; you can redistribute it
    and/or modify it under the terms of the GNU Lesser General
    Public License as published by the Free Software Foundation;
    either version 2.1 of the License, or (at your option) any
    later version.

    This library is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied
    warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
    PURPOSE.  See the GNU Lesser General Public License for
    more details.

    You should have received a copy of the GNU Lesser General
    Public License along with this library; if not, write to
    the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
    
    Yukino Amamiya
    iamyukino[at outlook.com]
    
    @file cpp/display.cpp
    This is a C++11 implementation file for controling the display
    window and screen.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/mclfwd.h"
#include "../src/display.h" 
#include "../src/clog4m.h"
#include "../src/surface.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
# pragma warning(push)
# pragma warning(disable: 4365)
#endif

#include <sstream>    // for ostringstream

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {
    
    // Enum class of display flags.
    mcl_dflags_t dflags;
    // Module for controling the display.
    mcl_display_t display;

#if __cplusplus < 201703L
    dflags_t constexpr mcl_dflags_t::Shown;
    dflags_t constexpr mcl_dflags_t::Hidden;
    dflags_t constexpr mcl_dflags_t::Minimize;
    dflags_t constexpr mcl_dflags_t::Maximize;
    dflags_t constexpr mcl_dflags_t::Resizable;
    dflags_t constexpr mcl_dflags_t::NoFrame;
    dflags_t constexpr mcl_dflags_t::NoMinimizeBox;
    dflags_t constexpr mcl_dflags_t::DoubleBuf;
#endif
    
    mcl_display_t::
    operator void* () const noexcept{
        return mcl_control_obj.bIsReady ?
            const_cast<mcl_display_t*>(this) : 0;
    }
    bool mcl_display_t::
    operator! () const noexcept{
        return !mcl_control_obj.bIsReady;
    }

   /**
    * @function mcl_display_t::get_init <src/display.h>
    * @brief Tells whether the window is open.
    *     Returns True if the display module is currently initialized.
    * @return void*
    */
    bool mcl_display_t::
    get_init () const noexcept{
        return static_cast<bool>(mcl_control_obj.bIsReady);
    }
    
    static void
    mcl_set_caption (wchar_t const* caption) noexcept 
    {
        if (caption) {
        // caption title is given
            MCL_WCSNCPY (mcl_control_obj.window_caption, caption, _MAX_FNAME);
            return ;
        }
        
        wchar_t lpFileRealName[MAX_PATH];
        DWORD ret = ::GetModuleFileNameW (nullptr, lpFileRealName, MAX_PATH);
        if (ret) {
        // succeeded. then get the file name
            MCL_WGETFILENAME ( lpFileRealName, mcl_control_obj.window_caption );
            return ;
        }
        
        // failed. use default caption "mclib"
        clog4m[cll4m.Warn] <<
           L"title = \nerror:  Failed to get module path "
           L"[-Wdisplay-winapi-" << ::GetLastError () << L"]\n, default ";
        if (!mcl_control_obj.window_caption[0])
            ::memcpy (mcl_control_obj.window_caption, L"mclib", 6 * sizeof (wchar_t));
    }
    
   /**
    * @function mcl_display_t::set_caption <src/display.h>
    * @brief Set the current window caption
    * @param {wchar_t const*}
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    set_caption (wchar_t const* caption) noexcept
    {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        mcl_set_caption (caption);
        if (mcl_control_obj.bIsReady) {
            ::SetWindowTextW ( mcl_control_obj.hwnd,
                               mcl_control_obj.window_caption );
            ::UpdateWindow   ( mcl_control_obj.hwnd );  // for vc6
        }
        return *this;
    }
    
   /**
    * @function mcl_display_t::set_caption <src/display.h>
    * @brief Set the current window caption
    * @param {char const*}
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    set_caption (char const* caption) noexcept{
        if (!caption) caption = "";
        mcl_simpletls_ns::mcl_m2w_str_t wstr (caption);
        return set_caption (wstr);
    }
    
   /**
    * @function mcl_display_t::get_caption <src/display.h>
    * @brief Get the current window caption
    * @return wchar_t const*
    */
    wchar_t const* mcl_display_t::
    get_caption () const noexcept{
        return mcl_control_obj.window_caption;
    }

   /**
    * @function mcl_display_t::set_icon <src/display.h>
    * @brief Change the system image for the display window
    * @param icon
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    set_icon (surface_t const& icon) noexcept {
        mcl_imagebuf_t* buf =
            mcl_get_surface_dataplus(const_cast<surface_t*>(&icon));
        if (!buf) return *this;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (!mcl_control_obj.bIsReady) return *this;
        
        // create a copy from surface
        HBITMAP hbmMask = ::CreateCompatibleBitmap (
            mcl_control_obj.dc, buf -> m_width, buf -> m_height);
        HBITMAP hbmColor = ::CreateCompatibleBitmap (
            mcl_control_obj.dc, buf -> m_width, buf -> m_height);
        if (!(hbmMask && hbmColor)) return *this;
        
        // copy the surface to the icon
        HDC hdcColor = ::CreateCompatibleDC (mcl_control_obj.dc);
        if (!hdcColor) return *this;
        HBITMAP hbmOldColor = 
            static_cast<HBITMAP>(::SelectObject (hdcColor, hbmColor));
        ::BitBlt (hdcColor, 0, 0, buf -> m_width, buf -> m_height,
                  buf -> m_hdc, 0, 0, SRCCOPY);
        ::SelectObject (hdcColor, hbmOldColor);
        ::DeleteDC (hdcColor);
        
        // create icon
        ICONINFO ii = { 0, 0, 0, 0, 0 };
        ii.fIcon = TRUE;
        ii.hbmColor = hbmColor;
        ii.hbmMask = hbmMask;
        HICON hIcon = ::CreateIconIndirect (&ii);
        ::DeleteObject (hbmMask);
        if (!hIcon) {
            clog4m[cll4m.Info] << L"error:  Failed to create icon from surface"
                L" [-Wdisplay-winapi-" << ::GetLastError () << L"]\n";
            return *this;
        }

        // set the icon
        if (mcl_control_obj.hicon)
            ::DestroyIcon (mcl_control_obj.hicon);
        mcl_control_obj.hicon = hIcon;

        ::SendMessage (mcl_control_obj.hwnd, WM_SETICON,
            ICON_BIG, reinterpret_cast<LPARAM>(reinterpret_cast<void*>(hIcon)));
        ::SendMessage(mcl_control_obj.hwnd, WM_SETICON,
            ICON_SMALL, reinterpret_cast<LPARAM>(reinterpret_cast<void*>(hIcon)));
        
        return *this;
    }
    
   /**
    * @function mcl_set_size <src/display.cpp>
    * @brief 
    *   Create and initialize a window, and then create a thread to
    *   process window messages.
    * @param ptrsize:  ptr to window size
    * @return void 
    */
    static void
    mcl_set_size (point2d_t* ptrsize) noexcept {
        if (!ptrsize || (!ptrsize -> x && !ptrsize -> y)) {
            // size parameter is not provided
            // the appropriate size is used based on the screen size
            if (mcl_control_obj.base_h > mcl_control_obj.base_w) {
                mcl_control_obj.dc_w =   mcl_control_obj.base_w  >> 1;
                mcl_control_obj.dc_h = ( mcl_control_obj.base_w
                                       + mcl_control_obj.base_h) >> 2;
                return ;
            }
            mcl_control_obj.dc_h =   mcl_control_obj.base_h  >> 1;
            mcl_control_obj.dc_w = ( mcl_control_obj.base_w
                                   + mcl_control_obj.base_h) >> 2;
            return ;
        }
        // if x or Y is set to 0 
        // calculated according to the screen scale
        int cxmin = ::GetSystemMetrics (SM_CXMIN);
        if (!ptrsize -> x) {
            if (mcl_control_obj.base_h > mcl_control_obj.base_w) {
                ptrsize -> x =
                    static_cast<point1d_t>(
                        static_cast<float>(ptrsize -> y) * (
                            static_cast<float>( mcl_control_obj.base_w) / 
                            static_cast<float>((mcl_control_obj.base_w  +
                                                mcl_control_obj.base_h) >> 1)
                        ) + .5f);
            }
            else {
                ptrsize -> x =
                    static_cast<point1d_t>(
                        static_cast<float>(ptrsize -> y) * (
                            static_cast<float>((mcl_control_obj.base_w +
                                                mcl_control_obj.base_h) >> 1) /
                            static_cast<float>( mcl_control_obj.base_h)
                        ) + .5f);
            }
        }
        else if (!ptrsize -> y) {
            if (mcl_control_obj.base_h > mcl_control_obj.base_w) {
                ptrsize -> y =
                    static_cast<point1d_t>(
                        static_cast<float>(ptrsize -> x) * (
                            static_cast<float>((mcl_control_obj.base_w +
                                                mcl_control_obj.base_h) >> 1) /
                            static_cast<float>( mcl_control_obj.base_w)
                        ) + .5f);
            }
            else {
                ptrsize -> y =
                    static_cast<point1d_t>(
                        static_cast<float>(ptrsize -> x) * (
                            static_cast<float>( mcl_control_obj.base_h) /
                            static_cast<float>((mcl_control_obj.base_w +
                                                mcl_control_obj.base_h) >> 1)
                        ) + .5f);
            }
        }

        if (ptrsize -> x < cxmin) ptrsize -> x = cxmin;
        if (ptrsize -> y < 1)     ptrsize -> y = 1;
        mcl_control_obj.dc_w = ptrsize -> x;
        mcl_control_obj.dc_h = ptrsize -> y;
    }

    static void
    mcl_init_window (point2d_t* ptrsize, dflags_t dpm_flags = 0) noexcept{
    // Create Window & Start Message Loop.
        if (mcl_control_obj.bAtQuitInClose)
            return ;
        
        // Initialize window parameters.
        bool bopen = clog4m.get_init () && clog4m.get_event_level ().value <= cll4m.Int.value;
        clog4m_t ml_;
        unsigned long retdpi = mcl_set_dbi_awareness();
        
        if (bopen) {
            if (retdpi) {
                ml_[cll4m.Info] << L"mcl::display::init  | window settings reset:\n"
                    << "    SetProcessDpiAwareness failed. {\"code\":\"" << retdpi << "\"}\n";
                ml_[cll4m.Int];
            } else ml_[cll4m.Int] << L"mcl::display::init  | window settings reset:\n";
            ml_[cll4m.Int] << L"  base resolution:   ";
        }

        mcl_control_obj.base_w = ::GetSystemMetrics (SM_CXSCREEN);
        mcl_control_obj.base_h = ::GetSystemMetrics (SM_CYSCREEN);
        if (!mcl_control_obj.base_w || !mcl_control_obj.base_h) {
            if (bopen) ml_ << std::endl
                << L"error:  Failed to get base resolution [-Wdisplay-winapi-"
                << ::GetLastError () <<    L"]\n, default resolution = ";
            mcl_control_obj.base_w = 1920; mcl_control_obj.base_h = 1080;
        }
        if (bopen)
            ml_ << mcl_control_obj.base_w << 'x' << mcl_control_obj.base_h
                << std::endl << L"  output resolution: ";
        mcl_set_size (ptrsize); 
        if (bopen)
            ml_ << mcl_control_obj.dc_w << 'x' << mcl_control_obj.dc_h
                << std::endl << L"  output 0: pos = (";
        mcl_control_obj.x_pos = (mcl_control_obj.base_w - mcl_control_obj.dc_w) >> 1;
        mcl_control_obj.y_pos = (mcl_control_obj.base_h - mcl_control_obj.dc_h) >> 1;
        if (bopen) ml_ << mcl_control_obj.x_pos << L", "
                       << mcl_control_obj.y_pos << L"), " << std::flush;
        
        // Process title string.
        if (!mcl_control_obj.window_caption[0])
            mcl_set_caption (nullptr);
        if (bopen)
            ml_ << L"caption = \"" << mcl_control_obj.window_caption
                << L"\"\n" << std::flush;
        
        // Set timer.
        LARGE_INTEGER tickCount;
        ::QueryPerformanceCounter (&tickCount);
        mcl_control_obj.timer = tickCount.QuadPart;
        
        // Start thread. 
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 5039)
#       endif // never throw an exception passed to extern C function
        mcl_control_obj.taskhandle = HANDLE (
            ::_beginthreadex ( nullptr, 0, mcl_simpletls_ns::bind_mf (
                    &mcl_window_info_t::threadMessageLoop, &mcl_control_obj
                ), &dpm_flags, 0, &mcl_control_obj.threaddr
        ));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
        if (!mcl_control_obj.taskhandle) {
            mcl_report_sysexception (L"Failed to create thread.");
            if (bopen) ml_.separator ();
            return ;
        }
        
        // Wait for thread getting ready.
        while (!mcl_control_obj.bIsReady) ::Sleep (0);
        
        // Check if error
        if (mcl_control_obj.bErrorCode) {
            if (bopen) ml_.separator ();
            mcl_control_obj.bErrorCode = 0;
            mcl_control_obj.timer = 0;
            ::InterlockedExchange (&mcl_control_obj.bIsReady, 0);
        }
        return ;
    }

   /**
    * @function mcl_display_t::init <src/display.h>
    * @brief 
    *   Create and initialize a window, and then create a thread
    *   to process window messages. The display module cannot do
    *   most thing until it is initialized. This is usually handled
    *   for you automatically when you call the higher level mcl::init().
    * @return mcl_display_t
    */
    mcl_display_t& mcl_display_t::
    init () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (mcl_control_obj.bIsReady) return *this;
        mcl_init_window (nullptr);
        return *this;
    }

   /**
    * @function mcl_display_t::set_mode <src/display.h>
    * @brief Initialize a window or screen for display
    * @param {point2d_t} size
    * @param {dflags_t} dpm_flags
    * @return surface_t&
    */
    surface_t& mcl_display_t::
    set_mode (point2d_t size) noexcept{
    // Initialize Window & Start Message Loop
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (mcl_control_obj.bIsReady || mcl_control_obj.bAtQuitInClose) {
        // change form size only
            if (mcl_control_obj.b_fullscreen) {
                clog4m[cll4m.Debug] << L"mcl.display.set_mode()\n"
                   L"    warning:  Not take effect.  In full screen "
                   L"mode [-display-window-togglefullscreen]\n";
                return *(mcl_control_obj.cur_surface);
            }
            bool bopen = clog4m.get_init ()
                         && clog4m.get_event_level ().value <= cll4m.Int.value;
            clog4m_t ml_; ml_[cll4m.Int];
            mcl_set_size (&size);
            
            // adjust windows rect for style
            if (bopen)
                ml_.wprintln (L"mcl.display.set_mode({%ld, %ld})",
                              mcl_control_obj.dc_w, mcl_control_obj.dc_h );
            LONG_PTR new_style   = ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
            LONG_PTR new_exstyle = ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_EXSTYLE);
            RECT wr = { mcl_control_obj.x_pos, mcl_control_obj.y_pos,
                        mcl_control_obj.x_pos + mcl_control_obj.dc_w,
                        mcl_control_obj.y_pos + mcl_control_obj.dc_h };
            ::AdjustWindowRectEx ( &wr, static_cast<DWORD>(new_style),
                                   FALSE, static_cast<DWORD>(new_exstyle) );
            ::SetWindowPos       ( mcl_control_obj.hwnd, nullptr,
                                   wr.left, wr.top, wr.right - wr.left,
                                   wr.bottom - wr.top, SWP_FRAMECHANGED );
            ::UpdateWindow       (mcl_control_obj.hwnd);  // for vc6
        } else 
            mcl_init_window (&size); 
        
        return *(mcl_control_obj.cur_surface);
    }
    surface_t& mcl_display_t::
    set_mode (point2d_t size, dflags_t dpm_flags) noexcept {
    // Initialize Window & Start Message Loop
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (mcl_control_obj.bIsReady || mcl_control_obj.bAtQuitInClose) {
        // change form style only
            bool bopen = clog4m.get_init ()
                         && clog4m.get_event_level ().value <= cll4m.Int.value;
            clog4m_t ml_; ml_[cll4m.Int];
            mcl_set_size (&size);
            
            LONG_PTR last_style =
                ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
            LONG_PTR last_exstyle;
            if (bopen)
                ml_.wprintf (L"mcl.display.set_mode({%ld, %ld}, 0x%08lx)\n"
                             L"  Setting styles... 0x%08lx -> ",
                             mcl_control_obj.dc_w, mcl_control_obj.dc_h,
                             dpm_flags, last_style );
            
            if (!mcl_control_obj.b_fullscreen) {
                // set window form style
                if (dpm_flags & dflags.NoFrame)       last_style &= ~WS_CAPTION, last_style |= WS_POPUP;
                else                                  last_style |= WS_CAPTION,  last_style &= ~WS_POPUP;
                if (dpm_flags & dflags.Resizable)     last_style |= (WS_THICKFRAME | WS_MAXIMIZEBOX);
                else                                  last_style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
                if (dpm_flags & dflags.NoMinimizeBox) last_style &= ~WS_MINIMIZEBOX;
                else                                  last_style |= WS_MINIMIZEBOX;
                
                last_exstyle = ::GetWindowLongPtrW (
                                   mcl_control_obj.hwnd, GWL_EXSTYLE );
                RECT wr = { mcl_control_obj.x_pos, mcl_control_obj.y_pos,
                            mcl_control_obj.x_pos + mcl_control_obj.dc_w,
                            mcl_control_obj.y_pos + mcl_control_obj.dc_h };
                ::AdjustWindowRectEx ( &wr, static_cast<DWORD>(last_style),
                                       FALSE, static_cast<DWORD>(last_exstyle) );
                ::SetWindowLongPtrW  ( mcl_control_obj.hwnd,
                                       GWL_STYLE, static_cast<DWORD>(last_style) );
                ::SetWindowPos       ( mcl_control_obj.hwnd, nullptr,
                                       wr.left, wr.top, wr.right - wr.left,
                                       wr.bottom - wr.top, SWP_FRAMECHANGED ); 
            }
            
            // Handle flags related to ShowWindow 
            if (dpm_flags & dflags.Hidden)
                ::ShowWindow (mcl_control_obj.hwnd, SW_HIDE);
            else if (!(last_style & WS_VISIBLE))
                ::ShowWindow (mcl_control_obj.hwnd, SW_SHOW);
            
            if (dpm_flags & dflags.Maximize)
                ::ShowWindow (mcl_control_obj.hwnd, SW_MAXIMIZE);
            if (dpm_flags & dflags.Minimize)
                ::ShowWindow (mcl_control_obj.hwnd, SW_MINIMIZE);
            
            // double buffered
            if ((dpm_flags & dflags.DoubleBuf) && !mcl_control_obj.dbuf_surface) {
                mcl_control_obj.dbuf_surface = new(std::nothrow)
                    surface_t({ mcl_control_obj.dc_w, mcl_control_obj.dc_h });
            }
            else if (!(dpm_flags & dflags.DoubleBuf) && mcl_control_obj.dbuf_surface) {
                delete mcl_control_obj.dbuf_surface;
                mcl_control_obj.dbuf_surface = nullptr;
            }
            
            ::UpdateWindow (mcl_control_obj.hwnd);  // for vc6
            last_style =
                ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
            if ((last_style & WS_VISIBLE) && !(last_style & WS_MINIMIZE))
                ::SetFocus (mcl_control_obj.hwnd);
            
            // log final style
            if (bopen) {
                ml_.wprintln (L"0x%08lx", last_style);
                if (mcl_control_obj.b_fullscreen)
                    ml_[cll4m.Debug] << L"    warning:  Not take effect."
                       L"  In full screen mode"
                       L" [-display-window-togglefullscreen]\n";
            }
        } else
            mcl_init_window (&size, dpm_flags);
            
        return *(mcl_control_obj.cur_surface);
    }
    
    MCL_NODISCARD_CXX17 static LONG_PTR&
    mcl_fullscreen_last_style () noexcept{
        static LONG_PTR mcl_fullscreen_last_style_obj{};
        return mcl_fullscreen_last_style_obj;
    }
    
    dflags_t mcl_display_t::
    get_flags () const noexcept {
    // get dpm_flags from form style
        LONG_PTR style =
            ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
        LONG_PTR last_style = mcl_control_obj.b_fullscreen ?
            mcl_fullscreen_last_style () : style;
        dflags_t flags = dflags.Hidden | dflags.NoFrame | dflags.NoMinimizeBox;
        if (     style & WS_VISIBLE)      flags &= ~dflags.Hidden;
        if (     style & WS_MINIMIZE)     flags |=  dflags.Minimize;
        if (mcl_control_obj.b_maximize)   flags |=  dflags.Maximize;
        if (last_style & WS_CAPTION)      flags &= ~dflags.NoFrame;
        if (last_style & WS_MINIMIZEBOX)  flags &= ~dflags.NoMinimizeBox;
        if (last_style & WS_THICKFRAME)   flags |=  dflags.Resizable;
        if (mcl_control_obj.dbuf_surface) flags |=  dflags.DoubleBuf;
        return flags;
    }

    surface_t& mcl_display_t::get_surface() const noexcept {
    // Return a reference to the currently set display Surface.
    // If no display mode has been set this will return None.
        return *(mcl_control_obj.dbuf_surface ?
            mcl_control_obj.dbuf_surface : mcl_control_obj.cur_surface);
    }

    static void mcl_filp_dbuf_surface () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (mcl_control_obj.dbuf_surface) {
            surface_t* t = mcl_control_obj.cur_surface;
            mcl_control_obj.cur_surface = mcl_control_obj.dbuf_surface;
            mcl_control_obj.dbuf_surface = t;
        }
    }

    mcl_display_t& mcl_display_t::
    flip () noexcept {
        // double buffered
        mcl_filp_dbuf_surface ();
        
        // flip
        RECT rc{ 0, 0, mcl_control_obj.dc_w, mcl_control_obj.dc_h };
        ::InvalidateRect (mcl_control_obj.hwnd, &rc, FALSE);
        ::UpdateWindow (mcl_control_obj.hwnd);
        return *this;
    }
    mcl_display_t& mcl_display_t::
    update (rect_t recta) noexcept {
        mcl_filp_dbuf_surface ();
        
        if (recta.w < 0) recta.x += recta.w + 1, recta.w = -recta.w;
        if (recta.h < 0) recta.y += recta.h + 1, recta.h = -recta.h;
        RECT rc{ recta.x, recta.y, recta.x + recta.w, recta.y + recta.h };
        ::InvalidateRect (mcl_control_obj.hwnd, &rc, FALSE);
        ::UpdateWindow (mcl_control_obj.hwnd);
        return *this;
    }
    mcl_display_t& mcl_display_t::
    update (std::initializer_list<rect_t>&& rects) noexcept {
        mcl_filp_dbuf_surface ();
        
        if (!rects.size ()) return *this;
        constexpr long int32_max = 2147483647L;
        RECT maxrc{ int32_max, int32_max, -int32_max - 1L, -int32_max - 1L};
        for (auto rc : rects) {
            point1d_t right = rc.x + rc.w;
            point1d_t max1 = rc.x, min1 = right;
            if (rc.x < right) min1 = rc.x, max1 = right;
            if (min1 < maxrc.left)  maxrc.left  = min1;
            if (max1 > maxrc.right) maxrc.right = max1;

            right = rc.y + rc.h;
            max1 = rc.y, min1 = right;
            if (rc.y < right) min1 = rc.y, max1 = right;
            if (min1 < maxrc.top)    maxrc.top   = min1;
            if (max1 > maxrc.bottom) maxrc.bottom = max1;
        }
        ::InvalidateRect (mcl_control_obj.hwnd, &maxrc, FALSE);
        ::UpdateWindow (mcl_control_obj.hwnd);
        return *this;
    }

   /**
    * @function mcl_display_t::hide <src/display.h>
    * @brief Put the window in the background / foreground
    * @param bHide:
    *     If b_hide is true, window will be opened in visible mode (default)
    *                    Or, window will be opened in hidden mode
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    hide (bool b_hide) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (mcl_control_obj.bIsReady) {
        // hide/show window if inited
            ::ShowWindow ( mcl_control_obj.hwnd,
                           b_hide ? SW_HIDE : SW_SHOW );
            if (!b_hide) {
                ::UpdateWindow (mcl_control_obj.hwnd);  // for vc6
                ::SetFocus (mcl_control_obj.hwnd);
            }
        } else clog4m[cll4m.Debug] <<
           L"warning:  The window has not been created"
           L" [-Wdisplay-window-uninitialized]\n";
        return *this;
    }
    
   /**
    * @function mcl_display_t::iconify <src/display.h>
    * @brief Iconify the display surface.
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    iconify () noexcept{ 
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        LONG_PTR last_style =
            ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
        if (mcl_control_obj.bIsReady) {
            if (!(last_style & WS_VISIBLE))
                ::ShowWindow (mcl_control_obj.hwnd, SW_SHOW);
            
            if (last_style & WS_MINIMIZE) {
                ::ShowWindow (mcl_control_obj.hwnd, 
                    mcl_control_obj.b_maximize ? SW_MAXIMIZE : SW_NORMAL);
                ::UpdateWindow (mcl_control_obj.hwnd); // for vc6
                ::SetFocus (mcl_control_obj.hwnd);
            } else
                ::ShowWindow (mcl_control_obj.hwnd, SW_MINIMIZE);
        
        } else clog4m[cll4m.Debug] <<
           L"warning:  The window has not been created"
           L" [-Wdisplay-window-uninitialized]\n";
        return *this;
    }
    
   /**
    * @function mcl_display_t::maximize <src/display.h>
    * @brief Maximize the display surface.
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    maximize () noexcept{ 
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        LONG_PTR last_style =
            ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
        if (mcl_control_obj.bIsReady) {
            if (!(last_style & WS_VISIBLE))
                ::ShowWindow (mcl_control_obj.hwnd, SW_SHOW);
            
            if (mcl_control_obj.b_fullscreen)
                ::ShowWindow (mcl_control_obj.hwnd, SW_MAXIMIZE);
            else if (last_style & WS_MINIMIZE || !mcl_control_obj.b_maximize)
                ::ShowWindow (mcl_control_obj.hwnd, SW_MAXIMIZE);
            else
                ::ShowWindow (mcl_control_obj.hwnd, SW_NORMAL);
                
            ::UpdateWindow (mcl_control_obj.hwnd); // for vc6
            ::SetFocus (mcl_control_obj.hwnd);

        } else clog4m[cll4m.Debug] <<
           L"warning:  The window has not been created"
           L" [-Wdisplay-window-uninitialized]\n";
        return *this;
    }

   /**
    * @function mcl_display_t::quit <src/display.h>
    * @brief Uninitialize the window. NOT end the program.
    *   This will shut down the entire display module. This means
    *   any active displays will be closed. This will also be handled
    *   automatically when the program exits.
    *   It is harmless to call this more than once, repeated calls
    *   have no effect.
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    quit () noexcept{
        if (!mcl_control_obj.bIsReady) return *this;
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (!mcl_control_obj.bIsReady) return *this; 
        ::PostMessage (mcl_control_obj.hwnd, WM_CLOSE, 2, 0);
        DWORD ret = ::WaitForSingleObject (mcl_control_obj.taskhandle, 1024);
        if (ret) clog4m[cll4m.Warn].wprintln (
           L"failed to wait for thread to end, return: 0x%08lu", ret);
        return *this;
    }
    
   /**
    * @function mcl_display_t::get_num_displays <src/display.h>
    * @brief
    *   Return the number of available displays.
    *   This is always 1 at present. 
    * @return size_t
    */
    size_t mcl_display_t::
    get_num_displays () const noexcept{ return 1; }

   /**
    * @function mcl_display_t::set_allow_screensaver <src/display.h>
    * @brief Set whether the screensaver may run.
    * @param {bool}: whether the screensaver is allowed to run after
    * @return {bool}: whether the screensaver is allowed to run before
    */
    bool mcl_display_t::
    set_allow_screensaver (bool b_allow) noexcept{
        BOOL b_allow_before = 0;
        ::SystemParametersInfoW (SPI_GETSCREENSAVEACTIVE, 0u, &b_allow_before, 0u);
        if (b_allow_before && !b_allow) {
            if (!::SystemParametersInfoW (SPI_SETSCREENSAVEACTIVE, 0u, nullptr, 0u))
                mcl_report_sysexception (L"Failed to disable the screensaver.");
        } else if (!b_allow_before && b_allow) {
            if (!::SystemParametersInfoW (SPI_SETSCREENSAVEACTIVE, TRUE, nullptr, 0u))
                mcl_report_sysexception (L"Failed to enable the screensaver.");
        }
        ::InterlockedCompareExchange (
            &mcl_control_obj.b_allow_screensaver_before,
            b_allow_before ? 1u : 0u, 2u  );
        return b_allow_before;
    }
    
   /**
    * @function mcl_display_t::get_allow_screensaver <src/display.h>
    * @brief Return whether the screensaver is allowed to run.
    * @return {bool} b_allow: whether the screensaver is allowed to run
    */
    bool mcl_display_t::
    get_allow_screensaver () const noexcept{
        BOOL b_allow = 0;
        ::SystemParametersInfoW (SPI_GETSCREENSAVEACTIVE, 0, &b_allow, 0);
        return b_allow;
    }
    
   /**
    * @function mcl_display_t::toggle_fullscreen <src/display.h>
    * @brief Switch between fullscreen and windowed displays.
    * @return mcl_display_t&
    */
    mcl_display_t& mcl_display_t::
    toggle_fullscreen () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (get_active ()) {
            static LONG_PTR last_exstyle;
            static point1d_t last_x, last_y, last_w, last_h;
            
            ::ShowWindow (mcl_control_obj.hwnd, SW_SHOW);
            LONG_PTR& last_style = mcl_fullscreen_last_style ();
            LONG_PTR  lstyle     = ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
            
            if (!mcl_control_obj.b_fullscreen) {
                // full screen
                last_x       = mcl_control_obj.x_pos;
                last_y       = mcl_control_obj.y_pos;
                last_w       = mcl_control_obj.dc_w;
                last_h       = mcl_control_obj.dc_h;
                last_style   = lstyle;
                last_exstyle = ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_EXSTYLE);
                
                LONG_PTR new_style   = (last_style & ~WS_THICKFRAME) | WS_VISIBLE | WS_POPUP;
                LONG_PTR new_exstyle = last_exstyle | WS_EX_CLIENTEDGE;
                
                RECT wr = {-1, -1, mcl_control_obj.base_w + 1, mcl_control_obj.base_h + 1};
                ::AdjustWindowRectEx (&wr, static_cast<DWORD>(new_style),
                                       FALSE, static_cast<DWORD>(new_exstyle) );
                ::SetWindowLongPtrW  (mcl_control_obj.hwnd, GWL_STYLE,   new_style);
                ::SetWindowLongPtrW  (mcl_control_obj.hwnd, GWL_EXSTYLE, new_exstyle);
                ::SetWindowPos       (mcl_control_obj.hwnd, HWND_TOPMOST, wr.left, wr.top,
                                       wr.right - wr.left, wr.bottom - wr.top, SWP_FRAMECHANGED);
                
                mcl_control_obj.b_fullscreen = true;
                
            } else if (!(lstyle & WS_MINIMIZE)) {
                // windowed displays
                if (last_style & WS_POPUP) last_style |= lstyle;
                else                      (last_style |= lstyle) &= ~WS_POPUP;
                last_exstyle |= ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_EXSTYLE);
                
                mcl_control_obj.b_fullscreen = false;
                
                RECT wr = {0, 0, last_w, last_h};
                ::AdjustWindowRectEx (&wr, static_cast<DWORD>(last_style),
                                       FALSE, static_cast<DWORD>(last_exstyle));
                ::SetWindowLongPtrW  (mcl_control_obj.hwnd,
                                       GWL_STYLE, static_cast<DWORD>(last_style));
                ::SetWindowLongPtrW  (mcl_control_obj.hwnd,
                                       GWL_EXSTYLE, static_cast<DWORD>(last_exstyle));
                ::SetWindowPos       (mcl_control_obj.hwnd, HWND_NOTOPMOST,
                                       wr.left + last_x, wr.top + last_y,
                                       wr.right - wr.left, wr.bottom - wr.top, SWP_FRAMECHANGED);   
            }
            ::UpdateWindow (mcl_control_obj.hwnd); // for vc6
        } else clog4m[cll4m.Debug] <<
            L"warning:  try to toggle fullscreen with no active display. [-Wdisplay-window-notactive]\n";
        return *this;
    }
    
   /**
    * @function mcl_display_t::set_window_alpha <src/display.h>
    * @brief Set WS_EX_LAYERED on this window.
    * @param {double} f_alpha: [0, 1]
    * @return mcl_display_t&
    */    
    mcl_display_t& mcl_display_t::
    set_window_alpha (double f_alpha) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (mcl_control_obj.bIsReady) {
            if (f_alpha > 1.f)      f_alpha = 1.f;
            else if (f_alpha < 0.f) f_alpha = 0.f;
            ::SetWindowLong (mcl_control_obj.hwnd, GWL_EXSTYLE, 
                ::GetWindowLong (mcl_control_obj.hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
            ::SetLayeredWindowAttributes (mcl_control_obj.hwnd,
                0, static_cast<BYTE>(255.f * (1.f - f_alpha)), LWA_ALPHA);
        } else clog4m[cll4m.Debug] <<
           L"warning:  The window has not been created [-Wdisplay-window-uninitialized]\n";
        return *this;
    }

   /**
    * @function mcl_display_t::get_wm_info <src/display.h>
    * @brief 
    *   Get information about the current windowing system.
    *   See also https://github.com/pygame/pygame/blob/main/test/display_test.py#L178-L211
    * @param key
    * @return void*
    */
    wmi_dict_t mcl_display_t::
        get_wm_info () const noexcept{ return wmi_dict_t{}; }
    
#   define MCL_MAKRGB(R, G, B) \
          ( (static_cast<color_t>(R) << 16) | (static_cast<color_t>(G) << 8) | (static_cast<color_t>(B)) )

    MCL_NODISCARD_CXX17 static void* mcl_get_wm_info (char const* key) noexcept{
        if (!(key && key[0] && key[1] && key[2])) return nullptr;
        switch (MCL_MAKRGB(key[0], key[1], key[2])) {
          case MCL_MAKRGB('h','d','c'):  return mcl_control_obj.dc;
          case MCL_MAKRGB('h','i','n'):  return mcl_control_obj.instance; 
          case MCL_MAKRGB('t','a','s'):  return mcl_control_obj.taskhandle;
          case MCL_MAKRGB('w','i','n'):  return mcl_control_obj.hwnd;
          default:                       break;
        }                                return nullptr;
    }

    MCL_NODISCARD_CXX17 static void* mcl_get_wm_info (wchar_t const* key) noexcept{
        if (!(key && key[0] && key[1] && key[2])) return nullptr;
        switch (MCL_MAKRGB(key[0], key[1], key[2])) {
          case MCL_MAKRGB('h','d','c'):  return mcl_control_obj.dc;
          case MCL_MAKRGB('h','i','n'):  return mcl_control_obj.instance; 
          case MCL_MAKRGB('t','a','s'):  return mcl_control_obj.taskhandle;
          case MCL_MAKRGB('w','i','n'):  return mcl_control_obj.hwnd;
          default:                       break;
        }                                return nullptr;
    }

#   undef MCL_MAKRGB
    
    wchar_t const* wmi_dict_t::get_ (size_t it) noexcept{
        switch (it) {
          case 0:  return L"hdc";
          case 1:  return L"hinstance";
          case 2:  return L"taskhandle";
          case 3:  return L"window";
          default: break;
        }          return nullptr;
    }
    
    std::string str_a (wmi_dict_t const) noexcept{  
        std::ostringstream oss;
        oss << "{'hdc': "      << mcl_control_obj.dc         << ", "
               "'hinstance': " << mcl_control_obj.instance   << ", "
               "'taskhandle': "<< mcl_control_obj.taskhandle << ", "
               "'window': "    << mcl_control_obj.hwnd       << "}";
        return oss.str ();
    }
    
    std::wstring str (wmi_dict_t const) noexcept{ 
        std::wostringstream oss;
        oss << L"{'hdc': "      << mcl_control_obj.dc         << L", "
               L"'hinstance': " << mcl_control_obj.instance   << L", "
               L"'taskhandle': "<< mcl_control_obj.taskhandle << L", "
               L"'window': "    << mcl_control_obj.hwnd       << L"}";
        return oss.str ();
    }
    
    constexpr size_t mcl_wmi_length = 4;
    size_t len (wmi_dict_t const) noexcept{ return mcl_wmi_length; }
    bool wmi_dict_t::has_key (char const* key) const noexcept
    { return static_cast<bool>(mcl_get_wm_info (key)); }
    bool wmi_dict_t::has_key (wchar_t const* key) const noexcept
    { return static_cast<bool>(mcl_get_wm_info (key)); }
    
    wmi_dict_t::map_proxy_t wmi_dict_t::
    operator[] (char const* key) const noexcept {
        return wmi_dict_t::map_proxy_t (mcl_get_wm_info (key));
    }
    wmi_dict_t::map_proxy_t wmi_dict_t::
    operator[] (wchar_t const* key) const noexcept {
        return wmi_dict_t::map_proxy_t (mcl_get_wm_info (key));
    }
    wmi_dict_t::map_proxy_t wmi_dict_t::
    get (char const* key, void* defaultv) const noexcept{
        void* ret = mcl_get_wm_info (key);
        return wmi_dict_t::map_proxy_t (ret ? ret : defaultv);
    }
    wmi_dict_t::map_proxy_t wmi_dict_t::
    get (wchar_t const* key, void* defaultv) const noexcept{
        void* ret = mcl_get_wm_info (key);
        return wmi_dict_t::map_proxy_t (ret ? ret : defaultv);
    }
    
   /**
    * @function mcl_display_t::get_window_size <src/display.h>
    * @brief Return the size of the desktop, window or screen
    * @return point2d_t 
    */
    point2d_t mcl_display_t::
    get_window_size  () const noexcept{
        return mcl_control_obj.b_fullscreen ? get_desktop_size() :
                point2d_t{ mcl_control_obj.dc_w, mcl_control_obj.dc_h };
    }
    rect_t mcl_display_t::
    get_window_rect  () const noexcept{
        return mcl_control_obj.b_fullscreen ? get_desktop_rect() :
                rect_t{ mcl_control_obj.x_pos, mcl_control_obj.y_pos,
                        mcl_control_obj.dc_w, mcl_control_obj.dc_h };
    }
    point2d_t mcl_display_t::
    get_window_pos   () const noexcept{
        return mcl_control_obj.b_fullscreen ? point2d_t{0, 0} :
                point2d_t{ mcl_control_obj.x_pos, mcl_control_obj.y_pos };
    }
    point2d_t mcl_display_t::
    get_desktop_size () const noexcept{
        return mcl_control_obj.bIsReady ?
                point2d_t{ mcl_control_obj.base_w,  mcl_control_obj.base_h } :
                point2d_t{::GetSystemMetrics (SM_CXSCREEN), ::GetSystemMetrics (SM_CYSCREEN)};
    }
    rect_t mcl_display_t::
    get_desktop_rect () const noexcept{
        return mcl_control_obj.bIsReady ?
            rect_t{ 0, 0, mcl_control_obj.base_w,  mcl_control_obj.base_h } :
            rect_t{ 0, 0, ::GetSystemMetrics (SM_CXSCREEN), ::GetSystemMetrics (SM_CYSCREEN)};
    }

   /**
    * @function mcl_display_t::get_active <src/display.h>
    * @brief 
    *    Returns True when the display is active on the screen
    *    Returns True when the display Surface is considered actively renderable
    *    on the screen and may be visible to the user.
    *  Note:  This function returning True is unrelated to whether the application
    *    has input focus.  Please see APIs related to input focus.
    * @return {bool} is_active
    */
    bool mcl_display_t::get_active() const noexcept{
        if (!mcl_control_obj.bIsReady) return false;
        LONG_PTR last_style = ::GetWindowLongPtrW (mcl_control_obj.hwnd, GWL_STYLE);
        return !(last_style & WS_MINIMIZE) && (last_style & WS_VISIBLE) && mcl_control_obj.bIsReady;
    }
    bool mcl_display_t::get_fullscreen () const noexcept{
        return mcl_control_obj.b_fullscreen && mcl_control_obj.bIsReady;
    }

   /**
    * @function mcl_display_t::get_driver <src/display.h>
    * @brief
    *   Get the name of the pygame display backend.
    *   In order to facilitate the porting of pyGame programs, many functions
    *   in this header file have the same names as pyGame functions, but they
    *   are not actually related.  This header file is based on Windows API,
    *   so only windows driver is used.
    * @return {wchar_t const*} driver_name: "windows"
    *   in pygame, drivers = [
    *      "aalib", "android", "arm", "cocoa", "dga", "directx",
    *      "directfb", "dummy", "emscripten", "fbcon", "ggi", "haiku",
    *      "khronos", "kmsdrm", "nacl", "offscreen", "pandora", "psp",
    *      "qnx", "raspberry", "svgalib", "uikit", "vgl", "vivante",
    *      "wayland", "windows", "windib", "winrt", "x11", ]
    */
    char const* mcl_display_t::
        get_driver_a () const noexcept{ return "windows"; }
    wchar_t const* mcl_display_t::
        get_driver () const noexcept{ return L"windows"; }
    
}
