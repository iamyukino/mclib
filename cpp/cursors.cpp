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
        std::vector<unsigned char> andmasks{}, xormasks{};
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
    cursor_t::cursor_t() noexcept
        : m_dataplus_(0), m_data_{ 0 } {}
    cursor_t::
    operator void* () const noexcept{
        return m_dataplus_ ?
            const_cast<cursor_t*>(this) : 0;
    }
    bool cursor_t::
    operator! () const noexcept{
        return !m_dataplus_;
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
