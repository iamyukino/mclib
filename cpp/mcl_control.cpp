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

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/mclfwd.h"
#include "../src/display.h"
#include "../src/surface.h"
#include "../src/clog4m.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

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
        clog4m[cll4m.Fatal] << L"\n"
           L"SYSTEM EXCEPTION: " << what << L"\n"
           L"- type:  "    "mcl::win32api_error\n"
           L"- value: " << eif      << L"\n"
           L"- msg:   " << lpMsgBuf;
        ::LocalFree (lpMsgBuf);
    }

    unsigned long
    mcl_set_dbi_awareness (bool awareness) noexcept {
    // Get windows system version
        HMODULE hModShcoredll = ::LoadLibraryW(L"Shcore.dll");
        bool    ret = false;

        typedef enum tagPROCESS_DPI_AWARENESS {
            PROCESS_DPI_UNAWARE = 0,
            PROCESS_SYSTEM_DPI_AWARE = 1,
            PROCESS_PER_MONITOR_DPI_AWARE = 2
        } PROCESS_DPI_AWARENESS;
        
        if (hModShcoredll) {
            using pfSETPROCESSDPIAWARENESS =
                HRESULT (*) (PROCESS_DPI_AWARENESS);
            pfSETPROCESSDPIAWARENESS pfSetProcessDpiAwareness;
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 4191)
#       endif // C4191: 'Function cast' : unsafe conversion.
            pfSetProcessDpiAwareness =
                reinterpret_cast<pfSETPROCESSDPIAWARENESS>(
                    ::GetProcAddress (hModShcoredll, "SetProcessDpiAwareness"));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif

#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 26812)
#       endif
            if (pfSetProcessDpiAwareness)
                pfSetProcessDpiAwareness (awareness ? PROCESS_DPI_UNAWARE : PROCESS_PER_MONITOR_DPI_AWARE);
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
            ::FreeLibrary (hModShcoredll);
        }
        return ret ? 0ul : ::GetLastError ();
    } 
    
    extern void mcl_do_atquit ();

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
    OnClose (HWND , WPARAM wParam, LPARAM lParam) {
    // WM_CLOSE
        // if needed, push to queue
        if (bCtrlMsgLoop && (0 == wParam)) {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.WindowClose;
            ev.window.wParam = wParam;
            ev.window.lParam = lParam;
            event.post (ev);

            ev.type = event.Quit;
            event.post (ev);
            return 1;
        }

        // exit
        if (::InterlockedCompareExchange (&bIsReady, 0, 1) == 0)
            return 0u;

        bAtQuitInClose = true;
        mcl_do_atquit ();
        bAtQuitInClose = false;
        
        bool bopen = clog4m.get_init () && clog4m.get_event_level ().value <= cll4m.Int.value;
        clog4m_t ml_;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (bopen) {
            ml_[cll4m.Int] << L"\n"
            L"==== Shutting down ====\n"
            L"mcl::quit" << std::endl << 
            L"mcl::display::uninit  | Message loop terminated "
             << std::flush <<  L"{\"reason\":\"";
        }
        switch (wParam) {
            case 0:  if (bopen) ml_ << L"user";    bIsExit = 2ul; break;
            case 2:  if (bopen) ml_ << L"program"; bIsExit = 3ul; break;
            default: if (bopen) ml_ << L"unknown"; bIsExit = 4ul; break;
        }
        if (bopen) ml_ << "\"}\n" << L"Reseting..." << std::endl;
        ::ShowWindow (hwnd, SW_HIDE);
        // if (bopen) ml_ << L"  Destroying window..." << std::endl;
        if (!::DestroyWindow (hwnd)) {
            mcl_report_sysexception (L"Failed to destroy window.");
            bErrorCode = true;
        }
        // if (bopen) ml_ << L"  Unregistering class..." << std::endl;
        if (!::UnregisterClassW (L"mclibTclass", instance)) {
            mcl_report_sysexception (L"Failed to unregister class.");
            bErrorCode = true;
        }
        // if (bopen) ml_ << L"  Reseting..." << std::endl;
        if (dbuf_surface) {
            delete dbuf_surface;
            dbuf_surface = nullptr;
        }
        if (cur_surface) {
            delete cur_surface;
            cur_surface = nullptr;
        }
        if (hicon) {
            ::DestroyIcon (hicon);
            hicon = nullptr;
        }

        // window properties           // positions
        threaddr      = 0;             base_w  = base_h   = 0;
        hwnd          = nullptr;       dc_w    = dc_h     = 0;
        instance      = nullptr;       x_pos   = y_pos    = 0;
        taskhandle    = nullptr;
        window_caption[0] = '\0';      // switchs
                                       b_fullscreen  = false;
        timer         = 0;             b_maximize    = false;

        // events
        if (hWndMouseGrabed) {
            ::UnhookWindowsHookEx (hWndMouseGrabed);
            hWndMouseGrabed = nullptr;
        }
        if (hWndKeyGrabed) {
            ::UnhookWindowsHookEx (hWndKeyGrabed);
            hWndKeyGrabed = nullptr;
        }
        bCtrlMsgLoop = false;
        bMouseInClient = false;
        
        // switchs
        if (b_allow_screensaver_before != 2) {
            display.set_allow_screensaver
                (static_cast<bool>(b_allow_screensaver_before));
            b_allow_screensaver_before = 2;
        }
        mcl_set_dbi_awareness (true);
        
        if (bopen) ml_ << std::endl;
        return 0ul;
    }
    
    LRESULT mcl_window_info_t::
    OnNCHitTest (HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // WM_NCHITTEST
        LRESULT hit = ::DefWindowProcW(hWnd, WM_NCHITTEST, wParam, lParam);
/*
        if (b_fullscreen ||
            (::GetWindowLongPtrW (hWnd, GWL_STYLE) & WS_CAPTION)
        ) return hit;
        if (hit == HTCLIENT) return HTCAPTION;
*/
        return hit;
    }
    
    LRESULT mcl_window_info_t::
    OnSize (HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // WM_SIZE
        if (wParam == SIZE_MINIMIZED)
            return 0;
        
        event_t ev{ 0, {{0, 0}} };
        ev.window.wParam = wParam;
        ev.window.lParam = lParam;

        // new_width
        int cxmin = ::GetSystemMetrics (SM_CXMIN);
        this -> dc_w = GET_X_LPARAM(lParam);
        if(this -> dc_w < cxmin)
            this -> dc_w = cxmin;
        
        // new_height
        this -> dc_h = GET_Y_LPARAM(lParam);
        if(this -> dc_h < 1)
            this -> dc_h = 1;
        
        // maximize
        if (wParam == SIZE_MAXIMIZED) {
            this -> b_maximize = true;
            if (b_fullscreen) {
                LONG_PTR llstyle = ::GetWindowLongPtrW (hWnd, GWL_STYLE);
                LONG_PTR exstyle = ::GetWindowLongPtrW (hWnd, GWL_EXSTYLE);
                llstyle = (llstyle & ~WS_THICKFRAME) | WS_VISIBLE | WS_POPUP;
                exstyle = exstyle | WS_EX_CLIENTEDGE;
                
                RECT wr = {-1, -1, base_w + 1, base_h + 1};
                ::AdjustWindowRectEx (&wr, static_cast<DWORD>(llstyle),
                                        FALSE, static_cast<DWORD>(exstyle));
                
                b_fullscreen = false;
                ::SetWindowLongPtrW  (hWnd, GWL_STYLE, static_cast<DWORD>(llstyle));
                ::SetWindowLongPtrW  (hWnd, GWL_EXSTYLE, static_cast<DWORD>(exstyle));
                ::SetWindowPos       (hWnd, HWND_TOPMOST, wr.left, wr.top,
                                       wr.right - wr.left, wr.bottom - wr.top, SWP_FRAMECHANGED);
                b_fullscreen = true;
            }
            ev.type = event.WindowMaximized;
            event.post (ev);
        }
        else if (this -> b_maximize) {
            this -> b_maximize = false;
            ev.type = event.WindowRestored;
            event.post (ev);
        }

        // resize
        if (dbuf_surface)
            dbuf_surface -> resize ({this -> dc_w, this -> dc_h}, false);
        cur_surface -> resize ({this -> dc_w, this -> dc_h}, false);
        
        ev.type = event.WindowResized;
        event.post (ev);

        return 0;
    }

    LRESULT mcl_window_info_t::
    OnMove (HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // WM_MOVE
        LONG_PTR lstyle = ::GetWindowLongPtrW (hWnd, GWL_STYLE);
        if (!(lstyle & WS_MINIMIZE)) {
            this -> x_pos = GET_X_LPARAM (lParam);
            this -> y_pos = GET_Y_LPARAM (lParam);

            event_t ev{ 0, {{0, 0}} };
            ev.type = event.WindowMoved;
            ev.window.wParam = wParam;
            ev.window.lParam = lParam;
            event.post (ev);
        }
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnPaint (HWND hWnd, WPARAM , LPARAM ) {
        PAINTSTRUCT ps;
        RECT rect{0, 0, 0, 0};
        ::GetUpdateRect (hWnd, &rect, FALSE);
        HDC hdc = ::BeginPaint (hWnd, &ps);

        mcl_imagebuf_t* buf = mcl_get_surface_dataplus (mcl_control_obj.cur_surface);
        if (buf) {
            mcl_simpletls_ns::mcl_spinlock_t lock (buf -> m_nrtlock);
            ::BitBlt (hdc, rect.left, rect.top, rect.right, rect.bottom,
                buf -> m_hdc, rect.left, rect.top, SRCCOPY);
        }
        
        ::EndPaint (hWnd, &ps);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnActivate (HWND, WPARAM wParam, LPARAM lParam) {
        WORD fActive    = LOWORD(wParam);
        BOOL fMinimized = static_cast<BOOL>(HIWORD(wParam));

        // post Window Event
        event_t ev{ 0, {{0, 0}} };
        if (fActive == WA_INACTIVE) ev.type = event.WindowFocusLost;
        else                        ev.type = event.WindowFocusGained;
        ev.window.wParam = wParam;
        ev.window.lParam = lParam;
        event.post (ev);

        // un-minimized
        if (fMinimized) {
            if (fActive != WA_INACTIVE) {
                ev.type = event.WindowRestored;
                event.post(ev);
                return 0;
            }
            ev.type = event.WindowMinimized;
            event.post (ev);
        }

        // post Active Event
        ev.type = event.ActiveEvent;
        if (LOWORD(wParam) == WA_INACTIVE) ev.active.gain = 0;
        else ev.active.gain = 1;

        constexpr int MouseFocus = 1, InputFocus = 2, Active = 4;
        if (!fMinimized)                       ev.active.state = Active;
        if (ev.active.state && ev.active.gain) ev.active.state |= InputFocus;
        if (LOWORD(wParam) == WA_CLICKACTIVE)  ev.active.state |= MouseFocus;

        event.post (ev);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnShowWindow (HWND, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        if (wParam) ev.type = event.WindowShown;
        else        ev.type = event.WindowHidden;
        ev.window.wParam = wParam;
        ev.window.lParam = lParam;
        event.post (ev);
        return 0;
    }
    
    LRESULT mcl_window_info_t::
    OnMouseMove (HWND, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseMotion;
        ev.mouse.pos.x = GET_X_LPARAM (lParam);
        ev.mouse.pos.y = GET_Y_LPARAM (lParam);
        ev.mouse.buttons = static_cast<char>(GET_KEYSTATE_WPARAM (wParam));
        *(&ev.mouse.buttons + 1) = ev.mouse.buttons;
        event.post (ev);

        if (!bMouseInClient) {
            ev.type = event.WindowEnter;
            ev.window.wParam = wParam;
            ev.window.lParam = lParam;
            event.post (ev);

            bMouseInClient = true;
            TRACKMOUSEEVENT tme{0, 0, 0, 0};
            tme.cbSize = sizeof(TRACKMOUSEEVENT);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hwnd;
            tme.dwHoverTime = HOVER_DEFAULT;
            ::TrackMouseEvent (&tme);
        }
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnMouseLeave (HWND, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.WindowLeave;
        ev.window.wParam = wParam;
        ev.window.lParam = lParam;
        event.post (ev);

        bMouseInClient = false;
        TRACKMOUSEEVENT tme{0, 0, 0, 0};
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_CANCEL;
        tme.hwndTrack = hwnd;
        tme.dwHoverTime = HOVER_DEFAULT;
        ::TrackMouseEvent (&tme);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnMouseWheel (char type, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseWheel;
        ev.mouse.del = GET_WHEEL_DELTA_WPARAM (wParam);
        ev.mouse.pos.x = GET_X_LPARAM (lParam);
        ev.mouse.pos.y = GET_Y_LPARAM (lParam);
        ev.mouse.buttons = static_cast<char>(GET_KEYSTATE_WPARAM (wParam) | (type ? 0x100 : 0x80));
        *(&ev.mouse.buttons + 1) = ev.mouse.buttons;
        event.post (ev);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnButtonDown (char type, WPARAM wParam, LPARAM lParam) {
        ::SetCapture (hwnd);
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseButtonDown;
        ev.mouse.button = type;
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
            ++ ev.mouse.button;
        ev.mouse.pos.x = GET_X_LPARAM (lParam);
        ev.mouse.pos.y = GET_Y_LPARAM (lParam);
        ev.mouse.buttons = static_cast<char>(GET_KEYSTATE_WPARAM (wParam));
        *(&ev.mouse.buttons + 1) = ev.mouse.buttons;
        event.post (ev);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnButtonUp (char type, WPARAM wParam, LPARAM lParam) {
        ::ReleaseCapture ();
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseButtonUp;
        ev.mouse.button = type;
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
            ++ ev.mouse.button;
        ev.mouse.pos.x = GET_X_LPARAM (lParam);
        ev.mouse.pos.y = GET_Y_LPARAM (lParam);
        ev.mouse.buttons = static_cast<char>(GET_KEYSTATE_WPARAM (wParam));
        *(&ev.mouse.buttons + 1) = ev.mouse.buttons;
        event.post (ev);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnKeyDown (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
        if (!(lParam >> 30 & 1)) {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.KeyDown;
            ev.key.key = static_cast<char>(wParam);
            // get scancode
            ev.key.scancode = MapVirtualKeyW(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);
            // get unicode
            std::vector<BYTE> keys(256, 0);
            wchar_t buffer[2];
            if (::ToUnicode(static_cast<UINT>(wParam), ev.key.scancode, keys.data(), buffer, 1, 0))
                ev.key.unicode = buffer[0];
            // caps unicode
            if (ev.key.unicode >= 'a' && ev.key.unicode <= 'z')
                if ((::GetKeyState (VK_CAPITAL) & 1) ^ ((::GetKeyState (VK_SHIFT) >> 8) & 1))
                    ev.key.unicode += static_cast<wchar_t>('A' - 'a');
            event.post (ev);
        }
        if (uMessage == WM_SYSKEYDOWN)
            return ::DefWindowProc (hWnd, uMessage, wParam, lParam);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnKeyUp (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
        {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.KeyUp;
            ev.key.key = static_cast<char>(wParam);
            // get scancode
            ev.key.scancode = MapVirtualKeyW(static_cast<UINT>(wParam), MAPVK_VK_TO_VSC);
            // get unicode
            std::vector<BYTE> keys(256, 0);
            wchar_t buffer[2];
            if (::ToUnicode(static_cast<UINT>(wParam), ev.key.scancode, keys.data(), buffer, 1, 0))
                ev.key.unicode = buffer[0];
            // caps unicode
            if (ev.key.unicode >= 'a' && ev.key.unicode <= 'z')
                if ((::GetKeyState (VK_CAPITAL) & 1) ^ ((::GetKeyState (VK_SHIFT) >> 8) & 1))
                    ev.key.unicode += static_cast<wchar_t>('A' - 'a');
            event.post (ev);
        }
        if (uMessage == WM_SYSKEYUP)
            return ::DefWindowProc (hWnd, uMessage, wParam, lParam);
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
    * @return {UINT}
    */
    LRESULT CALLBACK mcl_window_info_t::
    wndProc (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
    // Windows callback: This is where we will send messages to.
        switch (uMessage) {
            case WM_CLOSE:       return OnClose       (hWnd, wParam, lParam);    break;  
            case WM_NCHITTEST:   return OnNCHitTest   (hWnd, wParam, lParam);    break;
            case WM_SIZE:        return OnSize        (hWnd, wParam, lParam);    break;
            case WM_MOVE:        return OnMove        (hWnd, wParam, lParam);    break;
            case WM_PAINT:       return OnPaint       (hWnd, wParam, lParam);    break;
            case WM_ACTIVATE:    return OnActivate    (hWnd, wParam, lParam);    break;
            case WM_SHOWWINDOW:  return OnShowWindow  (hWnd, wParam, lParam);    break;
            case WM_LBUTTONDOWN: return OnButtonDown  (1, wParam, lParam);       break;
            case WM_MBUTTONDOWN: return OnButtonDown  (2, wParam, lParam);       break;
            case WM_RBUTTONDOWN: return OnButtonDown  (3, wParam, lParam);       break;
            case WM_XBUTTONDOWN: return OnButtonDown  (4, wParam, lParam);       break;
            case WM_LBUTTONUP:   return OnButtonUp    (1, wParam, lParam);       break;
            case WM_MBUTTONUP:   return OnButtonUp    (2, wParam, lParam);       break;
            case WM_RBUTTONUP:   return OnButtonUp    (3, wParam, lParam);       break;
            case WM_XBUTTONUP:   return OnButtonUp    (4, wParam, lParam);       break;
            case WM_MOUSEMOVE:   return OnMouseMove   (hWnd, wParam, lParam);    break;
            case WM_MOUSELEAVE:  return OnMouseLeave  (hWnd, wParam, lParam);    break;
            case WM_MOUSEWHEEL:  return OnMouseWheel  (0, wParam, lParam);       break;
            case 0x20E /* WM_MOUSEHWHEEL */:
                                 return OnMouseWheel  (1, wParam, lParam);       break;
            case WM_KEYDOWN:     return OnKeyDown (hWnd, uMessage, wParam, lParam); break;
            case WM_SYSKEYDOWN:  return OnKeyDown (hWnd, uMessage, wParam, lParam); break;
            case WM_KEYUP:       return OnKeyUp   (hWnd, uMessage, wParam, lParam); break;
            case WM_SYSKEYUP:    return OnKeyUp   (hWnd, uMessage, wParam, lParam); break;
            // case WM_ERASEBKGND: return true; // never erase background
            case WM_DESTROY:           ::PostQuitMessage (0);                    break; 
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
        bool bopen = clog4m.get_init () && clog4m.get_event_level ().value <= cll4m.Int.value;
        clog4m_t ml_; ml_[cll4m.Int];
        
        WNDCLASSEXW wc; // A properties struct of our window
        ::memset (&wc, 0, sizeof (wc));
                // Zero out the struct and set the stuff we want to modify.
        wc.cbSize        = sizeof (WNDCLASSEXW);
        wc.hInstance     = nullptr;
        wc.lpfnWndProc = mcl_simpletls_ns::bind_mf(&mcl_window_info_t::wndProc, this);
        wc.hCursor       = ::LoadCursor (nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr; // alternative: HBRUSH (COLOR_BACKGROUND + 1)
        wc.lpszClassName = L"mclibTclass";
        wc.hIcon         = ::LoadIcon (nullptr, IDI_APPLICATION);
                // Load a standard icon
        wc.hIconSm       = ::LoadIcon (nullptr, IDI_APPLICATION);
                // use the name "A" to use the project icon
        
#        define MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_()   \
        ml_ << L"Reseting..." << std::endl;                         \
        threaddr         = 0;                                       \
        hwnd             = nullptr;                                 \
        base_w = base_h  = 0;                                       \
        dc_w   = dc_h    = 0;                                       \
        x_pos  = y_pos   = 0;                                       \
        ml_[cll4m.Info]                                             \
            << L"threadMessageLoop initialization terminated.\n";   \
        bErrorCode = true;                                          \
        ::InterlockedExchange (&bIsReady, 1);
        
#        define MCL_UNREGISTERING_WINDOW_()                         \
        if (!::UnregisterClassW (L"mclibTclass", instance))         \
            mcl_report_sysexception (L"Failed to unregister class.")
        
#        define MCL_DESTROY_WINDOW_()                               \
        ml_ << L"Destroying window..." << std::endl;                \
        if (!::DestroyWindow (hwnd))                                \
            mcl_report_sysexception (L"Failed to destroy window.")
        
        // if (bopen) ml_ << L"  Registering class..." << std::endl;
        if (!::RegisterClassExW (&wc)) {
            mcl_report_sysexception (L"Failed to register class.");
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        
        dflags_t flags  = _parm_flags ? *static_cast<dflags_t*>(_parm_flags) : 0;
        LONG_PTR fstyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW, fexstyle = WS_EX_CLIENTEDGE;
        bool     fvisi  = true;
        // if (bopen) ml_.println ("  Setting styles... flags: 0x%08lx", flags);
        if (flags & dflags.NoFrame)       fstyle &= ~WS_CAPTION, fstyle |= WS_POPUP, fexstyle = 0;
        if (!(flags & dflags.Resizable))  fstyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
        if (flags & dflags.NoMinimizeBox) fstyle &= ~WS_MINIMIZEBOX;
        if (flags & dflags.Hidden)        fstyle &= ~WS_VISIBLE, fvisi = false;
        if (flags & dflags.Minimize)      fstyle |= WS_MINIMIZE, fvisi = false;
        if (flags & dflags.Maximize)      fstyle |= WS_MAXIMIZE;
        
        // Adjusting window rect
        RECT wr = {0, 0, dc_w, dc_h};
        if (!::AdjustWindowRectEx (&wr,
                static_cast<DWORD>(fstyle), FALSE, WS_EX_CLIENTEDGE)) {
            mcl_report_sysexception (L"Failed to adjust window rect.");
            MCL_UNREGISTERING_WINDOW_ ();
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        
        if (bopen) ml_.wprintln (L"Creating window... {\"flags\":\"0x%08lx\"}", flags).flush ();
        hwnd = ::CreateWindowExW(
            static_cast<DWORD>(fexstyle),
            LPCWSTR(wc.lpszClassName),
            LPCWSTR(window_caption),
            static_cast<DWORD>(fstyle),
            wr.left + x_pos, // CW_USEDEFAULT
            wr.top + y_pos, // CW_USEDEFAULT
            wr.right - wr.left, wr.bottom - wr.top,
            nullptr, nullptr, nullptr, nullptr
        );
        if (!hwnd) {
            mcl_report_sysexception (L"Failed to create window.");
            MCL_UNREGISTERING_WINDOW_ ();
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_ ();
            return 0;
        }
        
        // if (bopen) ml_ << L"  Loading device contexts..." << std::endl;
        instance = ::GetModuleHandleW (nullptr);
        cur_surface = new(std::nothrow) surface_t({ dc_w, dc_h });
        if (!cur_surface || !*cur_surface) {
            ml_[cll4m.Fatal] << "Failed to create surface." << std::endl;
            MCL_DESTROY_WINDOW_();
            MCL_UNREGISTERING_WINDOW_();
            MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_();
            return 0;
        }
        if (flags & dflags.DoubleBuf) {
        // surface for double buffer
            dbuf_surface = new(std::nothrow) surface_t({ dc_w, dc_h });
        }

        if (b_allow_screensaver_before == 2)
            display.set_allow_screensaver (false);
        
        // for vc6
        if (fvisi) {
            if (!b_maximize)
                ::ShowWindow (hwnd, SW_SHOWDEFAULT); 
            ::SetFocus (mcl_control_obj.hwnd);
        }
    
#        undef MCL_RELEASE_HDC_
#        undef MCL_DESTROY_WINDOW_
#        undef MCL_UNREGISTERING_WINDOW_
#        undef MCL_TERMINATED_THREAD_MESSAGELOOP_AND_SET_FLAG_
        
        if (bopen) {
            ml_.putws (L"\n== Startup complete ==");
            ml_.flush ();
        }
        
        } // end sub for exit
        
        {
        MSG msg;
        ::InterlockedExchange (&bIsReady, 1);
        
        while (::GetMessage (&msg, nullptr, 0, 0) > 0) {
            ::TranslateMessage (&msg);
            ::DispatchMessage (&msg);
        }
        } // end subfor exit

        if (::InterlockedExchange(&bIsExit, 0) == 2ul) {
            ::exit (0);
        }
        return 0;
    }
    
}
