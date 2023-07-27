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

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/event.h"
#include "../src/cursors.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    class
    mcl_window_info_t {
    public:
        mcl_window_info_t () {
            window_caption[0] = '\0';
        }

    public:
        unsigned threadMessageLoop (void* title); // message loop
        LRESULT CALLBACK wndProc          // callback
            (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
    
    public:   // callback functions
        LRESULT OnClose       (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnNCHitTest   (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnSize        (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnMove        (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnPaint       (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnActivate    (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnShowWindow  (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnMouseMove   (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnButtonDown  (char type, WPARAM wParam, LPARAM lParam);
        LRESULT OnButtonUp    (char type, WPARAM wParam, LPARAM lParam);
        LRESULT OnMouseWheel  (char type, WPARAM wParam, LPARAM lParam);
        LRESULT OnMouseLeave  (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnSetCursor   (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnKeyDown     (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
        LRESULT OnKeyUp       (HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);
        LRESULT OnTimer       (HWND hWnd, WPARAM wParam, LPARAM lParam);
#if (WINVER >= 0x0600)
        LRESULT OnClipboardUpdate (HWND hWnd, WPARAM wParam, LPARAM lParam);
#endif
        LRESULT OnDrawClipboard (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnChangeCBChain (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnDropFiles   (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnChar        (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnIMEChar     (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnIMEComposition (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnIMENotify   (HWND hWnd, WPARAM wParam, LPARAM lParam);
        LRESULT OnIMEEndComposition (HWND hWnd, WPARAM wParam, LPARAM lParam);

    public:
        VOID ToggleWallpaperProc ();
        LRESULT CALLBACK hookMouseProc (int nCode, WPARAM wParam, LPARAM lParam);
        LRESULT CALLBACK hookKeyBdProc (int nCode, WPARAM wParam, LPARAM lParam);

    public:
        bool hookOnMouseMove  (PMOUSEHOOKSTRUCTEX mouseData);
        bool hookOnMouseWheel (char type, PMOUSEHOOKSTRUCTEX mouseData);
        bool hookOnButtonDown (char type, PMOUSEHOOKSTRUCTEX mouseData);
        bool hookOnButtonUp   (char type, PMOUSEHOOKSTRUCTEX mouseData);

        bool hookOnKeyDown    (UINT uMessage, PKBDLLHOOKSTRUCT keybdData);
        bool hookOnKeyUp      (UINT uMessage, PKBDLLHOOKSTRUCT keybdData);

    public:   // window properties
        HINSTANCE instance   = nullptr;
        HWND      hwnd       = nullptr;
        HANDLE    taskhandle = nullptr;    // thread handle
        HICON     hicon      = nullptr;
        void*     timermap   = nullptr;
        void*     keymap     = nullptr;
        LONGLONG  timer      = 0ll;
        enum : UINT_PTR { timerIdMin = UINT_PTR(~eventtype_t(0)), timerWallpaper};
        unsigned  threaddr   = 0u;         // thread id
        TCHAR     window_caption[_MAX_FNAME];

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
        bool      b_wallpaper                = false;
        bool      b_maximize                 = false;
        bool      b_setdpiunaware            = false;

    public:   // state quantities
        unsigned  bIsReady = 0ul;          // whether message loop starts
        unsigned  bIsExit  = 0ul;
        bool      bErrorCode = false;      // successful flag
        bool      bCtrlMsgLoop = false;    // whether msg loop is taken over
        bool      bAtQuitInClose = false;

    public:   // mouse & key events
        bool      bMouseInClient = false;
        char      bHasIMFocus = 0;
        bool      bHideCursor = false;
        bool      bRepeatCount = false;

        char      fMouseKeyState = 0;
        wchar_t   fModKeyState = 0;
        char      fMouseCapture = 0;

        bool      bMouseHookAlone = false;
        bool      bKeyHookAlone = false;

        char : 8; char : 8; char : 8;
        char : 8; char : 8; char : 8; char : 8;

        cursor_t  cucur;

        HWND      hWndWorkerW = nullptr;
        HHOOK     hWndMouseGrabed = nullptr;
        HHOOK     hWndKeyGrabed = nullptr;

    public:
        HDROP     droppeddatas = nullptr;
        HMODULE   hModImm32 = nullptr;
        HWND      hwndNextViewer = nullptr;
        COMPOSITIONFORM immcpf{ 0, {0, 0}, {0, 0, 0, 0} };
        bool      bUseAddCBL = false;

        char : 8; char : 8; char : 8;

    public:   // surface
        surface_t* cur_surface = nullptr;  // current surface
        surface_t* dbuf_surface = nullptr; // surface for double buffer
    };
    
    extern mcl_window_info_t mcl_control_obj;
    unsigned long mcl_set_dbi_awareness (bool awareness = false) noexcept;
    void mcl_report_sysexception (wchar_t const* what);
    void mcl_reset_check () noexcept;


    /**
     * @class mcl_eventqueue_t <src/surface.cpp>
     * @brief Event queue for event.h
     */
    class
    mcl_eventqueue_t {
    public:
        struct mcl_lqnode_t {
            event_t data{ 0, {{0, 0}} };
            mcl_lqnode_t* next = nullptr;
        };

    public:
             mcl_eventqueue_t () noexcept;
            ~mcl_eventqueue_t () noexcept;
        int  push (event_t const& rhs) noexcept;
        int  pop  (event_t& rhs) noexcept;
        void pop_next (mcl_lqnode_t*& rhs) noexcept;
        void process (std::function<bool(mcl_lqnode_t*&)> process_func) noexcept;

    public:
        typename mcl_simpletls_ns::
            mcl_spinlock_t::lock_t _lock = 0ul;
        
        int _len = 0;
        mcl_lqnode_t* _f = nullptr;
        mcl_lqnode_t* _r = nullptr;
        enum : int { QueueLen = mcl_event_t::MaxLQSize };

        eventtype_t _blocked = 0;
        eventtype_t _userType = mcl::mcl_event_t::UserEventMin;
    };
    extern mcl_eventqueue_t mcl_event_obj;
    

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

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4191)
# endif // C4191: 'Function cast' : unsafe conversion.
    template <typename fun_t>
    fun_t* mcl_get_immfunc (char const* funcname){
        if (!mcl_control_obj.hModImm32)
            return nullptr;
        return reinterpret_cast<fun_t*>(
            ::GetProcAddress (mcl_control_obj.hModImm32, funcname)
        );
    }
# ifdef _MSC_VER
#  pragma warning(pop)
# endif


} // namespace

#endif // MCL_MCLCONTROL
