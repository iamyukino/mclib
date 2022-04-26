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
            windowtext[0] = '\0';
        }
        unsigned threadMessageLoop (void* title);
        LRESULT CALLBACK wndProc          // callback
            (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
    
    public:   // callback functions
        LRESULT OnClose     (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnNCHitTest (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnSize      (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnMove      (HWND hWnd, WPARAM wParam, LPARAM lParam);

    public:   // window properties
        HWND     threadhwnd = nullptr;    // window handle
        HDC      windowhdc  = nullptr;
        HINSTANCE inst      = nullptr;
        HANDLE   taskhandle = nullptr;    // thread handle
        LONGLONG timer      = 0ll;
        unsigned threaddr   = 0u;         // thread id
        wchar_t  windowtext[_MAX_FNAME];  // caption

    public:   // positions
        point1d_t bufw  = 0;              // screen size
        point1d_t bufh  = 0;
        point1d_t realw = 0;              // window size
        point1d_t realh = 0;
        point1d_t x_pos = 0;              // window position 
        point1d_t y_pos = 0;
        
    public:   // switchs
        unsigned  b_allow_screensaver_before = 2;
        bool      b_fullscreen               = false;
        bool      b_maximize                 = false;

    public:   // state quantities
        bool bErrorCode = false;          // successful flag
        char : 8;
        unsigned bIsReady = 0ul;          // whether message loop starts
        unsigned bIsExit  = 0ul;
        char : 8; char : 8; char : 8; char : 8;
    };
    
    extern mcl_window_info_t mcl_control_obj;
    void mcl_report_sysexception (wchar_t const* what);

} // namespace

#endif // MCL_MCLCONTROL
