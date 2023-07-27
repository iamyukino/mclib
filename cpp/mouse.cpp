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
#include "../src/surface.h"
#include "../src/cursors.h"

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

#if __cplusplus < 201703L
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnNone;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnLButton;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnRButton;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnShift;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnCtrl;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnMButton;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnXButton1;
    mcl_mouse_t::btn_type constexpr mcl_mouse_t::BtnXButton2;
#endif

    /**
     * @function mcl_mouse_t::get_pressed <src/mouse.h>
     * @brief Get the state of the mouse buttons.
     * @return buttons
     */
    std::vector<bool> mcl_mouse_t::
    get_pressed (int num_buttons) noexcept{
        if (!mcl_control_obj.bIsReady)
            return std::vector<bool>(static_cast<size_t>(
                num_buttons == 3 || num_buttons == 5 ?
                num_buttons : 0
            ));
        if (num_buttons == 3)
            return std::vector<bool>{
                bool(mcl_control_obj.fMouseKeyState & BtnLButton),
                bool(mcl_control_obj.fMouseKeyState & BtnMButton),
                bool(mcl_control_obj.fMouseKeyState & BtnRButton)
            };
        if (num_buttons == 5)
            return std::vector<bool>{
                bool(mcl_control_obj.fMouseKeyState & BtnLButton),
                bool(mcl_control_obj.fMouseKeyState & BtnMButton),
                bool(mcl_control_obj.fMouseKeyState & BtnRButton),
                bool(mcl_control_obj.fMouseKeyState & BtnXButton1),
                bool(mcl_control_obj.fMouseKeyState & BtnXButton2)
            };
        return std::vector<bool>();
    }
    mcl_mouse_t::btn_type mcl_mouse_t::
    get_buttons () noexcept{
        return mcl_control_obj.fMouseKeyState;
    }

    /**
     * @function mcl_mouse_t::get_async_pressed <src/mouse.h>
     * @brief Get the state of the mouse buttons. focus not required
     * @return bool
     */
    bool mcl_mouse_t::
    get_async_pressed (size_t m_index) noexcept{
        switch (m_index) {
            case 0: return bool(::GetAsyncKeyState (VK_LBUTTON) & 0x8000);
            case 1: return bool(::GetAsyncKeyState (VK_MBUTTON) & 0x8000);
            case 2: return bool(::GetAsyncKeyState (VK_RBUTTON) & 0x8000);
            case 3: return bool(::GetAsyncKeyState (VK_XBUTTON1) & 0x8000);
            case 4: return bool(::GetAsyncKeyState (VK_XBUTTON2) & 0x8000);
        }
        return false;
    }
    bool mcl_mouse_t::
    get_async_buttons (btn_type btn_mask) noexcept{
        btn_type btns = 0;
        if (btn_mask & BtnLButton)  btns |= (::GetAsyncKeyState (VK_LBUTTON)  & 0x8000 ? 1 : 2);
        if (btn_mask & BtnRButton)  btns |= (::GetAsyncKeyState (VK_RBUTTON)  & 0x8000 ? 1 : 2);
        if (btn_mask & BtnShift)    btns |= (::GetAsyncKeyState (VK_SHIFT)    & 0x8000 ? 1 : 2);
        if (btn_mask & BtnCtrl)     btns |= (::GetAsyncKeyState (VK_CONTROL)  & 0x8000 ? 1 : 2);
        if (btn_mask & BtnMButton)  btns |= (::GetAsyncKeyState (VK_MBUTTON)  & 0x8000 ? 1 : 2);
        if (btn_mask & BtnXButton1) btns |= (::GetAsyncKeyState (VK_XBUTTON1) & 0x8000 ? 1 : 2);
        if (btn_mask & BtnXButton2) btns |= (::GetAsyncKeyState (VK_XBUTTON2) & 0x8000 ? 1 : 2);
        return (btns & 1) && !(btns & 2);
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
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
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
            && ((mcl_control_obj.bHasIMFocus & 1) || mcl_control_obj.hWndMouseGrabed);
    }

    /**
     * @function mcl_mouse_t::set_cursor <src/mouse.h>
     * @brief Set the mouse cursor to a new cursor
     * @return bool: true if succeeded
     */
    bool mcl_mouse_t::
    set_cursor () noexcept{
        cursor_t it = mcl_control_obj.cucur;
        mcl_control_obj.cucur = nullptr;
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (cursor_t cur) noexcept{
        if (!cur) return false;
        cursor_t it = mcl_control_obj.cucur;
        mcl_control_obj.cucur = cur;
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (sys_cursor_t constant) noexcept{
        cursor_t cur(constant);
        if (!cur) return false;
        cursor_t it = mcl_control_obj.cucur;
        mcl_control_obj.cucur = cur;
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (point2d_t size, point2d_t hotspot,
            unsigned char const* xormasks, unsigned char const* andmasks)
    noexcept{
        cursor_t cur(size, hotspot, xormasks, andmasks);
        if (!cur) return false;
        cursor_t it = mcl_control_obj.cucur;
        mcl_control_obj.cucur = cur;
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (point2d_t size, point2d_t hotspot,
        pytuple< std::vector<unsigned char>, std::vector<unsigned char> >&&
        xor_and_masks)
    noexcept{
        cursor_t cur(size, hotspot, std::move(xor_and_masks));
        if (!cur) return false;
        cursor_t it = mcl_control_obj.cucur;
        mcl_control_obj.cucur = cur;
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (point2d_t hotspot, surface_t surface)
    noexcept{
        cursor_t cur(hotspot, surface);
        if (!cur) return false;
        cursor_t it = mcl_control_obj.cucur;
        mcl_control_obj.cucur = cur;
        ::SendMessage (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    /**
     * @function mcl_mouse_t::get_cursor <src/mouse.h>
     * @brief Get the current mouse cursor
     * @return cursor_t
     */
    cursor_t mcl_mouse_t::
    get_cursor () noexcept{
        return mcl_control_obj.cucur;
    }

}
