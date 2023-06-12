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

    @file cpp/mouse.cpp
    This is a C++11 implementation file for working
    with the mouse.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "mcl_control.h"

#include "../src/pyobj.h"
#include "../src/mouse.h"
#include "../src/clog4m.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for working with the mouse.
    mcl_mouse_t mouse;

    mcl_mouse_t::
    operator void* () const noexcept{
        return mcl_control_obj.bIsReady ?
            const_cast<mcl_mouse_t*>(this) : 0;
    }
    bool mcl_mouse_t::
    operator! () const noexcept{
        return !mcl_control_obj.bIsReady;
    }

    /**
     * @function mcl_mouse_t::get_pressed <src/mouse.h>
     * @brief Get the state of the mouse buttons.
     * @return buttons
     */
    pytuple<bool, bool, bool> mcl_mouse_t::
    mcl_get_pressed_3 () const noexcept{
        bool l = 0, m = 0, r = 0;
        // l = (::GetAsyncKeyState (VK_LBUTTON) & 0x8000);
        // m = (::GetAsyncKeyState (VK_MBUTTON) & 0x8000);
        // r = (::GetAsyncKeyState (VK_RBUTTON) & 0x8000);
        if (mcl_control_obj.bIsReady) {
            l = mcl_control_obj.bMouseKeyState & 0x01; // 1
            m = mcl_control_obj.bMouseKeyState & 0x10; // 5
            r = mcl_control_obj.bMouseKeyState & 0x02; // 2
        }
        return maktuple (l, m, r);
    }
    pytuple<bool, bool, bool, bool, bool> mcl_mouse_t::
    mcl_get_pressed_5 () const noexcept{
        bool l = 0, m = 0, r = 0, s1 = 0, s2 = 0;
        // l  = (::GetAsyncKeyState (VK_LBUTTON)  & 0x8000);
        // m  = (::GetAsyncKeyState (VK_MBUTTON)  & 0x8000);
        // r  = (::GetAsyncKeyState (VK_RBUTTON)  & 0x8000);
        // s1 = (::GetAsyncKeyState (VK_XBUTTON1) & 0x8000);
        // s2 = (::GetAsyncKeyState (VK_XBUTTON2) & 0x8000);
        if (mcl_control_obj.bIsReady) {
            l  = mcl_control_obj.bMouseKeyState & 0x01; // 1
            m  = mcl_control_obj.bMouseKeyState & 0x10; // 5
            r  = mcl_control_obj.bMouseKeyState & 0x02; // 2
            s1 = mcl_control_obj.bMouseKeyState & 0x20; // 6
            s2 = mcl_control_obj.bMouseKeyState & 0x40; // 7
        }
        return maktuple (l, m, r, s1, s2);
    }

    /**
     * @function mcl_mouse_t::get_pos <src/mouse.h>
     * @brief Get the mouse cursor position.
     * @param[in] b_global: false if position is relative to
     *     the top-left corner of the display.
     * @return buttons
     */
    point2d_t mcl_mouse_t::
    get_pos (bool b_global) noexcept{
        POINT point{ 0, 0 };
        if (!(mcl_control_obj.bIsReady || b_global))
            return point2d_t{ 0, 0 };
        
        ::GetCursorPos (&point);
        if (!b_global)
            ::ScreenToClient(mcl_control_obj.hwnd, &point);
        return point2d_t{ point.x, point.y };
    }

    
    /**
     * @function mcl_mouse_t::set_pos <src/mouse.h>
     * @brief Set the mouse cursor position.
     * @param[in] b_global: false if position is relative to
     *     the top-left corner of the display.
     * @return bool
     */
    bool mcl_mouse_t::
    set_pos (point2d_t pos, bool b_global) noexcept{
        POINT point{ pos.x, pos.y };
        if (!(mcl_control_obj.bIsReady || b_global))
            return false;

        if (!b_global)
            ::ClientToScreen(mcl_control_obj.hwnd, &point);
        return static_cast<bool>(::SetCursorPos(point.x, point.y));
    }

    /**
     * @function mcl_mouse_t::set_visible <src/mouse.h>
     * @brief Hide or show the mouse cursor.
     * @param[in] b_visible: true if show cursor
     * @return bool: previous visibility
     */
    bool mcl_mouse_t::
    set_visible (bool b_visible) noexcept{
        if (b_visible == !mcl_control_obj.bHideCursor)
            return b_visible;

        mcl_control_obj.bHideCursor = !b_visible;
        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, 0);
        return !b_visible;
    }
    
    /**
     * @function mcl_mouse_t::get_visible <src/mouse.h>
     * @brief Get the current visibility state of the mouse cursor
     * @return bool: current visibility
     */
    bool mcl_mouse_t::
    get_visible () noexcept{
        // CURSORINFO ci = { sizeof(CURSORINFO), 0, 0, {0, 0} };
        // if (::GetCursorInfo (&ci))
        //     return ci.flags & CURSOR_SHOWING;
        // return false;

        return mcl_control_obj.bIsReady
            && !mcl_control_obj.bHideCursor;
    }

    /**
     * @function mcl_mouse_t::get_focused <src/mouse.h>
     * @brief Check if the display is receiving mouse input
     * @return bool
     */
    bool mcl_mouse_t::
    get_focused () noexcept{
        return mcl_control_obj.bIsReady
            && (mcl_control_obj.bHasIMFocus & 1);
    }

}
