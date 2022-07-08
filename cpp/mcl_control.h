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
    
    @file cpp/mcl_control.h
    This is a C++11 inside header for controling display windows.
*/


#ifndef MCL_MCLCONTROL
# define MCL_MCLCONTROL

# include "mcl_base.h"

namespace
mcl {

    class
    mcl_window_info_t {
    public:
        mcl_window_info_t () {
            window_caption[0] = '\0';
        }
        unsigned threadMessageLoop (void* title);
        LRESULT CALLBACK wndProc          // callback
            (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
    
    public:   // callback functions
        LRESULT OnClose     (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnNCHitTest (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnSize      (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnMove      (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnPaint     (HWND hWnd, WPARAM wParam, LPARAM lParam);

    public:   // window properties
        HDC       dc        = nullptr;
        HINSTANCE instance  = nullptr;
        HWND      hwnd      = nullptr;
        HANDLE    taskhandle = nullptr;    // thread handle
        HICON     window_hicon = nullptr;
        LONGLONG  timer      = 0ll;
        unsigned  threaddr   = 0u;         // thread id
        wchar_t   window_caption[_MAX_FNAME];

    public:   // positions
        point1d_t base_w  = 0;             // screen size
        point1d_t base_h  = 0;
        point1d_t dc_w = 0;                // window size
        point1d_t dc_h = 0;
        point1d_t x_pos = 0;               // window position 
        point1d_t y_pos = 0;
        
    public:   // switchs
        unsigned  b_allow_screensaver_before = 2;
        bool      b_fullscreen               = false;
        bool      b_maximize                 = false;

    public:   // state quantities
        bool      bErrorCode = false;      // successful flag
        char : 8;
        unsigned  bIsReady = 0ul;          // whether message loop starts
        unsigned  bIsExit  = 0ul;
        char : 8; char : 8; char : 8; char : 8;
    
    public:   // surface
        surface_t* cur_surface = nullptr;
    };
    
    extern mcl_window_info_t mcl_control_obj;
    unsigned long mcl_set_dbi_awareness (bool awareness = false) noexcept;
    void mcl_report_sysexception (wchar_t const* what);




   /**
    * @class mcl_imagebuf_t <src/surface.cpp>
    * @brief The buffer for surface_t.
    */
    class mcl_imagebuf_t {
    public:
        explicit mcl_imagebuf_t () noexcept{}
        explicit mcl_imagebuf_t (point1d_t width, point1d_t height) noexcept;
                ~mcl_imagebuf_t () noexcept;
        bool     init           () noexcept;
        void     uninit         () noexcept;
        void     cleardevice    (color_t c) noexcept;

    public:
        color_t*  m_pbuffer  = nullptr;
        HDC       m_hdc      = nullptr;
        HBITMAP   m_hbmp     = nullptr;
        point1d_t m_width    = 0;
        point1d_t m_height   = 0;

    public:
        typename mcl_simpletls_ns::mcl_spinlock_t::lock_t m_nrtlock = 0ul;
        unsigned m_nrt_count = 0u;
    };

    inline mcl_imagebuf_t* mcl_get_surface_dataplus (surface_t* s) {
        return *reinterpret_cast<mcl_imagebuf_t**>(s);
    }

    inline char* mcl_get_surface_data (surface_t* s) {
        return reinterpret_cast<char*>(reinterpret_cast<mcl_imagebuf_t**>(s) + 1);
    }

} // namespace

#endif // MCL_MCLCONTROL
