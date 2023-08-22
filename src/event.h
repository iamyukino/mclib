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

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4365)
# endif 

# include <vector>
# include <string>

# ifdef _MSC_VER
#  pragma warning(pop)
# endif

namespace
mcl {
    
   /**
    * @todo
    *     mcl::event.set_grab()
    * 
    * @unimplemented
    *     pygame.event.pump()
    *     pygame.event.set_keyboard_grab()
    * 
    * @feature
    *     mcl::event.set_grab_xx()
    *     mcl::event.get_details_xx()
    */

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
            
            char : 8; char : 8; char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
        };   // ActiveEvent

        struct mcl_key_event_t {
            wchar_t       unicode;
            wchar_t       mod; // see key.ModXxxx
            unsigned      scancode;
            unsigned char key;

            char : 8; char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
        };   // KeyEvent

        struct mcl_mouse_event_t {
            point2d_t pos;
            char      buttons; // see mouse.BtnXxxx 
            char      button;
            
            char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
        };   // MouseEvent

        struct mcl_mouse_wheel_t {
        // almost different from pygame
            point2d_t delta;
            char      buttons;
            
            char : 8; char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
        };   // MouseWheel

        struct mcl_window_event_t {
            unsigned long long wParam;
            long long          lParam;
        };   // WindowEvent

        struct mcl_text_input_t {
        // if unicode == 0
        // use event.get_details_xx instead
            wchar_t unicode;
            
            char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
            char : 8; char : 8; char : 8; char : 8;
        };   // MouseWheel
        
    public:
        eventtype_t type;
        char : 8; char : 8; char : 8; char : 8;
        union {
            // nothing                 // QuitEvent
            mcl_active_event_t active; // ActiveEvent
            mcl_key_event_t    key;    // KeyEvent
            mcl_mouse_event_t  mouse;  // MouseEvent
            mcl_mouse_wheel_t  wheel;  // MouseWheelEvent
            mcl_window_event_t window; // WindowEvent
            mcl_text_input_t   text;   // TextInputEvent
            // event.get_details_xx    // ExtendedEvent
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

        static type constexpr NoEvent           =      0x0;

        static type constexpr QuitEvent         =      0x1; // 1
        static type constexpr   Quit            =      0x1;
        
        static type constexpr ActiveEvent       =      0x2; // 2
        static type constexpr   Active          =      0x2;
        
        static type constexpr KeyEvent          =      0xc; // 3-4
        static type constexpr   KeyDown         =      0x4;
        static type constexpr   KeyUp           =      0x8;
        
        static type constexpr MouseEvent        =     0x30; // 5-6
        static type constexpr   MouseMotion     =     0x10;
        static type constexpr   MouseButtonDown =     0x20;
        static type constexpr   MouseButtonUp   =     0x30;
        
        static type constexpr MouseWheelEvent   =     0x40; // 7
        static type constexpr   MouseWheel      =     0x40;
        
     // static type constexpr FingerEvent       =    0x180; // 8-9 (reserve)
     // static type constexpr   FingerMotion    =     0x80;
     // static type constexpr   FingerButtonDown=    0x100;
     // static type constexpr   FingerButtonUp  =    0x180;

        static type constexpr WindowEvent       =   0x1e00; // 10-13
        static type constexpr   WindowShown     =    0x200;
        static type constexpr   WindowHidden    =    0x400;
        static type constexpr   WindowMoved     =    0x600;
        static type constexpr   WindowResized   =    0x800;
        static type constexpr   WindowMinimized =    0xa00;
        static type constexpr   WindowMaximized =    0xc00;
        static type constexpr   WindowRestored  =    0xe00;
        static type constexpr   WindowEnter     =   0x1000;
        static type constexpr   WindowLeave     =   0x1200;
        static type constexpr   WindowFocusGained=  0x1400;
        static type constexpr   WindowFocusLost =   0x1600;
        static type constexpr   WindowClose     =   0x1800;

        static type constexpr TextInputEvent    =   0x6000; // 14-15
        static type constexpr   TextEditing     =   0x2000;
        static type constexpr   TextInput       =   0x4000;
        
        static type constexpr ExtendedEvent     =  0x18000; // 16-17
        static type constexpr   DropFile        =   0x8000;
        static type constexpr   ClipboardUpdate =  0x10000;

        static type constexpr UserEvent         =~0x1ffffu; // 18+
        static type constexpr   UserEventMin    =  0x20000;
        static type constexpr   UserEventMax    =~0x1ffffu;
        
    public:
        operator       void*       () const noexcept;
        bool           operator!   () const noexcept;
        
        // unlike pygame, there is no (need to call) event.pump because
        // mclib always handles internal events automatically.
        // However, once you call the function to get the event, you will
        // need to call exit() for event.Quit . If you do not want to,
        // please call mcl::event.uninterested ().
        void                 uninterested () noexcept;
        // get events from the queue
        std::vector<event_t> get          (void* = 0) noexcept;
        std::vector<event_t> get          (eventtype_t eventtype, void* = 0) noexcept;
        std::vector<event_t> get          (void*, eventtype_t exclude) noexcept;
        // get a single event from the queue
        event_t              poll         () noexcept;
        // wait for a single event from the queue
        event_t              wait         (long timeout = -1) noexcept;
        // test if event types are waiting on the queue
        bool                 peek         (void* = 0) noexcept;
        bool                 peek         (eventtype_t eventtype) noexcept;
        // remove all events from the queue
        void                 clear        (void* = 0) noexcept;
        void                 clear        (eventtype_t eventtype) noexcept;
        // get the string name from an event id
        wchar_t const*       event_name   (eventtype_t type) noexcept;
        wchar_t const*       event_name   (event_t type) noexcept;
        char const*          event_name_a (eventtype_t type) noexcept;
        char const*          event_name_a (event_t type) noexcept;
        // control which events are allowed on the queue
        void                 set_blocked  (void* = 0) noexcept;
        void                 set_blocked  (eventtype_t eventtypes) noexcept;
        // control which events are allowed on the queue
        void                 set_allowed  (void* = 0) noexcept;
        void                 set_allowed  (eventtype_t eventtypes) noexcept;
        // test if a type of event is blocked from the queue
        bool                 get_blocked  (eventtype_t eventtypes) noexcept;
        // control the sharing of input devices with other applications
        // Deprecated: it is better to use key.get_async_pressed & mouse.get_pos
        //     than use set_grab, which may significantly reduce processing speed
        void                 set_grab_mouse (bool b_grab, bool b_alone = true) noexcept;
        void                 set_grab_key (bool b_grab, bool b_alone = true) noexcept;
        void                 set_grab     (bool b_grab, bool b_alone = true) noexcept;
        // test if the program is sharing input devices
        bool                 get_grab_mouse () noexcept;
        bool                 get_grab_key () noexcept;
        bool                 get_grab     () noexcept;
        // get event details from extented events
        std::vector<std::wstring> get_details_dropfile () noexcept;
        std::vector<std::string>  get_details_dropfile_a () noexcept;
        std::wstring         get_details_textediting (size_t* curpos = 0) noexcept;
        std::string          get_details_textediting_a (size_t* curpos = 0) noexcept;
        std::wstring         get_details_textinput () noexcept;
        std::string          get_details_textinput_a () noexcept;
        // place a new event on the queue
        bool                 post         (event_t const& Event) noexcept;
        // make custom user event type
        eventtype_t          custom_type  () noexcept;
    };
    extern mcl_event_t event; // Module for interacting with events and queues.

} // namespace

#endif // MCL_EVENT
