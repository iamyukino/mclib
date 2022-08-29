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
#include "../src/clog4m.h"
#include "../src/timer.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for interacting with events and queues.
    mcl_event_t event;
    mcl_eventqueue_t mcl_event_obj;
    
#if __cplusplus < 201703L
    eventtype_t constexpr mcl_event_t::NoEvent;
    eventtype_t constexpr mcl_event_t::Quit;
    eventtype_t constexpr mcl_event_t::ActiveEvent;
    eventtype_t constexpr mcl_event_t::KeyEvent;
    eventtype_t constexpr mcl_event_t::MouseEvent;
    eventtype_t constexpr mcl_event_t::WindowEvent;
    eventtype_t constexpr mcl_event_t::UserEvent;

    eventtype_t constexpr mcl_event_t::KeyDown;
    eventtype_t constexpr mcl_event_t::KeyUp;

    eventtype_t constexpr mcl_event_t::MouseMotion;
    eventtype_t constexpr mcl_event_t::MouseButtonUp;
    eventtype_t constexpr mcl_event_t::MouseButtonDown;
    eventtype_t constexpr mcl_event_t::MouseWheel;

    eventtype_t constexpr mcl_event_t::WindowShown;
    eventtype_t constexpr mcl_event_t::WindowHidden;
    eventtype_t constexpr mcl_event_t::WindowMoved;
    eventtype_t constexpr mcl_event_t::WindowResized;
    eventtype_t constexpr mcl_event_t::WindowMinimized;
    eventtype_t constexpr mcl_event_t::WindowMaximized;
    eventtype_t constexpr mcl_event_t::WindowRestored;
    eventtype_t constexpr mcl_event_t::WindowEnter;
    eventtype_t constexpr mcl_event_t::WindowLeave;
    eventtype_t constexpr mcl_event_t::WindowFocusGained;
    eventtype_t constexpr mcl_event_t::WindowFocusLost;
    eventtype_t constexpr mcl_event_t::WindowClose;

    eventtype_t constexpr mcl_event_t::UserEventMin;
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_control_obj.bCtrlMsgLoop = true;
        std::vector<event_t> vec{};
        event_t ev{ 0, {{0, 0}} };
        while (mcl_event_obj.pop(ev))
            vec.push_back (ev);
        return vec; 
    }

    std::vector<event_t> mcl_event_t::
    get (eventtype_t eventtype, void*) noexcept {
        if (!eventtype)
            return get ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_control_obj.bCtrlMsgLoop = true;
        std::vector<event_t> vec{};
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_control_obj.bCtrlMsgLoop = true;
        std::vector<event_t> vec{};
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        return static_cast<bool>(mcl_event_obj._len);
    }

    bool mcl_event_t::
    peek (eventtype_t eventtype) noexcept {
        if (!eventtype)
            return peek ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_control_obj.bCtrlMsgLoop = true;
        mcl_event_obj._r = mcl_event_obj._f;
        mcl_event_obj._len = 0;
    }

    void mcl_event_t::
    clear (eventtype_t eventtype) noexcept {
        if (!eventtype)
            return clear ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
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
     * @function mcl_event_t::clear <src/event.h>
     * @brief Removes all events from the queue. If eventtype
     *     is given, removes the given event or sequence of
     *     events. This has the same effect as mcl::event.get()
     *     except nothing is returned. It can be slightly
     *     more efficient when clearing a full event queue.
     */
    wchar_t const* mcl_event_t::
    event_name (eventtype_t type) noexcept{
        if (type & UserEvent)
            return L"UserEvent";

        switch (type) {
            case NoEvent:       return L"NoEvent";
            case Quit:          return L"Quit";
            case ActiveEvent:   return L"ActiveEvent";
            case KeyEvent:      return L"KeyEvent";
            case MouseEvent:    return L"MouseEvent";
            case WindowEvent:   return L"WindowEvent";
            case UserEvent:     return L"UserEvent";

            case KeyDown:       return L"KeyDown";
            case KeyUp:         return L"KeyUp";

            case MouseMotion:   return L"MouseMotion";
            case MouseButtonUp: return L"MouseButtonUp";
            case MouseButtonDown: return L"MouseButtonDown";
            case MouseWheel:    return L"MouseWheel";

            case WindowShown:   return L"WindowShown";
            case WindowHidden:  return L"WindowHidden";
            case WindowMoved:   return L"WindowMoved";
            case WindowResized: return L"WindowResized";
            case WindowMinimized: return L"WindowMinimized";
            case WindowMaximized: return L"WindowMaximized";
            case WindowRestored: return L"WindowRestored";
            case WindowEnter:   return L"WindowEnter";
            case WindowLeave:   return L"WindowLeave";
            case WindowFocusGained: return L"WindowFocusGained";
            case WindowFocusLost: return L"WindowFocusLost";
            case WindowClose:   return L"WindowClose";
            default:            break;
        }
        return L"Unknown";
    }
    char const* mcl_event_t::
    event_name_a (eventtype_t type) noexcept{
        if (type & UserEvent)
            return "UserEvent";

        switch (type) {
            case NoEvent:       return "NoEvent";
            case Quit:          return "Quit";
            case ActiveEvent:   return "ActiveEvent";
            case KeyEvent:      return "KeyEvent";
            case MouseEvent:    return "MouseEvent";
            case WindowEvent:   return "WindowEvent";
            case UserEvent:     return "UserEvent";

            case KeyDown:       return "KeyDown";
            case KeyUp:         return "KeyUp";

            case MouseMotion:   return "MouseMotion";
            case MouseButtonUp: return "MouseButtonUp";
            case MouseButtonDown: return "MouseButtonDown";
            case MouseWheel:    return "MouseWheel";

            case WindowShown:   return "WindowShown";
            case WindowHidden:  return "WindowHidden";
            case WindowMoved:   return "WindowMoved";
            case WindowResized: return "WindowResized";
            case WindowMinimized: return "WindowMinimized";
            case WindowMaximized: return "WindowMaximized";
            case WindowRestored: return "WindowRestored";
            case WindowEnter:   return "WindowEnter";
            case WindowLeave:   return "WindowLeave";
            case WindowFocusGained: return "WindowFocusGained";
            case WindowFocusLost: return "WindowFocusLost";
            case WindowClose:   return "WindowClose";
            default:            break;
        }
        return "Unknown";
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_event_obj._blocked = 0xffffffff;
    }

    void mcl_event_t::
    set_blocked (eventtype_t types) noexcept {
        if (!types)
            return set_blocked ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_event_obj._blocked |= types;
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
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_event_obj._blocked = 0;
    }

    void mcl_event_t::
    set_allowed (eventtype_t types) noexcept{
        if (!types)
            return set_allowed ();
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        mcl_event_obj._blocked &= ~types;
    }

    /**
     * @function mcl_event_t::get_blocked <src/event.h>
     * @brief Returns True if the given event type is blocked from
     *     the queue. If a sequence of event types is passed, this
     *     will return True if any of those event types are blocked.
     */
    bool mcl_event_t::
    get_blocked (eventtype_t types) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        return static_cast<bool>(mcl_event_obj._blocked & types);
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
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (b_grab == static_cast<bool>(mcl_control_obj.hWndMouseGrabed))
            return ;
        if (mcl_control_obj.hWndMouseGrabed) {
            ::UnhookWindowsHookEx (mcl_control_obj.hWndMouseGrabed);
            mcl_control_obj.hWndMouseGrabed = nullptr;
            return ;
        } 
//        mcl_control_obj.hWndMouseGrabed =
//            ::SetWindowsHookEx (WH_MOUSE_LL, MyHookFunMouse, mcl_control_obj.instance, 0);
        return ;
    }

    void mcl_event_t::
    set_grab_key (bool b_grab) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (b_grab == static_cast<bool>(mcl_control_obj.hWndKeyGrabed))
            return ;
        if (mcl_control_obj.hWndKeyGrabed) {
            ::UnhookWindowsHookEx (mcl_control_obj.hWndKeyGrabed);
            mcl_control_obj.hWndKeyGrabed = nullptr;
            return ;
        } 
//        mcl_control_obj.hWndKeyGrabed =
//            ::SetWindowsHookEx (WH_KEYBOARD_LL, MyHookFunKey, mcl_control_obj.instance, 0);
        return ;
    }

    void mcl_event_t::
    set_grab (bool b_grab) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        if (b_grab != static_cast<bool>(mcl_control_obj.hWndMouseGrabed)) {
            if (mcl_control_obj.hWndMouseGrabed) {
                ::UnhookWindowsHookEx (mcl_control_obj.hWndMouseGrabed);
                mcl_control_obj.hWndMouseGrabed = nullptr;
            } else {
//                mcl_control_obj.hWndMouseGrabed =
//                    ::SetWindowsHookEx (WH_MOUSE_LL, MyHookFunMouse, mcl_control_obj.instance, 0);
            }
        }
        if (b_grab != static_cast<bool>(mcl_control_obj.hWndKeyGrabed)) {
            if (mcl_control_obj.hWndKeyGrabed) {
                ::UnhookWindowsHookEx (mcl_control_obj.hWndKeyGrabed);
                mcl_control_obj.hWndKeyGrabed = nullptr;
            } else {
//                mcl_control_obj.hWndKeyGrabed =
//                    ::SetWindowsHookEx (WH_KEYBOARD_LL, MyHookFunKey, mcl_control_obj.instance, 0);
            }
        }
    }

    /**
     * @function mcl_event_t::get_grab <src/event.h>
     * @brief test if the program is sharing input devices
     */
    bool mcl_event_t::
    get_grab_mouse() noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        return static_cast<bool>(mcl_control_obj.hWndMouseGrabed);
    }

    bool mcl_event_t::
    get_grab_key () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        return static_cast<bool>(mcl_control_obj.hWndKeyGrabed);
    }

    bool mcl_event_t::
    get_grab () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
        return mcl_control_obj.hWndMouseGrabed || mcl_control_obj.hWndKeyGrabed;
    }

    /**
     * @function mcl_event_t::get_blocked <src/event.h>
     * @brief Returns True if the given event type is blocked from
     *     the queue. If a sequence of event types is passed, this
     *     will return True if any of those event types are blocked.
     */
    bool mcl_event_t::
    post (event_t const& ev) noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        return static_cast<bool>(mcl_event_obj.push (ev));
    }

    /**
     * @function mcl_event_t::custom_type <src/event.h>
     * @brief Reserves a mcl::event.UserEvent for a custom use.
     *      If too many events are made mcl::event.NoEvent is returned.
     */
    eventtype_t mcl_event_t::
    custom_type () noexcept{
        mcl_simpletls_ns::mcl_spinlock_t lock(mcl_event_obj._lock);
        if (mcl_event_obj._userType == UserEventMax)
            return event.NoEvent;
        mcl_event_obj._userType <<= 1;
        return mcl_event_obj._userType;
    }

}
