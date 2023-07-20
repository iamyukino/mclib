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

    @file cpp/event.cpp
    This is a C++11 implementation file for interacting
    with events and queues.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "mcl_control.h"

#include "../src/event.h"
#include "../src/timer.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for interacting with events and queues.
    mcl_event_t event;
    mcl_eventqueue_t mcl_event_obj;

    mcl_event_t::
    operator void* () const noexcept{
        return mcl_control_obj.bCtrlMsgLoop ?
            const_cast<mcl_event_t*>(this) : nullptr;
    }
    
    bool mcl_event_t::
    operator! () const noexcept{
        return !mcl_control_obj.bCtrlMsgLoop;
    }

    
#if __cplusplus < 201703L
    eventtype_t constexpr mcl_event_t::NoEvent;

    eventtype_t constexpr mcl_event_t::QuitEvent;
    eventtype_t constexpr mcl_event_t::  Quit;
    
    eventtype_t constexpr mcl_event_t::ActiveEvent;
    eventtype_t constexpr mcl_event_t::  Active;
    
    eventtype_t constexpr mcl_event_t::KeyEvent;
    eventtype_t constexpr mcl_event_t::  KeyDown;
    eventtype_t constexpr mcl_event_t::  KeyUp;
    
    eventtype_t constexpr mcl_event_t::MouseEvent;
    eventtype_t constexpr mcl_event_t::  MouseMotion;
    eventtype_t constexpr mcl_event_t::  MouseButtonUp;
    eventtype_t constexpr mcl_event_t::  MouseButtonDown;

    eventtype_t constexpr mcl_event_t::MouseWheelEvent;
    eventtype_t constexpr mcl_event_t::  MouseWheel;

    eventtype_t constexpr mcl_event_t::WindowEvent;
    eventtype_t constexpr mcl_event_t::  WindowShown;
    eventtype_t constexpr mcl_event_t::  WindowHidden;
    eventtype_t constexpr mcl_event_t::  WindowMoved;
    eventtype_t constexpr mcl_event_t::  WindowResized;
    eventtype_t constexpr mcl_event_t::  WindowMinimized;
    eventtype_t constexpr mcl_event_t::  WindowMaximized;
    eventtype_t constexpr mcl_event_t::  WindowRestored;
    eventtype_t constexpr mcl_event_t::  WindowEnter;
    eventtype_t constexpr mcl_event_t::  WindowLeave;
    eventtype_t constexpr mcl_event_t::  WindowFocusGained;
    eventtype_t constexpr mcl_event_t::  WindowFocusLost;
    eventtype_t constexpr mcl_event_t::  WindowClose;

    eventtype_t constexpr mcl_event_t::TextInputEvent;
    eventtype_t constexpr mcl_event_t::  TextEditing;
    eventtype_t constexpr mcl_event_t::  TextInput;

    eventtype_t constexpr mcl_event_t::ExtendedEvent;
    eventtype_t constexpr mcl_event_t::  DropFile;
    eventtype_t constexpr mcl_event_t::  ClipboardUpdate;

    eventtype_t constexpr mcl_event_t::UserEvent;
    eventtype_t constexpr mcl_event_t::  UserEventMin;
    eventtype_t constexpr mcl_event_t::  UserEventMax;
#endif
    
    mcl_eventqueue_t::
    mcl_eventqueue_t () noexcept
      : _r (new (std::nothrow) mcl_lqnode_t()) {
        if (!_r) return;
        _f = _r;
        _r -> next = nullptr;
    }

        
    mcl_eventqueue_t::
    ~mcl_eventqueue_t () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_eventqueue_t::~mcl_eventqueue_t");
        if (!_f) return ;
        do {
            _r = _f;
            _f = _f -> next;
            delete _r;
        } while (_f);
        _f = nullptr;
    }

    /**
     * @function mcl_eventqueue_t::push <src/mcl_control.h>
     * @return int
     */
    int mcl_eventqueue_t::
    push (event_t const& rhs) noexcept{
        if (rhs.type & mcl_event_obj._blocked)
            return 0;
        if (!_f) return 0;
        if (_len == QueueLen) {
        // linkedqueue is full of msg
            mcl_lqnode_t* t = _f -> next -> next;
            _f -> next -> next = _r -> next;
            _r -> next = _f -> next;
            _r = _r -> next;
            _f -> next = t;

            _r -> next = nullptr;
            _r -> data = rhs;
            return 1;
        }
        if (!_r -> next) {
        // never got enough length
            mcl_lqnode_t* node = new (std::nothrow) mcl_lqnode_t();
            if (!node)
                return 0;

            ++ _len;
            _r -> next = node;
            _r = node;

            _r -> next = nullptr;
            _r -> data = rhs;
            return 1;
        }
        // has got enough length
        ++ _len;
        _r = _r -> next;

        _r -> data = rhs;
        return 1;
    }

    /**
     * @function mcl_eventqueue_t::pop <src/mcl_control.h>
     * @return int
     */
    int mcl_eventqueue_t::
    pop (event_t& rhs) noexcept{
        if (!_len)
            return 0;

        -- _len;
        rhs = _f -> next -> data;

        if (!_len) {
            _r = _f;
            return 1;
        }
        mcl_lqnode_t* t = _f -> next -> next;
        _f -> next -> next = _r -> next;
        _r -> next = _f -> next;
        _f -> next = t;
        return 1;
    }

    /**
     * @function mcl_eventqueue_t::pop_next <src/mcl_control.h>
     * @return void
     */
    void mcl_eventqueue_t::
    pop_next (mcl_lqnode_t*& rhs) noexcept{
        -- _len;
        if (rhs -> next == _r) {
            _r = rhs;
            return ;
        }
        mcl_lqnode_t* t = rhs -> next -> next;
        rhs -> next -> next = _r -> next;
        _r -> next = rhs -> next;
        rhs -> next = t;
    }

    /**
     * @function mcl_eventqueue_t::process <src/mcl_control.h>
     * @return void
     */
    void mcl_eventqueue_t::
    process (std::function<bool(mcl_lqnode_t*&)> process_func) noexcept{
        if (!_f) return ;
        for (mcl_lqnode_t *p = _f; (p != _r) && process_func (p););
    }

    /**
     * @function mcl_event_t::uninterested <src/event.h>
     * @return void
     */
    void mcl_event_t::
    uninterested () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::uninterested");
        mcl_control_obj.bCtrlMsgLoop = false;
    }

    /**
     * @function mcl_event_t::get <src/event.h>
     * @brief This will get all the messages and remove them
     *     from the queue. If a type or sequence of types is
     *     given only those messages will be removed from the
     *     queue and returned.
     *     If a type or sequence of types is passed in the
     *     exclude argument instead, then all only other messages
     *     will be removed from the queue. If an exclude parameter
     *     is passed, the eventtype parameter must be None.
     *     If you are only taking specific events from the queue,
     *     be aware that the queue could eventually fill up with
     *     the events you are not interested. 
     *     However, unlike pygame, if the number of events is
     *     greater than mcl::event.MaxLQSize, new events will
     *     directly overwrite the oldest events.
     */
    std::vector<event_t> mcl_event_t::
    get (void*) noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::get");
        mcl_control_obj.bCtrlMsgLoop = true;
        std::vector<event_t> vec;
        event_t ev{ 0, {{0, 0}} };
        while (mcl_event_obj.pop(ev))
            vec.push_back (ev);
        return vec; 
    }

    std::vector<event_t> mcl_event_t::
    get (eventtype_t eventtype, void*) noexcept {
        if (!eventtype)
            return get ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::get");
        mcl_control_obj.bCtrlMsgLoop = true;
        std::vector<event_t> vec;
        mcl_event_obj.process([&vec, &eventtype]
        (mcl_eventqueue_t::mcl_lqnode_t*& node) -> bool {
            if ((node -> next -> data.type) & eventtype) {
                vec.push_back (node -> next -> data);
                mcl_event_obj.pop_next (node);
                return true;
            }
            node = node -> next;
            return true;
        });
        return vec; 
    }

    std::vector<event_t> mcl_event_t::
    get (void*, eventtype_t exclude) noexcept {
        if (!exclude)
            return get ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::get");
        mcl_control_obj.bCtrlMsgLoop = true;
        std::vector<event_t> vec;
        mcl_event_obj.process([&vec, &exclude]
        (mcl_eventqueue_t::mcl_lqnode_t* node) -> bool {
            if ((node -> next -> data.type) & exclude) {
                node = node -> next;
                return true;
            }
            vec.push_back (node -> next -> data);
            mcl_event_obj.pop_next (node);
            return true;
        });
        return vec; 
    }

    /**
     * @function mcl_event_t::poll <src/event.h>
     * @brief Returns a single event from the queue. If
     *     the event queue is empty an event of type
     *     mcl::event.NoEvent will be returned immediately.
     *     The returned event is removed from the queue.
     */
    event_t mcl_event_t::
    poll () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::poll");
        mcl_control_obj.bCtrlMsgLoop = true;
        event_t ret{ 0, {{0, 0}} };
        mcl_event_obj.pop(ret);
        return ret; 
    }

    /**
     * @function mcl_event_t::wait <src/event.h>
     * @brief Returns a single event from the queue. If the queue
     *     is empty this function will wait until one is created. 
     *     If a timeout argument is given, the function will return
     *     an event of type mcl::event.NoEvent if no events enter
     *     the queue in timeout milliseconds. The event is removed
     *     from the queue once it has been returned.
     */
    event_t mcl_event_t::
    wait (long timeout) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::wait");
        mcl_control_obj.bCtrlMsgLoop = true;
        event_t ret{ 0, {{0, 0}} };
        if (mcl_event_obj.pop (ret))
            return ret;

        long long st = timer.get_ticks (), ed = 0;
        do {
            ed = timer.get_ticks ();
            if (timeout >= 0 && ed - st >= timeout)
                break;
            ::Sleep (1);
        } while (!mcl_event_obj.pop(ret));
        return ret; 
    }

    /**
     * @function mcl_event_t::peek <src/event.h>
     * @brief Returns True if there are any events of
     *     the given type waiting on the queue. If a 
     *     sequence of event types is passed, this will
     *     return True if any of those events are on
     *     the queue.
     */
    bool mcl_event_t::
    peek (void*) noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::peek");
        return static_cast<bool>(mcl_event_obj._len);
    }

    bool mcl_event_t::
    peek (eventtype_t eventtype) noexcept {
        if (!eventtype)
            return peek ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::peek");
        bool peeked = false;
        mcl_event_obj.process([&peeked, &eventtype]
        (mcl_eventqueue_t::mcl_lqnode_t* node) -> bool {
            if ((node -> next -> data.type) & eventtype) {
                peeked = true;
                return false;
            }
            node = node -> next;
            return true;
        });
        return peeked;
    }

    /**
     * @function mcl_event_t::clear <src/event.h>
     * @brief Removes all events from the queue. If eventtype
     *     is given, removes the given event or sequence of
     *     events. This has the same effect as mcl::event.get()
     *     except nothing is returned. It can be slightly
     *     more efficient when clearing a full event queue.
     */
    void mcl_event_t::
    clear (void*) noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::clear");
        mcl_control_obj.bCtrlMsgLoop = true;
        mcl_event_obj._r = mcl_event_obj._f;
        mcl_event_obj._len = 0;
    }

    void mcl_event_t::
    clear (eventtype_t eventtype) noexcept {
        if (!eventtype)
            return clear ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::clear");
        mcl_control_obj.bCtrlMsgLoop = true;
        mcl_event_obj.process([&eventtype]
        (mcl_eventqueue_t::mcl_lqnode_t* node) -> bool {
            if ((node -> next -> data.type) & eventtype) {
                mcl_event_obj.pop_next (node);
                return true;
            }
            node = node -> next;
            return true;
        });
    }

    /**
     * @function mcl_event_t::event_name <src/event.h>
     * @brief "UserEvent" is returned for all values in
     *     the user event id range. "Unknown" is returned
     *     when the event type does not exist.
     */
    wchar_t const* mcl_event_t::
    event_name (eventtype_t type) noexcept{
        switch (type) {
            case NoEvent:         return L"NoEvent";
            
            case Quit:            return L"Quit";
            
            case Active:          return L"ActiveEvent";

            case KeyDown:         return L"KeyDown";
            case KeyUp:           return L"KeyUp";

            case MouseMotion:     return L"MouseMotion";
            case MouseButtonUp:   return L"MouseButtonUp";
            case MouseButtonDown: return L"MouseButtonDown";

            case MouseWheel:      return L"MouseWheel";

            case WindowShown:     return L"WindowShown";
            case WindowHidden:    return L"WindowHidden";
            case WindowMoved:     return L"WindowMoved";
            case WindowResized:   return L"WindowResized";
            case WindowMinimized: return L"WindowMinimized";
            case WindowMaximized: return L"WindowMaximized";
            case WindowRestored:  return L"WindowRestored";
            case WindowEnter:     return L"WindowEnter";
            case WindowLeave:     return L"WindowLeave";
            case WindowFocusGained: return L"WindowFocusGained";
            case WindowFocusLost: return L"WindowFocusLost";
            case WindowClose:     return L"WindowClose";

            case TextEditing:     return L"TextEditing";
            case TextInput:       return L"TextInput";

            case DropFile:        return L"DropFile";
            case ClipboardUpdate: return L"ClipboardUpdate";
            default:              break;
        }
        if ((type & UserEvent) && !(type & ~UserEvent))
            return L"UserEvent";
        return L"Unknown";
    }
    wchar_t const* mcl_event_t::
    event_name (event_t type) noexcept{
        return event_name (type.type);
    }
    char const* mcl_event_t::
    event_name_a (eventtype_t type) noexcept{
        switch (type) {
            case NoEvent:         return "NoEvent";
            
            case Quit:            return "Quit";
            
            case Active:          return "ActiveEvent";

            case KeyDown:         return "KeyDown";
            case KeyUp:           return "KeyUp";

            case MouseMotion:     return "MouseMotion";
            case MouseButtonUp:   return "MouseButtonUp";
            case MouseButtonDown: return "MouseButtonDown";

            case MouseWheel:      return "MouseWheel";

            case WindowShown:     return "WindowShown";
            case WindowHidden:    return "WindowHidden";
            case WindowMoved:     return "WindowMoved";
            case WindowResized:   return "WindowResized";
            case WindowMinimized: return "WindowMinimized";
            case WindowMaximized: return "WindowMaximized";
            case WindowRestored:  return "WindowRestored";
            case WindowEnter:     return "WindowEnter";
            case WindowLeave:     return "WindowLeave";
            case WindowFocusGained: return "WindowFocusGained";
            case WindowFocusLost: return "WindowFocusLost";
            case WindowClose:     return "WindowClose";

            case TextEditing:     return "TextEditing";
            case TextInput:       return "TextInput";

            case DropFile:        return "DropFile";
            case ClipboardUpdate: return "ClipboardUpdate";
            default:              break;
        }
        if ((type & UserEvent) && !(type & ~UserEvent))
            return "UserEvent";
        return "Unknown";
    }
    char const* mcl_event_t::
    event_name_a (event_t type) noexcept{
        return event_name_a (type.type);
    }

    /**
     * @function mcl_event_t::set_blocked <src/event.h>
     * @brief The given event types are not allowed to appear
     *     appear on the event queue. By default all events
     *     can be placed on the queue. It is safe to disable
     *     an event type multiple times.
     *     If 0 is passed as the argument, ALL of the event types
     *     are blocked from being placed on the queue.
     */
    void mcl_event_t::
    set_blocked (void*) noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::set_blocked");
        mcl_event_obj._blocked = 0xffffffff;
    }

    void mcl_event_t::
    set_blocked (eventtype_t eventtypes) noexcept {
        if (!eventtypes)
            return set_blocked ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::set_blocked");
        mcl_event_obj._blocked |= eventtypes;
    }

    /**
     * @function mcl_event_t::set_allowed <src/event.h>
     * @brief The given event types are allowed to appear on
     *     the event queue. By default, all event types can
     *     be placed on the queue. It is safe to enable
     *     an event type multiple times.
     *     If 0 is passed as the argument, ALL of the event types
     *     are allowed to be placed on the queue.
     */
    void mcl_event_t::
    set_allowed (void*) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::set_allowed");
        mcl_event_obj._blocked = 0;
    }

    void mcl_event_t::
    set_allowed (eventtype_t eventtypes) noexcept{
        if (!eventtypes)
            return set_allowed ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::set_allowed");
        mcl_event_obj._blocked &= ~eventtypes;
    }

    /**
     * @function mcl_event_t::get_blocked <src/event.h>
     * @brief Returns True if the given event type is blocked from
     *     the queue. If a sequence of event types is passed, this
     *     will return True if any of those event types are blocked.
     */
    bool mcl_event_t::
    get_blocked (eventtype_t eventtypes) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::get_blocked");
        return static_cast<bool>(mcl_event_obj._blocked & eventtypes);
    }

    /**
     * @function mcl_event_t::set_grab <src/event.h>
     * @brief When your program runs in a windowed environment,
     *     it will share the mouse and keyboard devices with
     *     other applications that have focus. If your program
     *     sets the event grab to True, it will lock all input
     *     into your program.
     *     It is best to not always grab the input, since it
     *     prevents the user from doing other things on
     *     their system.
     *     Be careful when the program crashes (either because
     *     of logic error or some other exception) the set_grab
     *     state turned on and it is impossible to turn off the
     *     program afterwards if both mouse and key are grabed.
     */
    void mcl_event_t::
    set_grab_mouse (bool b_grab) noexcept{
        constexpr int MouseFocus = 1, InputFocus = 2;
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.Active;
        ev.active.gain = 0;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock, L"mcl_event_t::set_grab_mouse");
        {
        if (b_grab == static_cast<bool>(mcl_control_obj.hWndMouseGrabed))
            return ;

        if (mcl_control_obj.hWndMouseGrabed) {
        // unhook mouse hook
            if (!::UnhookWindowsHookEx (mcl_control_obj.hWndMouseGrabed))
                return ;
            mcl_control_obj.hWndMouseGrabed = nullptr;

            // post Active Event
            if (!(mcl_control_obj.bHasIMFocus & 1)) {
                ev.active.state = 0;
                if (mcl_control_obj.hWndKeyGrabed)
                    ev.active.state |= InputFocus;
                event.post (ev);
            }
            return ;
        } 

#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 5039)
#       endif // never throw an exception passed to extern C function

        // lock all mouse input into your program
        mcl_control_obj.hWndMouseGrabed =
            ::SetWindowsHookEx (WH_MOUSE_LL, mcl_simpletls_ns::bind_mf (
                    &mcl_window_info_t::hookMouseProc, &mcl_control_obj
                ), mcl_control_obj.instance, 0);

#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
        }
        if (!mcl_control_obj.hWndMouseGrabed)
            return ;

        // post Active Event
        if (!(mcl_control_obj.bHasIMFocus & 1)) {
            ev.active.state = MouseFocus;
            if (mcl_control_obj.hWndKeyGrabed)
                ev.active.state |= InputFocus;
            event.post (ev);
        }
    }

    void mcl_event_t::
    set_grab_key (bool b_grab) noexcept{
        constexpr int MouseFocus = 1, InputFocus = 2;
        event_t ev{ 0, {{0, 0}} };
        ev.type = event.Active;
        ev.active.gain = 0;

        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock, L"mcl_event_t::set_grab_key");
        {
        if (b_grab == static_cast<bool>(mcl_control_obj.hWndKeyGrabed))
            return ;
        
        if (mcl_control_obj.hWndKeyGrabed) {
        // unhook key hook
            if (!::UnhookWindowsHookEx (mcl_control_obj.hWndKeyGrabed) && 0)
                return ;
            mcl_control_obj.hWndKeyGrabed = nullptr;

            // post Active Event
            if (!(mcl_control_obj.bHasIMFocus & 1)) {
                ev.active.state = 0;
                if (mcl_control_obj.hWndMouseGrabed)
                    ev.active.state |= MouseFocus;
                event.post(ev);
            }
            return ;
        } 

#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 5039)
#       endif // never throw an exception passed to extern C function

        // lock all keyboard input into your program
        mcl_control_obj.hWndKeyGrabed =
            ::SetWindowsHookEx (WH_KEYBOARD_LL, mcl_simpletls_ns::bind_mf (
                    &mcl_window_info_t::hookKeyBdProc, &mcl_control_obj
                ), mcl_control_obj.instance, 0);

#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
        }
        if (!mcl_control_obj.hWndKeyGrabed)
            return ;

        // post Active Event
        if (!(mcl_control_obj.bHasIMFocus & 1)) {
            ev.active.state = InputFocus;
            if (mcl_control_obj.hWndMouseGrabed)
                ev.active.state |= MouseFocus;
            event.post (ev);
        }
    }

    void mcl_event_t::
    set_grab (bool b_grab) noexcept{
        set_grab_mouse (b_grab);
        set_grab_key   (b_grab);
    }

    /**
     * @function mcl_event_t::get_grab <src/event.h>
     * @brief test if the program is sharing input devices
     */
    bool mcl_event_t::
    get_grab_mouse() noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock, L"mcl_event_t::get_grab");
        return static_cast<bool>(mcl_control_obj.hWndMouseGrabed);
    }

    bool mcl_event_t::
    get_grab_key () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock, L"mcl_event_t::get_grab");
        return static_cast<bool>(mcl_control_obj.hWndKeyGrabed);
    }

    bool mcl_event_t::
    get_grab () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock, L"mcl_event_t::get_grab");
        return mcl_control_obj.hWndMouseGrabed || mcl_control_obj.hWndKeyGrabed;
    }

    /**
     * @function mcl_event_t::post <src/event.h>
     * @brief Place a new event on the queue.
     */
    bool mcl_event_t::
    post (event_t const& Event) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::post");
        return static_cast<bool>(mcl_event_obj.push (Event));
    }

    /**
     * @function mcl_event_t::get_details_dropfile <src/event.h>
     * @brief Get event details from DropFile events.
     */
    std::vector<std::wstring> mcl_event_t::
    get_details_dropfile () noexcept{
        std::vector<std::wstring> vec;
        wchar_t szPath[_MAX_PATH]{ 0 };
        HDROP hdrop = 0;
        {
            mcl_simpletls_ns::mcl_spinlock_t lock(mcl_base_obj.droplock, L"mcl_event_t::get_details_dropfile");
            hdrop = mcl_control_obj.droppeddatas;
            mcl_control_obj.droppeddatas = 0;
        }
        UINT cnt = ::DragQueryFileW (hdrop, 0xffffffff, NULL, 0), it = 0;
        while (it != cnt) {
            ::DragQueryFileW (hdrop, it, szPath, _MAX_PATH);
            vec.emplace_back (szPath);
            ++ it;
        }
        ::DragFinish (hdrop);
        return vec;
    }
    
    std::vector<std::string> mcl_event_t::
    get_details_dropfile_a () noexcept{
        std::vector<std::string> vec;
        char szPath[_MAX_PATH]{ 0 };
        HDROP hdrop = 0;
        {
            mcl_simpletls_ns::mcl_spinlock_t lock(mcl_base_obj.droplock, L"mcl_event_t::get_details_dropfile_a");
            hdrop = mcl_control_obj.droppeddatas;
            mcl_control_obj.droppeddatas = 0;
        }
        UINT cnt = ::DragQueryFileA (hdrop, 0xffffffff, NULL, 0), it = 0;
        while (it != cnt) {
            ::DragQueryFileA (hdrop, it, szPath, _MAX_PATH);
            vec.emplace_back (szPath);
            ++ it;
        }
        ::DragFinish (hdrop);
        return vec;
    }
    static std::wstring
    mcl_imm_getstr (DWORD type, size_t* psz = 0) noexcept{
        using igcs_t = decltype(ImmGetCompositionStringW);
        igcs_t* fpImmGetCompositionStringW = mcl_get_immfunc<igcs_t>("ImmGetCompositionStringW");
        if (!fpImmGetCompositionStringW) return std::wstring(L"");

        using igc_t = decltype(ImmGetContext);
        igc_t* fpImmGetContext = mcl_get_immfunc<igc_t>("ImmGetContext");
        if (!fpImmGetContext) return std::wstring(L"");

        using irc_t = decltype(ImmReleaseContext);
        irc_t* fpImmReleaseContext = mcl_get_immfunc<irc_t>("ImmReleaseContext");
        if (!fpImmReleaseContext) return std::wstring(L"");

        HIMC hImc = fpImmGetContext (mcl_control_obj.hwnd);
        if (!hImc) return std::wstring(L"");

        DWORD dwSize = static_cast<DWORD>(fpImmGetCompositionStringW (hImc, type, NULL, 0));
        dwSize += sizeof(wchar_t);
        HGLOBAL hstr = ::GlobalAlloc (GHND, dwSize);
        if (!hstr) {
            fpImmReleaseContext (mcl_control_obj.hwnd, hImc);
            return std::wstring(L"");
        }

        if (psz) {
            LONG sz = fpImmGetCompositionStringW (hImc, GCS_CURSORPOS, 0, 0);
            *psz = static_cast<size_t>(sz >= 0 ? sz : 0);
        }
        
        LPVOID lpstr = ::GlobalLock (hstr);
        if (!lpstr) {
            fpImmReleaseContext (mcl_control_obj.hwnd, hImc);
            ::GlobalFree (hstr);
            return std::wstring(L"");
        }
        fpImmGetCompositionStringW (hImc, type, lpstr, dwSize);
        std::wstring retstr(reinterpret_cast<wchar_t*>(lpstr));
        fpImmReleaseContext (mcl_control_obj.hwnd, hImc);
        // add this string into text buffer of application
        ::GlobalUnlock (hstr);
        ::GlobalFree (hstr);
        return retstr;
    }
    static std::string
    mcl_imm_getstr_a (DWORD type, size_t* psz = 0) noexcept{
        using igcs_t = decltype(ImmGetCompositionStringA);
        igcs_t* fpImmGetCompositionStringA = mcl_get_immfunc<igcs_t>("ImmGetCompositionStringA");
        if (!fpImmGetCompositionStringA) return std::string("");

        using igc_t = decltype(ImmGetContext);
        igc_t* fpImmGetContext = mcl_get_immfunc<igc_t>("ImmGetContext");
        if (!fpImmGetContext) return std::string("");

        using irc_t = decltype(ImmReleaseContext);
        irc_t* fpImmReleaseContext = mcl_get_immfunc<irc_t>("ImmReleaseContext");
        if (!fpImmReleaseContext) return std::string("");

        HIMC hImc = fpImmGetContext (mcl_control_obj.hwnd);
        if (!hImc) return std::string("");

        DWORD dwSize = static_cast<DWORD>(fpImmGetCompositionStringA (hImc, type, NULL, 0));
        dwSize += sizeof(wchar_t);
        HGLOBAL hstr = ::GlobalAlloc (GHND, dwSize);
        if (!hstr) {
            fpImmReleaseContext (mcl_control_obj.hwnd, hImc);
            return std::string("");
        }

        if (psz) {
            LONG sz = fpImmGetCompositionStringA (hImc, GCS_CURSORPOS, 0, 0);
            *psz = static_cast<size_t>(sz >= 0 ? sz : 0);
        }
        
        LPVOID lpstr = ::GlobalLock (hstr);
        if (!lpstr) {
            fpImmReleaseContext (mcl_control_obj.hwnd, hImc);
            ::GlobalFree (hstr);
            return std::string("");
        }
        fpImmGetCompositionStringA (hImc, type, lpstr, dwSize);
        std::string retstr(reinterpret_cast<char*>(lpstr));
        fpImmReleaseContext (mcl_control_obj.hwnd, hImc);
        // add this string into text buffer of application
        ::GlobalUnlock (hstr);
        ::GlobalFree (hstr);
        return retstr;
    }
    std::wstring mcl_event_t::
    get_details_textediting (size_t* curpos) noexcept{
        return mcl_imm_getstr (GCS_COMPSTR, curpos);
    }
    std::wstring mcl_event_t::
    get_details_textinput () noexcept{
        return mcl_imm_getstr (GCS_RESULTSTR);
    }
    std::string mcl_event_t::
    get_details_textediting_a (size_t* curpos) noexcept{
        return mcl_imm_getstr_a (GCS_COMPSTR, curpos);
    }
    std::string mcl_event_t::
    get_details_textinput_a () noexcept{
        return mcl_imm_getstr_a (GCS_RESULTSTR);
    }

    /**
     * @function mcl_event_t::custom_type <src/event.h>
     * @brief Reserves a mcl::event.UserEvent for a custom use.
     *      If too many events are made mcl::event.NoEvent is returned.
     */
    eventtype_t mcl_event_t::
    custom_type () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock, L"mcl_event_t::custom_type");
        if (mcl_event_obj._userType == UserEventMax)
            return event.NoEvent;
        mcl_event_obj._userType += UserEventMin;
        return mcl_event_obj._userType;
    }

}
