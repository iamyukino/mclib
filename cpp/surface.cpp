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

    @file cpp/surface.cpp
    This is a C++11 implementation file for representing any image.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/surface.h"
#include "../src/clog4m.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    /**
     * @function mcl_imagebuf_t::init <src/surface.cpp>
     * @brief initialize buffer for surface.
     * @return bool
     */
    bool mcl_imagebuf_t::
    init () noexcept {
        // get global dc
        HDC refdc = nullptr;
        if (mcl_control_obj.bIsReady)
            refdc = ::GetDC (mcl_control_obj.hwnd);

        // create a new dc
        m_hdc = ::CreateCompatibleDC (refdc);
        if (!m_hdc) {
            clog4m[cll4m.Warn] << L"surface.init()\n"
                L"    warning:  CreateCompatibleDC Failed. [-Wsurface-winapi-"
                << ::GetLastError() << L"]\n";
            if (refdc)
                ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return false;
        }
        
        // create a new bitmap
        BITMAPINFO bmi = { {0} };
        bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
        bmi.bmiHeader.biWidth = m_width;
        bmi.bmiHeader.biHeight = -1 - m_height;
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biSizeImage = m_width * m_height * 4u;

        m_hbmp = ::CreateDIBSection (
            nullptr, &bmi,
            DIB_RGB_COLORS,
            reinterpret_cast<void**>(&m_pbuffer),
            nullptr, 0u
        );
        if (!m_hbmp) {
            clog4m[cll4m.Warn] << L"surface.init()\n"
                L"    warning:  CreateDIBSection Failed. [-Wsurface-winapi-"
                << ::GetLastError() << L"]\n";
            ::DeleteDC (m_hdc);
            return false;
        }
        ::SelectObject (m_hdc, m_hbmp);
        
        // init the bitmap
        if (!::SetBkMode (m_hdc, TRANSPARENT))
            clog4m[cll4m.Info] << L"surface.init()\n"
                L"    warning:  SetBkMode Failed. [-Wsurface-winapi-"
            << ::GetLastError () << L"]\n";
        this -> cleardevice ();

        return true;
    }

    void mcl_imagebuf_t::
    uninit () noexcept {
        m_width = 0;
        ::DeleteDC (m_hdc);
        ::DeleteObject (m_hbmp);
    }

    /**
     * @function mcl_imagebuf_t::cleardevice <src/surface.cpp>
     * @brief fill buffer with bkcolor.
     * @return none
     */
    void mcl_imagebuf_t::
    cleardevice () noexcept {
        for (color_t* p = m_pbuffer,
            *e = m_pbuffer + static_cast<long long>(m_width) * m_height;
            p != e; ++p) {
                *p = m_bkcolor;
        }
    }

    /**
     * @function mcl_imagebuf_t::mcl_imagebuf_t <src/surface.cpp>
     * @brief Constructor.
     * @return none
     */
    mcl_imagebuf_t::mcl_imagebuf_t (point1d_t width, point1d_t height) noexcept
    : m_width (width > 0 ? width : 1), m_height (height > 0 ? height : 1) {
        if (!this -> init ()) m_width = 0;
    }

    /**
     * @function mcl_imagebuf_t::~mcl_imagebuf_t <src/surface.cpp>
     * @brief Destructor.
     * @return none
     */
    mcl_imagebuf_t::~mcl_imagebuf_t() noexcept {
        if (m_width) this -> uninit ();
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Constructor.
     * @param[in] b_srcalpha: True if the pixel format need
     *     including a per-pixel alpha
     * @return none
     */
    surface_t::
    surface_t (void*, bool b_srcalpha) noexcept
        : m_dataplus_ (nullptr), m_data_ { b_srcalpha }{ }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Constructor.
     * @param[in] size: Surface size
     * @param[in] b_srcalpha: True if the pixel format need
     *     including a per-pixel alpha.
     * @return none
     */
    surface_t::
    surface_t (point2d_t size, bool b_srcalpha) noexcept
        : m_dataplus_ (new(std::nothrow) mcl_imagebuf_t(size.x, size.y)),
        m_data_{ b_srcalpha } {
            if (m_dataplus_ &&
                !static_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width) {
                    delete m_dataplus_;
                    m_dataplus_ = nullptr;
            }
    }

    /**
     * @function surface_t::~surface_t <src/surface.h>
     * @brief Destructor.
     * @return none
     */
    surface_t::~surface_t() noexcept {
        if (m_dataplus_) {
            delete m_dataplus_;
            m_dataplus_ = nullptr;
        }
    }

    /**
     * @function surface_t::operator void* <src/surface.h>
     * @return bool
     */
    surface_t::
    operator void* () const noexcept {
        return m_dataplus_ ? const_cast<surface_t*>(this) : 0;
    }

    /**
     * @function surface_t::operator! <src/surface.h>
     * @return bool
     */
    bool surface_t::
    operator! () const noexcept {
        return !m_dataplus_;
    }

}
