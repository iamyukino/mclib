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
        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
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

    /**
     * @function mcl_mouse_t::set_cursor <src/mouse.h>
     * @brief Set the mouse cursor to a new cursor
     * @return bool: true if succeeded
     */
    bool mcl_mouse_t::
    set_cursor () noexcept{
        mcl_control_obj.hcursor = 0;
        mcl_control_obj.nCursorType = 0;
        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (cursor_t cur) noexcept{
        if (!(cur.m_data_[0] && cur.m_dataplus_)) return false;
        mcl_control_obj.hcursor = reinterpret_cast<HCURSOR>(cur.m_dataplus_);
        mcl_control_obj.nCursorType = cur.m_data_[0];
        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (sys_cursor_t constant) noexcept{
        LPTSTR  sys_cur = nullptr;
        HCURSOR hcur = 0;
        switch (constant) {
            case cursors.Arrow:       sys_cur = IDC_ARROW;      break;
            case cursors.Ibeam:       sys_cur = IDC_IBEAM;      break;
            case cursors.Wait:        sys_cur = IDC_WAIT;       break;
            case cursors.CrossHair:   sys_cur = IDC_CROSS;      break;
            case cursors.WaitArrow:   sys_cur = IDC_APPSTARTING;break;
            case cursors.SizeNWSE:    sys_cur = IDC_SIZENWSE;   break;
            case cursors.SizeNESW:    sys_cur = IDC_SIZENESW;   break;
            case cursors.SizeWE:      sys_cur = IDC_SIZEWE;     break;
            case cursors.SizeNS:      sys_cur = IDC_SIZENS;     break;
            case cursors.SizeAll:     sys_cur = IDC_SIZEALL;    break;
            case cursors.No:          sys_cur = IDC_NO;         break;
# ifdef IDC_HAND
            case cursors.Hand:        sys_cur = IDC_HAND;       break;
# endif
            case cursors.UpArrow:     sys_cur = IDC_UPARROW;    break;
# ifdef IDC_HELP
            case cursors.Help:        sys_cur = IDC_HELP;       break;
# endif
# if    defined (IDC_PIN) && defined (IDC_PERSON)
            case cursors.Pin:         sys_cur = IDC_PIN;        break;
            case cursors.Person:      sys_cur = IDC_PERSON;     break;
# endif
            default:                  return false;
        }
        hcur = ::LoadCursor (0, sys_cur);
        if (!hcur) return false;

        mcl_control_obj.hcursor = hcur;
        mcl_control_obj.nCursorType = 1;
        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (point2d_t size, point2d_t hotspot,
            unsigned char const* xormasks, unsigned char const* andmasks)
    noexcept{
        if (size.x != size.y || size.x & 7) return false;
        if (size.x & 15) size.x += 8;

        HCURSOR hcur = ::CreateCursor (
            nullptr, hotspot.x, hotspot.y, size.x, size.x, andmasks, xormasks);
        if (!hcur) return false;

        if (mcl_control_obj.hcursor)
            ::DestroyCursor (mcl_control_obj.hcursor);
        mcl_control_obj.hcursor = hcur;
        mcl_control_obj.nCursorType = 2;

        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    bool mcl_mouse_t::
    set_cursor (point2d_t size, point2d_t hotspot,
        pytuple< std::vector<unsigned char>, std::vector<unsigned char> >&&
        xor_and_masks)
    noexcept{
        std::vector<unsigned char> cx = xor_and_masks[0], ca = xor_and_masks[1];
        if (!cx.size()) return false;
        return set_cursor (size, hotspot, &cx[0], &ca[0]);
    }

    bool mcl_mouse_t::
    set_cursor (point2d_t hotspot, surface_t surface)
    noexcept{
        point1d_t wid = surface.get_width (), hei = surface.get_height ();
        if (!(wid && hei)) return false;

        if (wid != hei) wid = (wid < hei ? wid : hei), hei = 0;
        if (wid & 15) wid += 16 - (wid & 15), hei = 0;
        
        HBITMAP hBitmap = 0, hOldBitmap = 0;
        void*   lpBits = nullptr;
        HDC     hMemDC = 0, hdc = 0;
        HCURSOR hAlphaCursor = 0;
        
        int swWidth = 0, swHeight = 0;
        BITMAPV5HEADER bi{ 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0,
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}},
            0, 0, 0, 0, 0, 0, 0 };
        
        swWidth  = wid;   // width of cursor
        swHeight = wid;   // height of cursor

        bi.bV5Size        = sizeof(BITMAPV5HEADER);
        bi.bV5Width       = swWidth;
        bi.bV5Height      = swHeight;
        bi.bV5Planes      = 1;
        bi.bV5BitCount    = 32;
        bi.bV5Compression = BI_BITFIELDS;
        // The following mask specification specifies a supported 32 BPP
        // alpha format for Windows XP.
        bi.bV5AlphaMask   =  0xff000000; 
        bi.bV5RedMask     =  0x00ff0000;
        bi.bV5GreenMask   =  0x0000ff00;
        bi.bV5BlueMask    =  0x000000ff;

        hdc = ::GetDC (0);
        if (!hdc) return false;

        // Create the DIB section with an alpha channel.
        hBitmap = ::CreateDIBSection (hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS,
            reinterpret_cast<void**>(&lpBits), 0, 0);
        if (!hBitmap) {
            ::ReleaseDC (0, hdc);
            return false;
        }

        hMemDC = ::CreateCompatibleDC (hdc);
        ::ReleaseDC (0, hdc);
        if (!hMemDC) {
            ::DeleteObject (hBitmap);
            return false;
        }

        // Do a copy on the DIB section.
        hOldBitmap = static_cast<HBITMAP>(::SelectObject (hMemDC, hBitmap));
        mcl_imagebuf_t* src = mcl_get_surface_dataplus(&surface);
        ::BitBlt (hMemDC, 0, 0, swWidth, swHeight, src -> m_hdc, 0, 0, SRCCOPY);
        ::SelectObject (hMemDC, hOldBitmap);
        ::DeleteDC (hMemDC);

        // Create an empty mask bitmap.
        HBITMAP hMonoBitmap = ::CreateBitmap (swWidth, swHeight, 1, 1, NULL);
        if (!hMonoBitmap) {
            ::DeleteObject (hBitmap);
            return false;
        }

        // Reset the alpha values for each pixel in the cursor if
        // semi-transparent is not required.
        if (!surface.get_flags ()) {
            int    it = swWidth * swHeight;
            DWORD* lpdwPixel = reinterpret_cast<DWORD*>(lpBits);
            while (it) {
                *lpdwPixel |= 0xff000000;
                ++ lpdwPixel; -- it;
            }
        }

        ICONINFO ii{ 0, 0, 0, 0, 0 };
        ii.fIcon    = FALSE;  // Change fIcon to TRUE to create an alpha icon
        ii.xHotspot = static_cast<DWORD>(hotspot.x);
        ii.yHotspot = static_cast<DWORD>(hotspot.y);
        ii.hbmMask  = hMonoBitmap;
        ii.hbmColor = hBitmap;

        // Create the alpha cursor with the alpha DIB section.
        hAlphaCursor = ::CreateIconIndirect (&ii);
        ::DeleteObject (hBitmap);
        ::DeleteObject (hMonoBitmap);
        if (!hAlphaCursor) return false;

        mcl_control_obj.hcursor = hAlphaCursor;
        mcl_control_obj.nCursorType = 3;
        ::SendMessageW (mcl_control_obj.hwnd, WM_SETCURSOR, 0, HTCLIENT);
        return true;
    }

    /**
     * @function mcl_mouse_t::get_cursor <src/mouse.h>
     * @brief Get the current mouse cursor
     * @return cursor_t
     */
    cursor_t mcl_mouse_t::
    get_cursor () noexcept{
        cursor_t cur;
        cur.m_dataplus_ = reinterpret_cast<void*>(mcl_control_obj.hcursor);
        cur.m_data_[0]  = mcl_control_obj.nCursorType;
        return cur;
    }

}
