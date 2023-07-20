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
#include "../src/mouse.h"
#include "../src/key.h"
#include "../src/clog4m.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#include <cerrno>     // for errno
#include <cstring>    // for strerrno
#include <map>

namespace
mcl {
    
    mcl_window_info_t mcl_control_obj;
    
    struct mcl_timermape_t { event_t event;
        int loops; char : 8; char : 8; char : 8; char : 8; };
    using mcl_timermap_t = std::map<UINT_PTR, mcl_timermape_t>;

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
        if (mcl_control_obj.b_setdpiunaware)
            return 0ul;

        HMODULE hModShcoredll = ::LoadLibrary (_T("Shcore.dll"));
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
            if (pfSetProcessDpiAwareness) {
                mcl_control_obj.b_setdpiunaware = awareness;
                pfSetProcessDpiAwareness (awareness ? PROCESS_DPI_UNAWARE : PROCESS_PER_MONITOR_DPI_AWARE);
            }
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
            ::FreeLibrary (hModShcoredll);
        }
        return ret ? 0ul : ::GetLastError ();
    } 
    
    extern void mcl_do_atquit ();

    void mcl_reset_check () noexcept{
        // called when ends abnormally
    }

   /**
    * @function mcl_window_info_t::OnXxxxx <cpp/mcl_control.cpp>
    * @brief Callback function.
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
            ev.window.lParam = 0;
            ev.window.wParam = 0;
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
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock, L"mcl_window_info_t::OnClose");
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
#if (WINVER >= 0x0600)
        if (bUseAddCBL) {
            typedef BOOL (WINAPI *LPFN_ACFL)(HWND);
            HMODULE hModUser32 = ::GetModuleHandle (_T("user32"));
            if (hModUser32) {
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4191)
# endif // C4191: 'Function cast' : unsafe conversion.
                LPFN_ACFL lpRemoveClipboardFormatListener =
                    reinterpret_cast<LPFN_ACFL>(::GetProcAddress (
                        hModUser32, "RemoveClipboardFormatListener"));
# ifdef _MSC_VER
#  pragma warning(pop)
# endif
                if (lpRemoveClipboardFormatListener)
                    lpRemoveClipboardFormatListener (hwnd);
                ::FreeLibrary (hModUser32);
            }
            bUseAddCBL = false;
        }
        else {
            ::ChangeClipboardChain (hwnd, hwndNextViewer);
            hwndNextViewer = nullptr;
        }
#else
        ::ChangeClipboardChain (hwnd, hwndNextViewer);
        hwndNextViewer = nullptr;
#endif
        // if (bopen) ml_ << L"  Destroying window..." << std::endl;
        if (!::DestroyWindow (hwnd)) {
            mcl_report_sysexception (L"Failed to destroy window.");
            bErrorCode = true;
        }
        // if (bopen) ml_ << L"  Unregistering class..." << std::endl;
        if (!::UnregisterClass (_T("MclibWnd"), instance)) {
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
        if (cucur) {
            ::SetCursor (0);
            cucur = nullptr;
        }
        if (timermap) {
            mcl_simpletls_ns::mcl_spinlock_t timerlock (mcl_base_obj.timerlock, L"mcl_window_info_t::OnClose");
            if (timermap) {
                delete reinterpret_cast<mcl_timermap_t*>(timermap);
                timermap = nullptr;
            }
        }
        if (keymap) {
            mcl_simpletls_ns::mcl_spinlock_t keymaplock (mcl_base_obj.keymaplock, L"mcl_window_info_t::OnClose");
            delete reinterpret_cast<std::vector<BYTE>*>(keymap);
            keymap = nullptr;
        }
        if (droppeddatas) {
            mcl_simpletls_ns::mcl_spinlock_t droplock (mcl_base_obj.droplock, L"mcl_window_info_t::OnClose");
            ::DragFinish (droppeddatas);
            droppeddatas = nullptr;
        }
        if (hModImm32) {
            ::FreeLibrary (hModImm32);
            hModImm32 = nullptr;
        }
        immcpf = { 0, {0, 0}, {0, 0, 0, 0} };
        mcl_event_obj._userType = mcl::mcl_event_t::UserEventMin;
        fMouseKeyState = 0;
        fModKeyState = 0;
        fMouseCapture = 0;

        // window properties           // positions
        threaddr      = 0;             base_w  = base_h   = 0;
        hwnd          = nullptr;       dc_w    = dc_h     = 0;
        instance      = nullptr;       x_pos   = y_pos    = 0;
        taskhandle    = nullptr;       // switchs
        window_caption[0] = '\0';      b_setdpiunaware = false;
                                       b_fullscreen    = false;
        timer         = 0;             b_maximize      = false;

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
        bHideCursor = false;
        bRepeatCount = false;
        
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
        LRESULT hit = ::DefWindowProc (hWnd, WM_NCHITTEST, wParam, lParam);
/*
        if (b_fullscreen ||
            (::GetWindowLongPtr (hWnd, GWL_STYLE) & WS_CAPTION)
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
                LONG_PTR llstyle = ::GetWindowLongPtr (hWnd, GWL_STYLE);
                LONG_PTR exstyle = ::GetWindowLongPtr (hWnd, GWL_EXSTYLE);
                llstyle = (llstyle & ~WS_THICKFRAME) | WS_VISIBLE | WS_POPUP;
                exstyle = exstyle | WS_EX_CLIENTEDGE;
                
                RECT wr = {-1, -1, base_w + 1, base_h + 1};
                ::AdjustWindowRectEx (&wr, static_cast<DWORD>(llstyle),
                                        FALSE, static_cast<DWORD>(exstyle));
                
                b_fullscreen = false;
                ::SetWindowLongPtr   (hWnd, GWL_STYLE, static_cast<DWORD>(llstyle));
                ::SetWindowLongPtr   (hWnd, GWL_EXSTYLE, static_cast<DWORD>(exstyle));
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
        if (cur_surface)
            cur_surface -> resize ({this -> dc_w, this -> dc_h}, false);
        
        ev.type = event.WindowResized;
        event.post (ev);

        return 0;
    }

    LRESULT mcl_window_info_t::
    OnMove (HWND hWnd, WPARAM wParam, LPARAM lParam) {
    // WM_MOVE
        LONG_PTR lstyle = ::GetWindowLongPtr (hWnd, GWL_STYLE);
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

        mcl_imagebuf_t* buf = mcl_get_surface_dataplus (cur_surface);
        if (cur_surface && buf) {
            mcl_simpletls_ns::mcl_spinlock_t lock (buf -> m_nrtlock, L"mcl_window_info_t::OnPaint");
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
        ev.type = event.Active;
        ev.window.lParam = 0;
        ev.window.wParam = 0;

        if (fActive == WA_INACTIVE) ev.active.gain = 0, bHasIMFocus = 0;
        else                        ev.active.gain = 1, bHasIMFocus |= 1;
        if (fActive == WA_CLICKACTIVE) bHasIMFocus |= 2;
        
        constexpr int MouseFocus = 1, InputFocus = 2, MouseActive = 4;
        if (bHasIMFocus & 1)        ev.active.state |= (MouseFocus | InputFocus);
        else if (hWndMouseGrabed)   ev.active.state |= MouseFocus;
        else if (hWndKeyGrabed)     ev.active.state |= InputFocus;
        if (bHasIMFocus & 2)        ev.active.state |= MouseActive;

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
        
        if (!hWndMouseGrabed) {
            ev.type = event.MouseMotion;
            ev.mouse.pos.x = GET_X_LPARAM (lParam);
            ev.mouse.pos.y = GET_Y_LPARAM (lParam);
            fMouseKeyState = static_cast<char>(GET_KEYSTATE_WPARAM(wParam));
            ev.mouse.buttons = fMouseKeyState;
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
    OnSetCursor (HWND, WPARAM wParam, LPARAM lParam) {
        if (bHideCursor) { ::SetCursor (0); return TRUE; }
        if (LOWORD(lParam) == HTCLIENT && cucur) {
            ::SetCursor ((**reinterpret_cast<mcl_simpletls_ns::
                mcl_shared_ptr_t<HCURSOR>**>(&cucur)).ptr());
            return TRUE;
        }
        return ::DefWindowProc (hwnd, WM_SETCURSOR, wParam, lParam);
    }

    LRESULT mcl_window_info_t::
    OnMouseWheel (char type, WPARAM wParam, LPARAM) {
        if (!hWndMouseGrabed) {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.MouseWheel;
            if (type) ev.wheel.delta.x = GET_WHEEL_DELTA_WPARAM (wParam);
            else      ev.wheel.delta.y = GET_WHEEL_DELTA_WPARAM (wParam);
            fMouseKeyState = static_cast<char>(GET_KEYSTATE_WPARAM(wParam));
            ev.wheel.buttons = fMouseKeyState;
            event.post (ev);
        }
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnButtonDown (char type, WPARAM wParam, LPARAM lParam) {
        if (!fMouseCapture)
            ::SetCapture (hwnd);
        fMouseCapture |= (1 << type);

        if (!hWndMouseGrabed) {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.MouseButtonDown;
            ev.mouse.button = type;
            if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
                ++ ev.mouse.button;
            ev.mouse.pos.x = GET_X_LPARAM (lParam);
            ev.mouse.pos.y = GET_Y_LPARAM (lParam);
            ev.mouse.buttons = fMouseKeyState = static_cast<char>(GET_KEYSTATE_WPARAM (wParam));
            event.post (ev);
        }
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnButtonUp (char type, WPARAM wParam, LPARAM lParam) {
        if (fMouseCapture) {
            fMouseCapture &= ~(1 << type);
            if (!fMouseCapture)
                ::ReleaseCapture ();
        }
        if (!hWndMouseGrabed) {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.MouseButtonUp;
            ev.mouse.button = type;
            if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
                ++ ev.mouse.button;
            ev.mouse.pos.x = GET_X_LPARAM (lParam);
            ev.mouse.pos.y = GET_Y_LPARAM (lParam);
            ev.mouse.buttons = fMouseKeyState = static_cast<char>(GET_KEYSTATE_WPARAM (wParam));
            event.post (ev);
        }
        return 0;
    }

    static void
    mcl_set_mods_state (unsigned char* keys, wchar_t& mod) noexcept{
        if (keys[VK_LSHIFT]  & 0x80) mod |= key.ModLShift;
        if (keys[VK_RSHIFT]  & 0x80) mod |= key.ModRShift;
        if (keys[VK_LCONTROL]& 0x80) mod |= key.ModLCtrl;
        if (keys[VK_RCONTROL]& 0x80) mod |= key.ModRCtrl;
        if (keys[VK_LMENU]   & 0x80) mod |= key.ModLAlt;
        if (keys[VK_RMENU]   & 0x80) mod |= key.ModRAlt;
        if (keys[VK_LWIN]    & 0x80) mod |= key.ModLMeta;
        if (keys[VK_RWIN]    & 0x80) mod |= key.ModRMeta;
        if (keys[VK_CAPITAL] &  0x1) mod |= key.ModCaps;
        if (keys[VK_NUMLOCK] &  0x1) mod |= key.ModNum;
        if ((mod & key.ModLCtrl) && (mod & key.ModRAlt)) mod |= key.ModMode;
        mcl_control_obj.fModKeyState = mod;
    }

    LRESULT mcl_window_info_t::
    OnKeyDown (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
        if (hWndKeyGrabed)
            return ::DefWindowProc (hWnd, uMessage, wParam, lParam);

        WORD keyFlags = HIWORD(lParam);
        BOOL wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;
        if (bRepeatCount || !wasKeyDown) {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.KeyDown;
            ev.key.key = static_cast<unsigned char>(LOWORD(wParam));
            ev.key.scancode = LOBYTE(keyFlags);

            // get unicode
            {
                mcl_simpletls_ns::mcl_spinlock_t lock(mcl_base_obj.keymaplock, L"mcl_window_info_t::OnKeyDown");
                std::vector<BYTE>*& keys = *reinterpret_cast<std::vector<BYTE>**>(&keymap);
                if (!keys) keys = new (std::nothrow) std::vector<BYTE>(256, 0);
                if (keys) {
                    wchar_t buffer[2];
                    void(::GetKeyboardState (keys -> data())); // std::ignore
                    if (::ToUnicode (static_cast<UINT>(wParam), ev.key.scancode, keys -> data(), buffer, 1, 0))
                        ev.key.unicode = buffer[0];
                    mcl_set_mods_state (keys -> data(), ev.key.mod);
                }
            }

            // extended keys
            BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
            if (isExtendedKey)
                ev.key.scancode = MAKEWORD(ev.key.scancode, 0xe0);
            
            // distinguish keys
            switch (ev.key.key) {
                case VK_SHIFT:  case VK_CONTROL:  case VK_MENU: {
                    ev.key.key = static_cast<unsigned char>(LOWORD(
                        ::MapVirtualKey (ev.key.scancode, MAPVK_VSC_TO_VK_EX)));
                    break;
                }
            }

            event.post (ev);
            
            if (bRepeatCount) {
                WORD repeatCount = LOWORD(lParam);
                while (-- repeatCount)
                    event.post (ev);
            }
        }
        if (uMessage == WM_SYSKEYDOWN)
            return ::DefWindowProc (hWnd, uMessage, wParam, lParam);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnKeyUp (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam) {
        if (hWndKeyGrabed)
            return ::DefWindowProc (hWnd, uMessage, wParam, lParam);

        WORD keyFlags = HIWORD(lParam);
        {
            event_t ev{ 0, {{0, 0}} };
            ev.type = event.KeyUp;
            ev.key.key = static_cast<unsigned char>(LOWORD(wParam));
            ev.key.scancode = LOBYTE(keyFlags);

            // get unicode
            {
                mcl_simpletls_ns::mcl_spinlock_t lock(mcl_base_obj.keymaplock, L"mcl_window_info_t::OnKeyUp");
                std::vector<BYTE>*& keys = *reinterpret_cast<std::vector<BYTE>**>(&keymap);
                if (!keys) keys = new (std::nothrow) std::vector<BYTE>(256, 0);
                if (keys) {
                    wchar_t buffer[2];
                    void(::GetKeyboardState (keys -> data())); // std::ignore
                    if (::ToUnicode (static_cast<UINT>(wParam), ev.key.scancode, keys -> data(), buffer, 1, 0))
                        ev.key.unicode = buffer[0];
                    mcl_set_mods_state (keys -> data(), ev.key.mod);
                }
            }

            // extended keys
            BOOL isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
            if (isExtendedKey)
                ev.key.scancode = MAKEWORD(ev.key.scancode, 0xe0);
            
            // distinguish keys
            switch (ev.key.key) {
                case VK_SHIFT:  case VK_CONTROL:  case VK_MENU: {
                    ev.key.key = static_cast<unsigned char>(LOWORD(
                        ::MapVirtualKey (ev.key.scancode, MAPVK_VSC_TO_VK_EX)));
                    break;
                }
            }

            event.post (ev);
        }
        if (uMessage == WM_SYSKEYUP)
            return ::DefWindowProc (hWnd, uMessage, wParam, lParam);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnTimer (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        do {
            if (!timermap) break;
            mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.timerlock, L"mcl_window_info_t::OnTimer");
            if (!timermap) break;

            mcl_timermap_t& tmap = *reinterpret_cast<mcl_timermap_t*>(timermap);
            mcl_timermap_t::iterator it = tmap.find (wParam);
            if (it == tmap.end ()) break;
        
            ev.type = it -> second.event.type;
            ev.window.wParam = it -> second.event.window.wParam;
            ev.window.lParam = it -> second.event.window.lParam;
        
            if (it -> second.loops) {
                -- (it -> second.loops);
                if (!it -> second.loops)
                    tmap.erase (it);
            }
        } while (0);
        
        if (ev.type) 
            event.post (ev);

        return ::DefWindowProc (hWnd, WM_TIMER, wParam, lParam);
    }
#if (WINVER >= 0x0600)
    LRESULT mcl_window_info_t::
    OnClipboardUpdate (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        if (!bUseAddCBL)
            return ::DefWindowProc (hWnd, WM_CLIPBOARDUPDATE, wParam, lParam);

        event_t ev{ 0, {{0, 0}} };
        ev.type = event.ClipboardUpdate;
        ev.window.lParam = 0;
        ev.window.wParam = 0;
        event.post (ev);
        return 0;
    }
#endif
    LRESULT mcl_window_info_t::
    OnDrawClipboard (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        if (bUseAddCBL)
            return ::DefWindowProc (hWnd, WM_DRAWCLIPBOARD, wParam, lParam);

        if (hwndNextViewer)
            ::SendMessage (hwndNextViewer, WM_DRAWCLIPBOARD, wParam, lParam);

        event_t ev{ 0, {{0, 0}} };
        ev.type = event.ClipboardUpdate;
        ev.window.lParam = 0;
        ev.window.wParam = 0;
        event.post (ev);
        return 0;
    }
    LRESULT mcl_window_info_t::
    OnChangeCBChain (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        if (bUseAddCBL)
            return ::DefWindowProc (hWnd, WM_CHANGECBCHAIN, wParam, lParam);

        if (reinterpret_cast<HWND>(reinterpret_cast<void*>(wParam)) == hwndNextViewer)
            hwndNextViewer = reinterpret_cast<HWND>(reinterpret_cast<void*>(lParam));
        else if (hwndNextViewer)
            ::SendMessage (hwndNextViewer, WM_CHANGECBCHAIN, wParam, lParam);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnDropFiles (HWND, WPARAM wParam, LPARAM) {
        HDROP hDrop = reinterpret_cast<HDROP>(reinterpret_cast<void*>(wParam)), hOldDrop = 0;
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.DropFile;
        ev.window.lParam = 0;
        ev.window.wParam = 0;
        {
            mcl_simpletls_ns::mcl_spinlock_t lock(mcl_base_obj.droplock, L"mcl_window_info_t::OnDropFiles");
            hOldDrop = droppeddatas;
            droppeddatas = hDrop;
        }
        event.post (ev);

        if (hOldDrop)
            ::DragFinish (hOldDrop);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnChar (HWND, WPARAM wParam, LPARAM) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.TextInput;
        ev.text.unicode = static_cast<wchar_t>(wParam);
        event.post (ev);
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnIMEChar (HWND, WPARAM, LPARAM) {
        return 0;
    }

    LRESULT mcl_window_info_t::
    OnIMEComposition (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.TextEditing;
        ev.window.lParam = 0;
        ev.window.wParam = 0;
        event.post (ev);
        return ::DefWindowProc (hWnd, WM_IME_COMPOSITION, wParam, lParam);
    }
    
    LRESULT mcl_window_info_t::
    OnIMEEndComposition (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.TextInput;
        ev.text.unicode = 0;
        event.post (ev);
        return ::DefWindowProc (hWnd, WM_IME_ENDCOMPOSITION, wParam, lParam);
    }

    LRESULT mcl_window_info_t::
    OnIMENotify (HWND hWnd, WPARAM wParam, LPARAM lParam) {
        if (wParam == 0 && lParam == 0) {
            // key.stop_text_input()
            using igc_t = decltype(ImmGetContext);
            igc_t* fpImmGetContext = mcl_get_immfunc<igc_t>("ImmGetContext");
            if (!fpImmGetContext) return 0;

            using ini_t = decltype(ImmNotifyIME);
            ini_t* fpImmNotifyIME = mcl_get_immfunc<ini_t>("ImmNotifyIME");
            if (!fpImmNotifyIME) return 0;

            using iace_t = decltype(ImmAssociateContextEx);
            iace_t* fpImmAssociateContextEx = mcl_get_immfunc<iace_t>("ImmAssociateContextEx");
            if (!fpImmAssociateContextEx) return 0;

            using idc_t = decltype(ImmDestroyContext);
            idc_t* fpImmDestroyContext = mcl_get_immfunc<idc_t>("ImmDestroyContext");
            if (!fpImmDestroyContext) return 0;

            HIMC hImc = fpImmGetContext (hwnd);
            fpImmNotifyIME (hImc, NI_COMPOSITIONSTR, CPS_CANCEL, 0);
            fpImmNotifyIME (hImc, NI_CLOSECANDIDATE, 0, 0);
            fpImmAssociateContextEx (hwnd, NULL, IACE_CHILDREN);
            fpImmDestroyContext (hImc);
            immcpf = { 0, {0, 0}, {0, 0, 0, 0} };
            return 0;
        } else if (wParam == 0 && lParam == 1) {
            // key.start_text_input()
            using icc_t = decltype(ImmCreateContext);
            icc_t* fpImmCreateContext = mcl_get_immfunc<icc_t>("ImmCreateContext");
            if (!fpImmCreateContext) return 0;

            using iace_t = decltype(ImmAssociateContextEx);
            iace_t* fpImmAssociateContextEx = mcl_get_immfunc<iace_t>("ImmAssociateContextEx");
            if (!fpImmAssociateContextEx) return 0;

            HIMC hImc = fpImmCreateContext ();
            fpImmAssociateContextEx (mcl_control_obj.hwnd, hImc, IACE_CHILDREN);
            return 0;
        }
        return ::DefWindowProc (hWnd, WM_IME_NOTIFY, wParam, lParam);
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
            case WM_SETCURSOR:   return OnSetCursor   (hWnd, wParam, lParam);    break;
            case WM_MOUSEWHEEL:  return OnMouseWheel  (0, wParam, lParam);       break;
            case 0x20E /* WM_MOUSEHWHEEL */:
                                 return OnMouseWheel  (1, wParam, lParam);       break;
            case WM_KEYDOWN:     return OnKeyDown (hWnd, uMessage, wParam, lParam); break;
            case WM_SYSKEYDOWN:  return OnKeyDown (hWnd, uMessage, wParam, lParam); break;
            case WM_KEYUP:       return OnKeyUp   (hWnd, uMessage, wParam, lParam); break;
            case WM_SYSKEYUP:    return OnKeyUp   (hWnd, uMessage, wParam, lParam); break;
            case WM_TIMER:       return OnTimer       (hWnd, wParam, lParam);    break;
            case WM_DROPFILES:   return OnDropFiles   (hWnd, wParam, lParam);    break;
#if (WINVER >= 0x0600)
            case WM_CLIPBOARDUPDATE: return OnClipboardUpdate (hWnd, wParam, lParam); break;
#endif
            case WM_DRAWCLIPBOARD: return OnDrawClipboard (hWnd, wParam, lParam);break;
            case WM_CHANGECBCHAIN: return OnChangeCBChain (hWnd, wParam, lParam);break;
            case WM_CHAR:        return OnChar        (hWnd, wParam, lParam);    break;
            case WM_IME_CHAR:    return OnIMEChar     (hWnd, wParam, lParam);    break;
            case WM_IME_COMPOSITION: return OnIMEComposition (hWnd, wParam, lParam); break;
            case WM_IME_NOTIFY:  return OnIMENotify   (hWnd, wParam, lParam);    break;
            case WM_IME_ENDCOMPOSITION: return OnIMEEndComposition (hWnd, wParam, lParam); break;
            // case WM_ERASEBKGND: return true; // never erase background
            case WM_DESTROY:            ::PostQuitMessage (0);                   break; 
            default:             return ::DefWindowProc (hWnd, uMessage, wParam, lParam);
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
        
        WNDCLASSEX wc; // A properties struct of our window
        ::memset (&wc, 0, sizeof (wc));
                // Zero out the struct and set the stuff we want to modify.
        wc.cbSize        = sizeof (WNDCLASSEXW);
        wc.hInstance     = nullptr;
        wc.lpfnWndProc = mcl_simpletls_ns::bind_mf(&mcl_window_info_t::wndProc, this);
        wc.hCursor       = ::LoadCursor (nullptr, IDC_ARROW);
        wc.hbrBackground = nullptr; // alternative: HBRUSH (COLOR_BACKGROUND + 1)
        wc.lpszClassName = _T("MclibWnd");
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
        if (!::UnregisterClass (_T("MclibWnd"), instance))       \
            mcl_report_sysexception (L"Failed to unregister class.")
        
#        define MCL_DESTROY_WINDOW_()                               \
        ml_ << L"Destroying window..." << std::endl;                \
        if (!::DestroyWindow (hwnd))                                \
            mcl_report_sysexception (L"Failed to destroy window.")
        
        // if (bopen) ml_ << L"  Registering class..." << std::endl;
        if (!::RegisterClassEx (&wc)) {
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
        if (flags & dflags.AllowDropping) fexstyle |= WS_EX_ACCEPTFILES;
        
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
        hwnd = ::CreateWindowEx (
            static_cast<DWORD>(fexstyle),
            LPCTSTR(wc.lpszClassName),
            LPCTSTR(window_caption),
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

#ifndef UOI_TIMERPROC_EXCEPTION_SUPPRESSION
        int UOI_TIMERPROC_EXCEPTION_SUPPRESSION = 7;
#endif
        BOOL bfalse = FALSE;
        ::SetUserObjectInformation (::GetCurrentProcess (),
            UOI_TIMERPROC_EXCEPTION_SUPPRESSION, reinterpret_cast<void*>(&bfalse), sizeof(BOOL));
        
        // if (bopen) ml_ << L"  Loading device contexts..." << std::endl;
        instance = ::GetModuleHandle (nullptr);
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
            ::SetFocus (hwnd);
        }

        // event.ClipboardUpdate
#if (WINVER >= 0x0600)
        typedef BOOL (WINAPI *LPFN_ACFL)(HWND);
        HMODULE hModUser32 = ::GetModuleHandle (_T("user32"));
        if (hModUser32) {
# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4191)
# endif // C4191: 'Function cast' : unsafe conversion.
            LPFN_ACFL lpAddClipboardFormatListener =
                reinterpret_cast<LPFN_ACFL>(::GetProcAddress(
                    hModUser32, "AddClipboardFormatListener"));
# ifdef _MSC_VER
#  pragma warning(pop)
# endif
            if (lpAddClipboardFormatListener) {
                lpAddClipboardFormatListener (hwnd);
                bUseAddCBL = true;
            }
            ::FreeLibrary (hModUser32);
        }
        if (!bUseAddCBL)
            hwndNextViewer = ::SetClipboardViewer (hwnd);
#else
        hwndNextViewer = ::SetClipboardViewer (hwnd);
#endif
        if (!hModImm32)
            hModImm32 = ::GetModuleHandle (_T("Imm32.dll"));
        
    
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

   /**
    * @function mcl_window_info_t::hookMouseProc <cpp/mcl_control.cpp>
    * @brief see event.set_grab_mouse (event.cpp)
    */
    LRESULT CALLBACK mcl_window_info_t::
    hookMouseProc (int nCode, WPARAM wParam, LPARAM lParam){
        PMOUSEHOOKSTRUCTEX pMouseData =
            reinterpret_cast<PMOUSEHOOKSTRUCTEX>(reinterpret_cast<void*>(lParam));
        if (nCode >= 0) {
            switch (wParam) {
                case WM_MOUSEMOVE:   if (hookOnMouseMove  (pMouseData))    break; return 1;
                case WM_MOUSEWHEEL:  if (hookOnMouseWheel (0, pMouseData)) break; return 1;
                case 0x20E /* WM_MOUSEHWHEEL */:
                                     if (hookOnMouseWheel (1, pMouseData)) break; return 1;
                case WM_LBUTTONDOWN: if (hookOnButtonDown (1, pMouseData)) break; return 1;
                case WM_MBUTTONDOWN: if (hookOnButtonDown (2, pMouseData)) break; return 1;
                case WM_RBUTTONDOWN: if (hookOnButtonDown (3, pMouseData)) break; return 1;
                case WM_XBUTTONDOWN: if (hookOnButtonDown (4, pMouseData)) break; return 1;
                case WM_LBUTTONUP:   if (hookOnButtonUp   (1, pMouseData)) break; return 1;
                case WM_MBUTTONUP:   if (hookOnButtonUp   (2, pMouseData)) break; return 1;
                case WM_RBUTTONUP:   if (hookOnButtonUp   (3, pMouseData)) break; return 1;
                case WM_XBUTTONUP:   if (hookOnButtonUp   (4, pMouseData)) break; return 1;
            }
        }
        return ::CallNextHookEx (hWndMouseGrabed, nCode, wParam, lParam);
    }

    static void
    mcl_update_mousekeystate (char& buttons) {
        buttons = 0;
        if (::GetAsyncKeyState (VK_LBUTTON)  & 0x8000) buttons |= mouse.BtnLButton;
        if (::GetAsyncKeyState (VK_RBUTTON)  & 0x8000) buttons |= mouse.BtnRButton;
        if (::GetAsyncKeyState (VK_SHIFT)    & 0x8000) buttons |= mouse.BtnShift;
        if (::GetAsyncKeyState (VK_CONTROL)  & 0x8000) buttons |= mouse.BtnCtrl;
        if (::GetAsyncKeyState (VK_MBUTTON)  & 0x8000) buttons |= mouse.BtnMButton;
        if (::GetAsyncKeyState (VK_XBUTTON1) & 0x8000) buttons |= mouse.BtnXButton1;
        if (::GetAsyncKeyState (VK_XBUTTON2) & 0x8000) buttons |= mouse.BtnXButton2;
    }

    bool mcl_window_info_t::
    hookOnMouseMove (PMOUSEHOOKSTRUCTEX mouseData){
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseMotion;
        ::ScreenToClient (hwnd, &mouseData -> pt);
        ev.mouse.pos.x = mouseData -> pt.x;
        ev.mouse.pos.y = mouseData -> pt.y;
        
        mcl_update_mousekeystate (ev.mouse.buttons);
        fMouseKeyState = ev.mouse.buttons;
        event.post (ev);

        return true;
    }
    bool mcl_window_info_t::
    hookOnMouseWheel (char type, PMOUSEHOOKSTRUCTEX mouseData){
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseWheel;
        if (type) ev.wheel.delta.x = GET_WHEEL_DELTA_WPARAM (mouseData -> mouseData);
        else      ev.wheel.delta.y = GET_WHEEL_DELTA_WPARAM (mouseData -> mouseData);
        
        mcl_update_mousekeystate (ev.wheel.buttons);
        fMouseKeyState = ev.wheel.buttons;
        event.post (ev);

        bool ret = ::WindowFromPoint (mouseData -> pt) == hwnd;
        return ret;
    }
    bool mcl_window_info_t::
    hookOnButtonDown (char type, PMOUSEHOOKSTRUCTEX mouseData) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseButtonDown;
        ev.mouse.button = type;
        if (GET_XBUTTON_WPARAM(mouseData -> mouseData) == XBUTTON2)
            ++ ev.mouse.button;

        bool ret = ::WindowFromPoint (mouseData -> pt) == hwnd;

        ::ScreenToClient (hwnd, &mouseData -> pt);
        ev.mouse.pos.x = mouseData -> pt.x;
        ev.mouse.pos.y = mouseData -> pt.y;
        
        mcl_update_mousekeystate (ev.mouse.buttons);
        fMouseKeyState = ev.mouse.buttons;
        event.post (ev);

        return ret;
    }
    bool mcl_window_info_t::
    hookOnButtonUp (char type, PMOUSEHOOKSTRUCTEX mouseData) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.MouseButtonUp;
        ev.mouse.button = type;
        if (GET_XBUTTON_WPARAM(mouseData -> mouseData) == XBUTTON2)
            ++ ev.mouse.button;

        // bool ret = ::WindowFromPoint (mouseData -> pt) == hwnd;

        ::ScreenToClient (hwnd, &mouseData -> pt);
        ev.mouse.pos.x = mouseData -> pt.x;
        ev.mouse.pos.y = mouseData -> pt.y;
        
        mcl_update_mousekeystate (ev.mouse.buttons);
        fMouseKeyState = ev.mouse.buttons;
        event.post (ev);

        return true; // ret;
    }
  
   /**
    * @function mcl_window_info_t::hookKeyBdProc <cpp/mcl_control.cpp>
    * @brief see event.set_grab_key (event.cpp)
    */
    LRESULT CALLBACK mcl_window_info_t::
    hookKeyBdProc (int nCode, WPARAM wParam, LPARAM lParam){
        PKBDLLHOOKSTRUCT pVirKey =
            reinterpret_cast<PKBDLLHOOKSTRUCT>(reinterpret_cast<void*>(lParam));
        if (nCode >= 0) {
            switch (wParam) {
                case WM_KEYDOWN:    if (hookOnKeyDown (static_cast<UINT>(wParam), pVirKey)) break; return 1;
                case WM_SYSKEYDOWN: if (hookOnKeyDown (static_cast<UINT>(wParam), pVirKey)) break; return 1;
                case WM_KEYUP:      if (hookOnKeyUp   (static_cast<UINT>(wParam), pVirKey)) break; return 1;
                case WM_SYSKEYUP:   if (hookOnKeyUp   (static_cast<UINT>(wParam), pVirKey)) break; return 1;
            }
        }
        return ::CallNextHookEx (hWndKeyGrabed, nCode, wParam, lParam);
    }
        
    bool mcl_window_info_t::
    hookOnKeyDown (UINT uMessage, PKBDLLHOOKSTRUCT keybdData) {
        bool bExt = uMessage == WM_SYSKEYDOWN || keybdData -> flags & LLKHF_ALTDOWN ||
            keybdData -> vkCode == VK_NUMLOCK || keybdData -> vkCode == VK_CAPITAL;
        event_t ev{ 0, {{0, 0}} };

        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.keymaplock, L"mcl_window_info_t::hookOnKeyDown");
        {
        std::vector<BYTE>*& keys = *reinterpret_cast<std::vector<BYTE>**>(&keymap);
        if (!keys) keys = new (std::nothrow) std::vector<BYTE>(256, 0);
        
        bool bKeyCount = bRepeatCount || !keys || !((*keys)[keybdData -> vkCode] & 0x80);
        if (!bKeyCount) return bExt;

        ev.type = event.KeyDown;
        ev.key.key = static_cast<unsigned char>(keybdData -> vkCode);
        ev.key.scancode = keybdData -> scanCode;

        // get unicode
        if (keys) {
            wchar_t buffer[2];
            // void(::GetKeyboardState (keys -> data())); // std::ignore
            keys -> data()[keybdData -> vkCode] = (keybdData -> flags & LLKHF_UP ? 0x80u : 0x81u);
            if (::ToUnicode (keybdData -> vkCode, ev.key.scancode, keys -> data(), buffer, 1, 0))
                ev.key.unicode = buffer[0];
            mcl_set_mods_state (keys -> data(), ev.key.mod);
        }
        }
        
        // extended keys
        BOOL isExtendedKey = (keybdData -> flags & LLKHF_EXTENDED) == LLKHF_EXTENDED;
        if (isExtendedKey)
            ev.key.scancode = MAKEWORD(ev.key.scancode, 0xe0);
            
        // distinguish keys
        switch (ev.key.key) {
            case VK_SHIFT:  case VK_CONTROL:  case VK_MENU: {
                ev.key.key = static_cast<unsigned char>(LOWORD(
                    ::MapVirtualKey (ev.key.scancode, MAPVK_VSC_TO_VK_EX)));
                break;
            }
        }

        event.post (ev);
        return bExt;
    }

    bool mcl_window_info_t::
    hookOnKeyUp (UINT, PKBDLLHOOKSTRUCT keybdData) {
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.KeyUp;
        ev.key.key = static_cast<unsigned char>(keybdData -> vkCode);
        ev.key.scancode = keybdData -> scanCode;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.keymaplock, L"mcl_window_info_t::hookOnKeyUp");
        {
            // get unicode
            std::vector<BYTE>*& keys = *reinterpret_cast<std::vector<BYTE>**>(&keymap);
            if (!keys) keys = new (std::nothrow) std::vector<BYTE>(256, 0);
            if (keys) {
                wchar_t buffer[2];
                // void(::GetKeyboardState (keys -> data())); // std::ignore
                keys -> data()[keybdData -> vkCode] = (keybdData -> flags & LLKHF_UP ? 0x00u : 0x01u);
                if (::ToUnicode (keybdData -> vkCode, ev.key.scancode, keys -> data(), buffer, 1, 0))
                    ev.key.unicode = buffer[0];
                mcl_set_mods_state (keys -> data(), ev.key.mod);
            }
        }
        
        // extended keys
        BOOL isExtendedKey = (keybdData -> flags & LLKHF_EXTENDED) == LLKHF_EXTENDED;
        if (isExtendedKey)
            ev.key.scancode = MAKEWORD(ev.key.scancode, 0xe0);
            
        // distinguish keys
        switch (ev.key.key) {
            case VK_SHIFT:  case VK_CONTROL:  case VK_MENU: {
                ev.key.key = static_cast<unsigned char>(LOWORD(
                    ::MapVirtualKey (ev.key.scancode, MAPVK_VSC_TO_VK_EX)));
                break;
            }
        }

        event.post (ev);
        return true;
    }
    
}
