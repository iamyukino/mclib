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
    
    @file cpp/mcl_control.cpp
    This is a C++11 implementation file for controling display
    windows.
*/


#include "../src/display.h"
#include "../src/clog4m.h"
#include "mcl_control.h"

#include <cerrno>     // for errno
#include <cstring>    // for strerrno

namespace
mcl {
    
    mcl_window_info_t mcl_control_obj;
    
   /**
    * @function mcl_report_sysexception <cpp/mcl_control.h>
    * @brief
    *    Output details of exceptions that occurred in the
    *    windows API function to the log.
    * @param what: brief summary of anomalies
    * @return void
    */
    void
    mcl_report_sysexception (wchar_t const* what) {
        LPWSTR lpMsgBuf = nullptr;
        DWORD eif = ::GetLastError ();
        ::FormatMessageW (FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, eif, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
            LPWSTR (&lpMsgBuf), 0, nullptr );
        clog4m[logWarn] << L"\n"
           L"SYSTEM EXCEPTION: " << what << L"\n"
           L"- type:  "    "mcl::win32api_error\n"
           L"- value: " << eif      << L"\n"
           L"- msg:   " << lpMsgBuf;
        ::LocalFree (lpMsgBuf);
    }
    
   /**
    * @function mcl_window_info_t::OnClose <cpp/mcl_control.cpp>
    * @brief
    *    Callback function for window closing.
    * @param wParam:
    *    constant values related to messages,
    *    or possibly a handle to a window or control
    * @param lParam:
    *    pointer to specific additional information
    * @return NO_USE
    */
    LRESULT mcl_window_info_t::
    OnClose (HWND , WPARAM wParam, LPARAM ) {
    // WM_CLOSE
        if (::InterlockedCompareExchange (&bIsReady, 0, 1) == 0)
            return 0u;
        bool bopen = clog4m.is_init () && clog4m.get_event_level () <= logInt;
        clog4m_t ml_; ml_[logInt];
        if (bopen) {
            ml_ << L"\n"
            L"==== Shutting down ====\n"
            L"mcl::quit" << std::endl << 
            L"mcl::display::uninit  | Message loop terminated. "
             << std::flush <<  L"{\"reason\":\"";
        }
        switch (wParam) {
            case 0:  if (bopen) ml_ << L"user";    bIsExit = 2ul; break;
            case 2:  if (bopen) ml_ << L"program"; bIsExit = 3ul; break;
            default: if (bopen) ml_ << L"unknown";   bIsExit = 4ul; break;
        }
        if (bopen) ml_ << "\"}\n" << L"Reseting..." << std::endl;
        ::ShowWindow (threadhwnd, SW_HIDE);
        // if (bopen) ml_ << L"  Releasing device contexts..." << std::endl;
        if (!::ReleaseDC (threadhwnd, windowhdc)) {
            mcl_report_sysexception (L"Failed to release device contexts.");
            bErrorCode = true;
        }
        // if (bopen) ml_ << L"  Destroying window..." << std::endl;
        if (!::DestroyWindow (threadhwnd)) {
            mcl_report_sysexception (L"Failed to destroy window.");
            bErrorCode = true;
        }
        // if (bopen) ml_ << L"  Unregistering class..." << std::endl;
        if (!::UnregisterClassW (L"mclibTclass", inst)) {
            mcl_report_sysexception (L"Failed to unregister class.");
            bErrorCode = true;
        }
        // if (bopen) ml_ << L"  Reseting..." << std::endl;
        
        // window properties           // positions
        threaddr      = 0;             bufw    = bufh      = 0;
        threadhwnd    = nullptr;       realw   = realh     = 0;
        windowhdc     = nullptr;       x_pos   = y_pos     = 0;
        inst          = nullptr;
        taskhandle    = nullptr;       // switchs
        windowtext[0] = '\0';          b_fullscreen  = false;
        time          = 0;             b_maximize    = false;
        
        // switchs
        if (b_allow_screensaver_before != 2) {
            display.set_allow_screensaver
                (static_cast<bool>(b_allow_screensaver_before));
            b_allow_screensaver_before = 2;
        }
        
        if (bopen) ml_ << L"All window data cleared\n";
        return 0ul;
    }
    
    LRESULT mcl_window_info_t::
    OnNCHitTest (HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // WM_NCHITTEST
        LRESULT hit = ::DefWindowProcW (hWnd, WM_NCHITTEST, wParam, lParam);
        if (b_fullscreen ||
            (::GetWindowLongPtrW (hWnd, GWL_STYLE) & WS_CAPTION)
        ) return hit;
        if (hit == HTCLIENT) return HTCAPTION;
        return hit;
    }
    
    LRESULT mcl_window_info_t::
    OnSize (HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // WM_SIZE
        if (wParam == SIZE_MINIMIZED)
            return 0;
        
        // new_width
        int cxmin = ::GetSystemMetrics (SM_CXMIN);
        this -> realw = GET_X_LPARAM(lParam);
        if(this -> realw < cxmin)
            this -> realw = cxmin;
        
        // new_height
        this -> realh = GET_Y_LPARAM(lParam);
        if(this -> realh < 1)
            this -> realh = 1;
            
        // maximize
        if (wParam == SIZE_MAXIMIZED) {
            this -> b_maximize = true;
            if (b_fullscreen) {
                LONG_PTR llstyle = ::GetWindowLongPtrW (hWnd, GWL_STYLE);
                LONG_PTR exstyle = ::GetWindowLongPtrW (hWnd, GWL_EXSTYLE);
                llstyle = (llstyle & ~WS_THICKFRAME) | WS_VISIBLE | WS_POPUP;
                exstyle = exstyle | WS_EX_CLIENTEDGE;
                
                RECT wr = {-1, -1, bufw + 1, bufh + 1};
                ::AdjustWindowRectEx (&wr, static_cast<DWORD>(llstyle),
                                        FALSE, static_cast<DWORD>(exstyle));
                
                b_fullscreen = false;
                ::SetWindowLongPtrW  (hWnd, GWL_STYLE, static_cast<DWORD>(llstyle));
                ::SetWindowLongPtrW  (hWnd, GWL_EXSTYLE, static_cast<DWORD>(exstyle));
                ::SetWindowPos       (hWnd, HWND_TOPMOST, wr.left, wr.top,
                                       wr.right - wr.left, wr.bottom - wr.top, SWP_FRAMECHANGED);
                b_fullscreen = true;
            }
        } else
            this -> b_maximize = false;
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnMove (HWND hWnd, WPARAM , LPARAM lParam) {
    // WM_MOVE
        LONG_PTR lstyle = ::GetWindowLongPtrW (hWnd, GWL_STYLE);
        if (!(lstyle & WS_MINIMIZE)) {
            this -> x_pos = GET_X_LPARAM (lParam);
            this -> y_pos = GET_Y_LPARAM (lParam);
        }
        return 0;
    }
    
   /**
    * @function mcl_window_info_t::wndProc <cpp/mcl_control.cpp>
    * @brief Callback function of window procedure.
    * @param {HWND} hWnd: window handle
    * @param {UINT} uMessage: message constant identifier
    * @param {WPARAM} wParam: constant values related to messages,
    *                           or possibly a handle to a window or control
    * @param {LPARAM} lParam: pointer to specific additional information
    * @return {UINT} UINT_AO: {UINT_AO} = {0}
    */
    LRESULT CALLBACK mcl_window_info_t::
    wndProc (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    // Windows callback: This is where we will send messages to.
        switch (uMessage) {
            case WM_CLOSE:      return OnClose       (hWnd, wParam, lParam);    break;  
            case WM_NCHITTEST:  return OnNCHitTest   (hWnd, wParam, lParam);    break;
            case WM_SIZE:       return OnSize        (hWnd, wParam, lParam);    break;
            case WM_MOVE:       return OnMove        (hWnd, wParam, lParam);    break;
            case WM_DESTROY:           ::PostQuitMessage (0);                   break; 
            default:            return ::DefWindowProcW (hWnd, uMessage, wParam, lParam);
        }
        return 0;
    }
    
   /**
    * @function mcl_window_info_t::threadMessageLoop <cpp/mcl_control.cpp>
    * @brief Thread to process window messages independently.
    * @param {wchar_t*} title: the caption of the window
    * @return {UINT} UINT_AO: {UINT_AO} = {0}
    */
    unsigned mcl_window_info_t::
    threadMessageLoop (void* _parm_flags) {
    // Message loop
        { // <- sub for exit
        bool bopen = clog4m.is_init () && clog4m.get_event_level () <= logInt;
        clog4m_t ml_; ml_[logInt];
        
        WNDCLASSEXW wc; // A properties struct of our window
        ::memset (&wc, 0, sizeof (wc));
                // Zero out the struct and set the stuff we want to modify.
        wc.cbSize        = sizeof (WNDCLASSEXW);
        wc.hInstance     = nullptr;
        wc.lpfnWndProc = mcl_simpletls_ns::bind_mf(&mcl_window_info_t::wndProc, this);
        wc.hCursor       = ::LoadCursor (nullptr, IDC_ARROW);
        wc.hbrBackground = HBRUSH (COLOR_BACKGROUND + 1);
        wc.lpszClassName = L"mclibTclass";
        wc.hIcon         = ::LoadIcon (nullptr, IDI_APPLICATION);
                // Load a standard icon
        wc.hIconSm       = ::LoadIcon (nullptr, IDI_APPLICATION);
                // use the name "A" to use the project icon
        
#        define MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_()   \
        ml_ << L"Reseting..." << std::endl;                         \
        threaddr      = 0;                                          \
        threadhwnd    = nullptr;                                    \
        windowhdc     = nullptr;                                    \
        bufw  = bufh  = 0;                                          \
        realw = realh = 0;                                          \
        x_pos = y_pos = 0;                                          \
        ml_[logWarn]                                                \
            << L"threadMessageLoop initialization terminated.\n";   \
        bErrorCode = true;                                          \
        ::InterlockedExchange (&bIsReady, 1);
        
#        define MCL_UNREGISTERING_WINDOW_()                         \
        if (!::UnregisterClassW (L"mclibTclass", inst))             \
            mcl_report_sysexception (L"Failed to unregister class.")
        
#        define MCL_DESTROY_WINDOW_()                               \
        ml_ << L"Destroying window..." << std::endl;                \
        if (!::DestroyWindow (threadhwnd))                          \
            mcl_report_sysexception (L"Failed to destroy window.")
            
#        define MCL_RELEASE_HDC_()                                  \
        if (!::ReleaseDC (threadhwnd, windowhdc))                   \
            mcl_report_sysexception (L"Failed to release "          \
                     L"device contexts.");
        
        // if (bopen) ml_ << L"  Registering class..." << std::endl;
        if (!::RegisterClassExW (&wc)) {
            mcl_report_sysexception (L"Failed to register class.");
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        
        flag32_t flags  = _parm_flags ? *static_cast<flag32_t*>(_parm_flags) : 0;
        LONG_PTR fstyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW;
        bool     fvisi  = true;
        // if (bopen) ml_.println ("  Setting styles... flags: 0x%08lx", flags);
        if (flags & dpmNoFrame)       fstyle &= ~WS_CAPTION, fstyle |= WS_POPUP;
        if (!(flags & dpmMovable))    fstyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
        if (flags & dpmNoMinimizeBox) fstyle &= ~WS_MINIMIZEBOX;
        if (flags & dpmHidden)        fstyle &= ~WS_VISIBLE, fvisi = false;
        if (flags & dpmMinimize)      fstyle |= WS_MINIMIZE, fvisi = false;
        if (flags & dpmMaximize)      fstyle |= WS_MAXIMIZE;
        
        // Adjusting window rect
        RECT wr = {0, 0, realw, realh};
        if (!::AdjustWindowRectEx (&wr,
                static_cast<DWORD>(fstyle), FALSE, WS_EX_CLIENTEDGE)) {
            mcl_report_sysexception (L"Failed to adjust window rect.");
            MCL_UNREGISTERING_WINDOW_ ();
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        
        if (bopen) ml_.wprintln (L"Creating window... {\"flags\":\"0x%08lx\"}", flags).flush ();
        threadhwnd = ::CreateWindowExW(
            WS_EX_CLIENTEDGE,
            LPCWSTR(wc.lpszClassName),
            LPCWSTR(windowtext),
            static_cast<DWORD>(fstyle),
            wr.left + x_pos, // CW_USEDEFAULT
            wr.top + y_pos, // CW_USEDEFAULT
            wr.right - wr.left, wr.bottom - wr.top,
            nullptr, nullptr, nullptr, nullptr
        );
        if (!threadhwnd) {
            mcl_report_sysexception (L"Failed to create window.");
            MCL_UNREGISTERING_WINDOW_ ();
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        
        // if (bopen) ml_ << L"  Loading device contexts..." << std::endl;
        windowhdc = ::GetDC(threadhwnd);
        if (!windowhdc) {
            mcl_report_sysexception (L"Failed to get device contexts.");
            MCL_DESTROY_WINDOW_ ();
            MCL_UNREGISTERING_WINDOW_ ();
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        inst = ::GetModuleHandleW (nullptr);
        if (b_allow_screensaver_before == 2)
            display.set_allow_screensaver (false);
        
        // for vc6
        if (fvisi) {
            if (!b_maximize)
                ::ShowWindow (threadhwnd, SW_SHOWDEFAULT); 
            ::UpdateWindow (threadhwnd);
            ::SetFocus (mcl_control_obj.threadhwnd);
        }
    
#        undef MCL_RELEASE_HDC_
#        undef MCL_DESTROY_WINDOW_
#        undef MCL_UNREGISTERING_WINDOW_
#        undef MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_
        
        if (bopen)
            ml_.putws (L"\n== Startup complete ==");
        
        } // end sub for exit
        MSG msg;
        ::InterlockedExchange (&bIsReady, 1);
        
        while (::GetMessage (&msg, nullptr, 0, 0) > 0) {
            ::TranslateMessage (&msg);
            ::DispatchMessage (&msg);
        }
        
        if (::InterlockedExchange (&bIsExit, 0) == 2ul)
            ::exit (0);
        return 0;
    }
    
}
