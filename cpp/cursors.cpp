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

    @file cpp/cursors.cpp
    This is a C++11 implementation file for
    cursor resources.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "mcl_control.h"

#include "../src/pyobj.h"
#include "../src/cursors.h"
#include "../src/surface.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for cursor resources.
    mcl_cursors_t cursors;

    mcl_cursors_t::
    operator void* () const noexcept{
        return mcl_control_obj.bIsReady ?
            const_cast<mcl_cursors_t*>(this) : 0;
    }
    bool mcl_cursors_t::
    operator! () const noexcept{
        return !mcl_control_obj.bIsReady;
    }

#if __cplusplus < 201703L
    sys_cursor_t constexpr mcl_cursors_t::Arrow;
    sys_cursor_t constexpr mcl_cursors_t::Ibeam;
    sys_cursor_t constexpr mcl_cursors_t::Wait;
    sys_cursor_t constexpr mcl_cursors_t::CrossHair;
    sys_cursor_t constexpr mcl_cursors_t::WaitArrow;

    sys_cursor_t constexpr mcl_cursors_t::SizeNWSE;
    sys_cursor_t constexpr mcl_cursors_t::SizeNESW;
    sys_cursor_t constexpr mcl_cursors_t::SizeWE;
    sys_cursor_t constexpr mcl_cursors_t::SizeNS;
    sys_cursor_t constexpr mcl_cursors_t::SizeAll;

    sys_cursor_t constexpr mcl_cursors_t::No;
    sys_cursor_t constexpr mcl_cursors_t::Hand;
    sys_cursor_t constexpr mcl_cursors_t::UpArrow;
    sys_cursor_t constexpr mcl_cursors_t::Help;
    sys_cursor_t constexpr mcl_cursors_t::Pin;
    sys_cursor_t constexpr mcl_cursors_t::Person;
#endif

    /**
     * @function mcl_cursors_t::compile <src/cursors.h>
     * @brief create binary cursor data from simple strings.
     * @return parm of mcl::mouse.set_cursor()
     */
    pytuple< std::vector<unsigned char>, std::vector<unsigned char> >
    mcl_cursors_t::
    compile (char const* strings, char cblack, char cwhite, char cxor) noexcept
    {
        std::vector<unsigned char> andmasks, xormasks;
        char const *str = strings;
        int len = 0, qlen = 1, it = 0;
        unsigned char andbit = 0, xorbit = 0;

        while (*str) ++ str;
        len = static_cast<int>(str - strings);
        if (!len || len & 0x3f) // have incomplete byte
            return maktuple (std::vector<unsigned char>(), std::vector<unsigned char>());
        str = strings;
        
        len >>= 6;
        while (len > 0) len -= qlen, qlen += 2;
        if (len) // not perfect square
            return maktuple (std::vector<unsigned char>(), std::vector<unsigned char>());
        -- qlen; qlen >>= 1; // width = qlen

        while (*str) {
            andbit <<= 1; xorbit <<= 1;
            if (*str != cblack && *str != cwhite)
                ++ andbit;
            if (*str == cwhite || *str == cxor)
                ++ xorbit;
            ++ it, ++ str;
            
            if (it == 8) {
                andmasks.push_back (andbit);
                xormasks.push_back (xorbit);
                andbit = 0, xorbit = 0, it = 0;

                if (qlen & 1) {
                    if (++ len == qlen) {
                        len = 0;
                        andmasks.push_back (255);
                        xormasks.push_back (0);
                    }    
                }
            }
        }
        if (qlen & 1) {
            ++ qlen;
            for (it = qlen << 3; it; -- it) {
                andmasks.push_back (255);
                xormasks.push_back (0);
            }
        }

        return maktuple (xormasks, andmasks);
    }


    // cursor type
    inline mcl_simpletls_ns::mcl_shared_ptr_t<HCURSOR>&
        mcl_get_ptr4cur (void* m_dataplus) noexcept {
        return *reinterpret_cast<mcl_simpletls_ns::
            mcl_shared_ptr_t<HCURSOR>*>(m_dataplus);
    }
    cursor_t::
    operator void* () const noexcept{
        return m_dataplus_ && mcl_get_ptr4cur (m_dataplus_).ptr() ?
            const_cast<cursor_t*>(this) : 0;
    }
    bool cursor_t::
    operator! () const noexcept{
        return !(m_dataplus_ && mcl_get_ptr4cur(m_dataplus_).ptr());
    }

    /**
     * @function cursor_t::constructor <src/mouse.h>
     * @brief Set the mouse cursor to a new cursor
     * @return none
     */
    cursor_t::cursor_t () noexcept
        : m_dataplus_(0), m_data_{0} { }

    cursor_t::cursor_t (sys_cursor_t constant) noexcept
        : m_dataplus_(0), m_data_{1} {
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
            default:                  return ;
        }
        hcur = ::LoadCursor (0, sys_cur);
        if (!hcur) return ;

        m_dataplus_ = new (std::nothrow)
            mcl_simpletls_ns::mcl_shared_ptr_t<HCURSOR>(hcur, [] { return; });
    }

    cursor_t::cursor_t (point2d_t size, point2d_t hotspot,
            unsigned char const* xormasks, unsigned char const* andmasks) noexcept
        : m_dataplus_(0), m_data_{2} {
        if (size.x != size.y || size.x & 7) return ;
        if (size.x & 15) size.x += 8;

        HCURSOR hcur = ::CreateCursor (
            nullptr, hotspot.x, hotspot.y, size.x, size.x, andmasks, xormasks);
        if (!hcur) return ;

        m_dataplus_ = new (std::nothrow)
            mcl_simpletls_ns::mcl_shared_ptr_t<HCURSOR>(hcur, [hcur] { ::DestroyCursor (hcur); });
    }

    cursor_t::cursor_t (point2d_t size, point2d_t hotspot,
        pytuple< std::vector<unsigned char>, std::vector<unsigned char> >&&
        xor_and_masks) noexcept
        : m_dataplus_(0), m_data_{2} {
        std::vector<unsigned char> cx = xor_and_masks[0], ca = xor_and_masks[1];
        if (!cx.size() || size.x != size.y || size.x & 7) return ;
        if (size.x & 15) size.x += 8;

        HCURSOR hcur = ::CreateCursor (
            nullptr, hotspot.x, hotspot.y, size.x, size.x, &ca[0], &cx[0]);
        if (!hcur) return ;

        m_dataplus_ = new (std::nothrow)
            mcl_simpletls_ns::mcl_shared_ptr_t<HCURSOR>(hcur, [hcur] { ::DestroyCursor (hcur); });
    }

    cursor_t::cursor_t (point2d_t hotspot, surface_t surface) noexcept
        : m_dataplus_(0), m_data_{3} {
        point1d_t wid = surface.get_width (), hei = surface.get_height ();
        if (!(wid && hei)) return ;

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
        if (!hdc) return ;

        // Create the DIB section with an alpha channel.
        hBitmap = ::CreateDIBSection (hdc, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS,
            reinterpret_cast<void**>(&lpBits), 0, 0);
        if (!hBitmap) {
            ::ReleaseDC (0, hdc);
            return ;
        }

        hMemDC = ::CreateCompatibleDC (hdc);
        ::ReleaseDC (0, hdc);
        if (!hMemDC) {
            ::DeleteObject (hBitmap);
            return ;
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
            return ;
        }

        // Reset the alpha values for each pixel in the cursor if
        // semi-transparent is not required.
        if (!surface.get_flags ()) {
            int it = 0;
            DWORD* lpdwPixel = reinterpret_cast<DWORD*>(lpBits);
            
            if (hei) { // never resized
                for (it = swWidth * swHeight; it; ++ lpdwPixel, -- it)
                    *lpdwPixel |= 0xff000000;
            } else { // resized
                wid = surface.get_width ();
                hei = surface.get_height ();
                int skipw = swWidth - wid;
                while (-- hei) {
                    for (it = wid; it; ++ lpdwPixel, -- it)
                        *lpdwPixel |= 0xff000000;
                    lpdwPixel += skipw;
                }
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
        if (!hAlphaCursor) return ;

        m_dataplus_ = new (std::nothrow)
            mcl_simpletls_ns::mcl_shared_ptr_t<HCURSOR>(hAlphaCursor,
                [hAlphaCursor] { ::DestroyCursor (hAlphaCursor); });
    }
    cursor_t::cursor_t (cursor_t const& rhs) noexcept
        : m_dataplus_(rhs.m_dataplus_), m_data_{rhs.m_data_[0]} {
        if (m_dataplus_) mcl_get_ptr4cur(m_dataplus_).add ();
    }
    cursor_t::cursor_t (cursor_t&& rhs) noexcept
        : m_dataplus_(rhs.m_dataplus_), m_data_{rhs.m_data_[0]} {
        rhs.m_dataplus_ = 0;
    }
    cursor_t::~cursor_t () noexcept{
        if (m_dataplus_) {
            if (mcl_get_ptr4cur(m_dataplus_).del())
                delete mcl_get_ptr4cur(m_dataplus_);
        }
    }
    cursor_t& cursor_t::operator = (cursor_t const& rhs) noexcept {
        if (m_dataplus_ == rhs.m_dataplus_)
            return *this;
        if (m_dataplus_)
            if (mcl_get_ptr4cur(m_dataplus_).del())
                delete mcl_get_ptr4cur(m_dataplus_);
        m_dataplus_ = rhs.m_dataplus_;
        m_data_[0] = rhs.m_data_[0];
        if (m_dataplus_) mcl_get_ptr4cur(m_dataplus_).add ();
        return *this;
    }
    cursor_t& cursor_t::operator = (cursor_t&& rhs) noexcept {
        if (m_dataplus_ == rhs.m_dataplus_)
            return *this;
        if (m_dataplus_)
            if (mcl_get_ptr4cur(m_dataplus_).del())
                delete mcl_get_ptr4cur(m_dataplus_);
        m_dataplus_ = rhs.m_dataplus_;
        m_data_[0] = rhs.m_data_[0];
        rhs.m_dataplus_ = 0;
        return *this;
    }
    cursor_t& cursor_t::operator=(decltype(nullptr)) noexcept {
        if (m_dataplus_)
            if (mcl_get_ptr4cur(m_dataplus_).del())
                delete mcl_get_ptr4cur(m_dataplus_);
        m_dataplus_ = nullptr;
        return *this;
    }

    /**
     * @function cursor_t::type <src/mouse.h>
     * @brief Gets the cursor type
     * @return string
     */
    wchar_t const* cursor_t::
    type () noexcept{
        switch (m_data_[0]) {
        case 1: return L"system";
        case 2: return L"bitmap";
        case 3: return L"color";
        }
        return L"default";
    }
    char const* cursor_t::
    type_a () noexcept{
        switch (m_data_[0]) {
        case 1: return "system";
        case 2: return "bitmap";
        case 3: return "color";
        }
        return "default";
    }

}
