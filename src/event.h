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
    
    @file src/event.h
    This is a C++11 header for interacting with
    events and queues.
*/


#ifndef MCL_EVENT
# define MCL_EVENT

# include "mclfwd.h"
# include <vector>

namespace
mcl {
    
    /**
     * @class event_t <src/event.h>
     * @brief Event types with their specific attributes
     * 
     * @ingroup event
     * @ingroup mclib
     */
    struct
    event_t {
    public:
        struct mcl_active_event_t {
            int gain;
            int state;
        };   // ActiveEvent

        struct mcl_key_event_t {
            unsigned  scancode;
            wchar_t   unicode;
            char      key; // vk-code
            char : 8;
        };   // KeyXxxxx

        struct mcl_mouse_event_t {
            point2d_t pos;
            char      buttons;
            bool      lbutton   : 1; // button = 1
            bool      rbutton   : 1; // button = 3
            bool      shift     : 1;
            bool      ctrl      : 1;
            bool      mbutton   : 1; // button = 2
            bool      xbutton_1 : 1; // button = 4
            bool      xbutton_2 : 1; // button = 5
            bool      wheel     : 1;
            bool      hwhell    : 1;
            char : 7;

            char      button;  // only MouseButtonXxx
            int       del;     // only MouseWheel
        };   // MouseXxxxx

        struct mcl_window_event_t {
            unsigned long long wParam;
            long long          lParam;
        };   // WindowXxxxx
        
    public:
        eventtype_t type;
        char : 8; char : 8; char : 8; char : 8;
        union {
            mcl_active_event_t active; // ActiveEvent
            mcl_key_event_t    key;    // KeyEvent
            mcl_mouse_event_t  mouse;  // MouseEvent
            mcl_window_event_t window; // WindowEvent
            void*              code;   // UserEvent
        };
    };
    
   /**
    * @class mcl_event_t <src/event.h>
    * @brief module for interacting with events and queues
    * 
    * @ingroup event
    * @ingroup display
    * @ingroup mclib
    * @{
    */
    class
    mcl_event_t { public:
        explicit       mcl_event_t ()                       = default;
                      ~mcl_event_t ()                       = default;
                       mcl_event_t (mcl_event_t const& rhs) = delete;
        mcl_event_t&   operator=   (mcl_event_t const& rhs) = delete;

    public:
        using type = eventtype_t;
        enum : int { MaxLQSize = 128 };

        static type constexpr NoEvent         =      0x0;
        static type constexpr Quit            =      0x1;
        static type constexpr ActiveEvent     =      0x2;
        static type constexpr KeyEvent        =      0xc;
        static type constexpr MouseEvent      =     0xf0;
        static type constexpr WindowEvent     =  0xfff00;
        static type constexpr UserEvent       =~0xfffffu;

        static type constexpr KeyDown         =      0x4;
        static type constexpr KeyUp           =      0x8;

        static type constexpr MouseMotion     =     0x10;
        static type constexpr MouseButtonUp   =     0x20;
        static type constexpr MouseButtonDown =     0x40;
        static type constexpr MouseWheel      =     0x80;
        
        static type constexpr WindowShown     =    0x100;
        static type constexpr WindowHidden    =    0x200;
        static type constexpr WindowMoved     =    0x400;
        static type constexpr WindowResized   =    0x800;
        static type constexpr WindowMinimized =   0x1000;
        static type constexpr WindowMaximized =   0x2000;
        static type constexpr WindowRestored  =   0x4000;
        static type constexpr WindowEnter     =   0x8000;
        static type constexpr WindowLeave     =  0x10000;
        static type constexpr WindowFocusGained= 0x20000;
        static type constexpr WindowFocusLost =  0x40000;
        static type constexpr WindowClose     =  0x80000;

        static type constexpr UserEventMin    = 0x100000;
        static type constexpr UserEventMax    = 0x80000000;
        
    public:
        // operator       void*       () const noexcept;
        // bool           operator!   () const noexcept;
        
        // unlike pygame, there is no (need to call) event.pump because
        // mclib always handles internal events automatically.
        // However, once you call the function to get the event, you will
        // need to call exit() for event.Quit . If you do not want to,
        // please call mcl::event.uninterested ().
        void                 uninterested () noexcept;
        // get events from the queue
        std::vector<event_t> get   (void* = 0) noexcept;
        std::vector<event_t> get   (eventtype_t eventtype, void* = 0) noexcept;
        std::vector<event_t> get   (void*, eventtype_t exclude) noexcept;
        // get a single event from the queue
        event_t              poll  () noexcept;
        // wait for a single event from the queue
        event_t              wait  (long timeout = -1) noexcept;
        // test if event types are waiting on the queue
        bool                 peek  (void* = 0) noexcept;
        bool                 peek  (eventtype_t eventtype) noexcept;
        // remove all events from the queue
        void                 clear (void* = 0) noexcept;
        void                 clear (eventtype_t eventtype) noexcept;
        // get the string name from an event id
        wchar_t const*       event_name (eventtype_t type) noexcept;
        char const*          event_name_a (eventtype_t type) noexcept;
        // control which events are allowed on the queue
        void                 set_blocked (void* = 0) noexcept;
        void                 set_blocked (eventtype_t types) noexcept;
        // control which events are allowed on the queue
        void                 set_allowed (void* = 0) noexcept;
        void                 set_allowed (eventtype_t types) noexcept;
        // test if a type of event is blocked from the queue
        bool                 get_blocked (eventtype_t types) noexcept;
        // control the sharing of input devices with other applications
        void                 set_grab_mouse (bool b_grab) noexcept; // unfinished
        void                 set_grab_key (bool b_grab) noexcept; // unfinished
        void                 set_grab (bool b_grab) noexcept; // unfinished
        // test if the program is sharing input devices
        bool                 get_grab_mouse () noexcept;
        bool                 get_grab_key () noexcept;
        bool                 get_grab () noexcept;
        // place a new event on the queue
        bool                 post (event_t const& Event) noexcept;
        // make custom user event type
        eventtype_t          custom_type () noexcept;
    };
    extern mcl_event_t event; // Module for interacting with events and queues.

} // namespace

#endif // MCL_EVENT
