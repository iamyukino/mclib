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
#include "../src/colors.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Enum class. Color blending operations.
    mcl_blend_t blend;

#if __cplusplus < 201703L
    blend_t constexpr mcl_blend_t::Copy;
    blend_t constexpr mcl_blend_t::Add;
    blend_t constexpr mcl_blend_t::Sub;
    blend_t constexpr mcl_blend_t::Mult;
    blend_t constexpr mcl_blend_t::Min;
    blend_t constexpr mcl_blend_t::Max;
    blend_t constexpr mcl_blend_t::Xor;
    blend_t constexpr mcl_blend_t::Ovl;

    blend_t constexpr mcl_blend_t::Copy_rgb;
    blend_t constexpr mcl_blend_t::Add_rgb;
    blend_t constexpr mcl_blend_t::Sub_rgb;
    blend_t constexpr mcl_blend_t::Mult_rgb;
    blend_t constexpr mcl_blend_t::Min_rgb;
    blend_t constexpr mcl_blend_t::Max_rgb;
    blend_t constexpr mcl_blend_t::Xor_rgb;
    blend_t constexpr mcl_blend_t::Ovl_rgb;

    blend_t constexpr mcl_blend_t::Copy_rgba;
    blend_t constexpr mcl_blend_t::Add_rgba;
    blend_t constexpr mcl_blend_t::Sub_rgba;
    blend_t constexpr mcl_blend_t::Mult_rgba;
    blend_t constexpr mcl_blend_t::Min_rgba;
    blend_t constexpr mcl_blend_t::Max_rgba;
    blend_t constexpr mcl_blend_t::Xor_rgba;
    blend_t constexpr mcl_blend_t::Ovl_rgba;
#endif

    /**
     * @function mcl_imagebuf_t::init <src/surface.cpp>
     * @brief initialize buffer for surface.
     * @return bool
     */
    bool mcl_imagebuf_t::
    init () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lk(m_nrtlock);
        // get global dc
        HDC refdc = nullptr;
        if (mcl_control_obj.bIsReady)
            refdc = mcl_control_obj.dc;

        // create a new dc
        m_hdc = ::CreateCompatibleDC (refdc);
        if (!m_hdc) {
            clog4m[cll4m.Warn] << L"surface.init()\n"
                L"    warning:  CreateCompatibleDC Failed. [-Wsurface-winapi-"
                << ::GetLastError() << L"]\n";
            return false;
        }
        
        // create a new bitmap
        BITMAPINFO bmi = { {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, {{0, 0, 0, 0}} };
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

        return true;
    }

    void mcl_imagebuf_t::
    uninit () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lk(m_nrtlock);
        if (m_width) {
            m_width = 0;
            ::DeleteDC (m_hdc);
            ::DeleteObject (m_hbmp);
        }
    }

    /**
     * @function mcl_imagebuf_t::cleardevice <src/surface.cpp>
     * @brief fill buffer with bkcolor.
     * @return none
     */
    void mcl_imagebuf_t::
    cleardevice (color_t c) noexcept {
        for (color_t* p = m_pbuffer,
            *e = m_pbuffer + static_cast<long long>(m_width) * m_height;
            p != e; ++p) {
                *p = c;
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
        if (!m_dataplus_) return ;
        if (!static_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width) {
            delete static_cast<mcl_imagebuf_t*>(m_dataplus_);
            m_dataplus_ = nullptr;
            return ;
        }
        static_cast<mcl_imagebuf_t*>(m_dataplus_)
            -> cleardevice (black);
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Copy constructor.
     * @retrun none
     */
    surface_t::
    surface_t (surface_t const& src) noexcept
      : m_dataplus_ (&src && src.m_dataplus_ ?
          new(std::nothrow) mcl_imagebuf_t(
              static_cast<mcl_imagebuf_t*>(src.m_dataplus_) -> m_width,
              static_cast<mcl_imagebuf_t*>(src.m_dataplus_) -> m_height
          ) : 0
      ), m_data_{ &src && src.m_data_[0] } {
        if (!m_dataplus_) return ;
        mcl_imagebuf_t* dsrc = static_cast<mcl_imagebuf_t*>(src.m_dataplus_);
        mcl_imagebuf_t* ddst = static_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!(dsrc && dsrc -> m_width)) {
            static_cast<mcl_imagebuf_t*>(m_dataplus_)
                -> cleardevice (black);
            return;
        }
        if (!(ddst && ddst -> m_width)) {
            delete static_cast<mcl_imagebuf_t*>(m_dataplus_);
            m_dataplus_ = nullptr;
            return ;
        }
        ::BitBlt (ddst -> m_hdc, 0, 0, ddst -> m_width, ddst -> m_height,
            dsrc -> m_hdc, 0, 0, SRCCOPY);
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Copy constructor.
     * @param b_srcalpha: True if the pixel format need
     * @retrun none
     */
    surface_t::
    surface_t (surface_t const& src, bool b_srcalpha) noexcept
      : surface_t (src) {
        m_data_[0] = b_srcalpha;
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Move constructor.
     * @retrun none
     */
    surface_t::
    surface_t (surface_t&& src) noexcept
      : m_dataplus_ (src ? src.m_dataplus_ : 0),
        m_data_{ src && src.m_data_[0] } {
        if (src) src.m_dataplus_ = nullptr;
    }
    
    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Move constructor.
     * @param b_srcalpha: True if the pixel format need
     * @retrun none
     */
    surface_t::
    surface_t (surface_t&& src, bool b_srcalpha) noexcept
      : surface_t (std::move(src)) {
        m_data_[0] = b_srcalpha;
    }
    
    /**
     * @function surface_t::~surface_t <src/surface.h>
     * @brief Destructor.
     * @return none
     */
    surface_t::~surface_t() noexcept {
        if (m_dataplus_) {
            mcl_imagebuf_t* p = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
            m_dataplus_ = nullptr;
            delete p;
        }
    }
    
    /**
     * @function surface_t::operator= <src/surface.h>
     * @brief Copy assignment operator.
     * @return none
     */
    surface_t& surface_t::
    operator= (surface_t const& rhs) noexcept {
        if (!(&rhs && this)) return *this;
        
        m_data_[0] = rhs.m_data_[0];
        if (m_dataplus_ == rhs.m_dataplus_ || !rhs.m_dataplus_)
            return *this;
        
        if (m_dataplus_)
            delete static_cast<mcl_imagebuf_t*>(m_dataplus_);
        
        mcl_imagebuf_t* dsrc = static_cast<mcl_imagebuf_t*>(rhs.m_dataplus_);
        m_dataplus_ = new(std::nothrow)
            mcl_imagebuf_t(dsrc -> m_width, dsrc -> m_height);
        if (!m_dataplus_) return *this;

        mcl_imagebuf_t* ddst = static_cast<mcl_imagebuf_t*>(m_dataplus_); 
        mcl_simpletls_ns::mcl_spinlock_t lk (ddst -> m_nrtlock);
        if (!ddst -> m_width) {
            delete static_cast<mcl_imagebuf_t*>(m_dataplus_);
            m_dataplus_ = nullptr;
            return *this;
        }

        ::BitBlt (ddst -> m_hdc, 0, 0, ddst -> m_width, ddst -> m_height,
            dsrc -> m_hdc, 0, 0, SRCCOPY);
        return *this;
    }
    
    /**
     * @function surface_t::operator= <src/surface.h>
     * @brief Move assignment operator.
     * @return none
     */
    surface_t& surface_t::
    operator= (surface_t&& rhs) noexcept {
        if (!(&rhs && this)) return *this;

        m_data_[0] = rhs.m_data_[0];
        if (m_dataplus_ == rhs.m_dataplus_ || !rhs.m_dataplus_)
            return *this;
        
        if (m_dataplus_)
            delete static_cast<mcl_imagebuf_t*>(m_dataplus_);
        
        m_dataplus_ = rhs.m_dataplus_;
        rhs.m_dataplus_ = nullptr;
        return *this;
    }

    /**
     * @function surface_t::operator void* <src/surface.h>
     * @return bool
     */
    surface_t::
    operator void* () const noexcept {
        return this && m_dataplus_ ?
            const_cast<surface_t*>(this) : 0;
    }

    /**
     * @function surface_t::operator! <src/surface.h>
     * @return bool
     */
    bool surface_t::
    operator! () const noexcept {
        return !(this && m_dataplus_);
    }

    /**
     * @function surface_t::get_size <src/surface.h>
     * @return point2d_t
     */
    point2d_t surface_t::
    get_size () const noexcept {
        return this && m_dataplus_ ? point2d_t {
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width,
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_height
        }: point2d_t {0, 0};
    }
    
    /**
     * @function surface_t::get_width <src/surface.h>
     * @return point1d_t
     */
    point1d_t surface_t::
    get_width () const noexcept {
        return this && m_dataplus_ ?
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width : 0;
    }
    
    /**
     * @function surface_t::get_height <src/surface.h>
     * @return point1d_t
     */
    point1d_t surface_t::
    get_height () const noexcept {
        return this && m_dataplus_ ?
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_height : 0;
    }

    /**
     * @function surface_t::get_rect <src/surface.h>
     * @return rect_t
     */
    rect_t surface_t::
    get_rect (void* ) const noexcept {
        if (!(this && m_dataplus_)) return { 0, 0, 0, 0 }; // display surface quit
        point1d_t rw = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_)->m_width;
        point1d_t rh = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_)->m_height;
        return { 0, 0, rw, rh };
    }

    /**
     * @function surface_t::get_rect <src/surface.h>
     * @param center: position that surface centered at
     * @return rect_t
     */
    rect_t surface_t::
    get_rect (point2d_t center) const noexcept {
        if (!(this && m_dataplus_)) // display surface quit
            return { center.x, center.y, 0, 0 };
        point1d_t rw = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width;
        point1d_t rh = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_height;
        return { center.x - (rw >> 1), center.y - (rh >> 1), rw, rh };
    }

    /**
     * @function surface_t::lock <src/surface.h>
     * @return surface_t&
     */
    surface_t& surface_t::
    lock () noexcept {
        if (!this) return *this;
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return *this; // display surface quit
        mcl_simpletls_ns::mcl_lock (dataplus -> m_nrtlock, dataplus -> m_nrt_count);
        return *this;
    }
    
    /**
     * @function surface_t::unlock <src/surface.h>
     * @return surface_t&
     */
    surface_t& surface_t::
    unlock () noexcept {
        if (!this) return *this;
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return *this; // display surface quit
        mcl_simpletls_ns::mcl_unlock (dataplus -> m_nrtlock, dataplus -> m_nrt_count);
        return *this;
    }
    
    /**
     * @function surface_t::get_locked <src/surface.h>
     * @return bool
     */
    bool surface_t::
    get_locked () const noexcept {
        return this && m_dataplus_ && reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_nrtlock;
    }
    
    /**
     * @function surface_t::get_at <src/surface.h>
     * @brief Get the color value at a single pixel
     * @param pos
     * @return color_t
     */
    color_t surface_t::
    get_at (point2d_t pos) const noexcept {
        if (!this) return opaque;
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return opaque; // display surface quit
        if (pos.x < 0 || pos.y < 0) return opaque;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock);
        if (pos.x >= dataplus -> m_width || pos.y >= dataplus -> m_height)
            return opaque; // out of clip area
        
        return dataplus -> m_pbuffer[pos.x + dataplus -> m_width * pos.y];
    }
    
    /**
     * @function surface_t::set_at <src/surface.h>
     * @brief Set the color value at a single pixel
     * @param pos
     * @param color
     * @return color_t
     */
    color_t surface_t::
    set_at (point2d_t pos, color_t color) noexcept {
        if (!this) return opaque;
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return opaque; // display surface quit
        if (pos.x < 0 || pos.y < 0) return opaque;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock);
        if (pos.x >= dataplus -> m_width || pos.y >= dataplus -> m_height)
            return opaque; // out of clip area

        return (dataplus -> m_pbuffer[pos.x + dataplus -> m_width * pos.y] = color);
    }
    
    /**
     * @function surface_t::get_at <src/surface.h>
     * @brief Get the additional flags used for the Surface
     * @return bool: true if srcalpha is set
     */
    bool surface_t::
    get_flags () const noexcept {
        return this && m_dataplus_ && m_data_[0];
    }

    /**
     * @function surface_t::_pixels_address <src/surface.h>
     */
    color_t* surface_t::
    _pixels_address () noexcept{
        return this && m_dataplus_ ?
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_pbuffer
            : nullptr;
    }

    /**
     * @function mcl_switch_blend_fun_fill <src/surface.cpp>
     * @brief Choose blend function. for surface.fill
     * @param[out] blend_func: blend function
     * @param[in] color: src color value
     * @param[in] special_flags: special flags
     * @param[in] m_data_
     * @return bool: true if failed
     */
    static bool mcl_switch_blend_fun_fill(
        std::function<void(color_t&)>& blend_fun,
        color_t color, blend_t special_flags, char* m_data_
    ) {
        if (!(special_flags & 0x300))
            special_flags |= (m_data_[0] ? 0x200 : 0x100);
        else if ((special_flags & 0x200) && !m_data_[0]) {
            // the alpha is ignored unless the surface uses per pixel alpha
            special_flags = (special_flags & ~0x300) | 0x100;
        }
        // choose blend function
        switch (special_flags) {
            // copy
            case mcl_blend_t::Copy_rgb: {
                if (m_data_[0]) {
                    color &= 0xffffff;
                    blend_fun = [color](color_t& dst) {
                        dst = color | (dst & 0xff000000);
                    };
                    break;
                }
                MCL_FALLTHROUGH_CXX17
            }
            case mcl_blend_t::Copy_rgba: {
                blend_fun = [color](color_t& dst) { dst = color; };
                break;
            }
            // darken
            case mcl_blend_t::Min_rgb: {
                color &= 0xffffff;
                if (m_data_[0]) {
                    blend_fun = [color](color_t& dst) {
                        if ((dst & 0xffffff) > color)
                            dst = color | (dst & 0xff000000);
                    };
                    break;
                } 
                blend_fun = [color](color_t& dst) {
                    if ((dst & 0xffffff) > color) dst = color;
                };
                break;
            }
            case mcl_blend_t::Min_rgba: {
                blend_fun = [color](color_t& dst) {
                    if (dst > color) dst = color;
                };
                break;
            }
            // lighten
            case mcl_blend_t::Max_rgb: {
                color &= 0xffffff;
                if (m_data_[0]) {
                    blend_fun = [color](color_t& dst) {
                        if ((dst & 0xffffff) < color)
                            dst = color | (dst & 0xff000000);
                    };
                    break;
                }
                blend_fun = [color](color_t& dst) {
                    if ((dst & 0xffffff) < color)
                        dst = color;
                };
                break;
            }
            case mcl_blend_t::Max_rgba: {
                blend_fun = [color](color_t& dst) {
                    if (dst < color) dst = color;
                };
                break;
            }
            // color dodge
            case mcl_blend_t::Add_rgb: {
                color_t const r1 = getr4rgb(color);
                color_t const g1 = getg4rgb(color);
                color_t const b1 = color & 0xff;
                if (m_data_[0]) {
                    blend_fun = [r1, g1, b1](color_t& dst) {
                        color_t r2 = getr4rgb(dst);
                        color_t g2 = getg4rgb(dst);
                        color_t b2 = dst & 0xff;
                        if ((r2 + 0x1) & 0xff) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 & ~0xff) r2 = 255; }
                        if ((g2 + 0x1) & 0xff) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 & ~0xff) g2 = 255; }
                        if ((b2 + 0x1) & 0xff) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 & ~0xff) b2 = 255; }
                        dst = (dst & 0xff000000) | (r2 << 16) | (g2 << 8) | b2;
                    };
                    break;
                }
                blend_fun = [r1, g1, b1](color_t& dst) {
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = dst & 0xff;
                    if ((r2 + 1) & 0xff) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 & ~0xff) r2 = 255; }
                    if ((g2 + 1) & 0xff) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 & ~0xff) g2 = 255; }
                    if ((b2 + 1) & 0xff) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 & ~0xff) b2 = 255; }
                    dst = (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            case mcl_blend_t::Add_rgba: {
                color_t const a1 = color >> 24;
                color_t const r1 = getr4rgb(color);
                color_t const g1 = getg4rgb(color);
                color_t const b1 = getb4rgb(color);
                blend_fun = [a1, r1, g1, b1](color_t& dst) {
                    color_t a2 = dst >> 24;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    if (a2 != 255) { a2 = a1 + a1 * a2 / (255 - a2); if (a2 > 255) a2 = 255; }
                    if (r2 != 255) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 > 255) r2 = 255; }
                    if (g2 != 255) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 > 255) g2 = 255; }
                    if (b2 != 255) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 > 255) b2 = 255; }
                    dst = (a2 << 24) | (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            // color burn
            case mcl_blend_t::Sub_rgb: {
                color_t const r1 = getr4rgb(color) + 0xfffff;
                color_t const g1 = getg4rgb(color) + 0xfffff;
                color_t const b1 = getb4rgb(color) + 0xfffff;
                color_t const pr1 = 255 + 0xfffff - r1;
                color_t const pg1 = 255 + 0xfffff - g1;
                color_t const pb1 = 255 + 0xfffff - b1;
                if (m_data_[0]) {
                    blend_fun = [r1, g1, b1, pr1, pg1, pb1](color_t& dst) {
                        color_t r2 = getr4rgb(dst);
                        color_t g2 = getg4rgb(dst);
                        color_t b2 = getb4rgb(dst);
                        if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                        if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                        if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                        r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                        g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                        b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                        dst = (dst & 0xff000000) | r2 | g2 | b2;
                    };
                    break;
                }
                blend_fun = [r1, g1, b1, pr1, pg1, pb1](color_t& dst) {
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                    if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                    if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                    r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                    g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                    b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                    dst = r2 | g2 | b2;
                };
                break;
            }
            case mcl_blend_t::Sub_rgba: {
                color_t const a1 = (color >> 24)   + 0xfffff;
                color_t const r1 = getr4rgb(color) + 0xfffff;
                color_t const g1 = getg4rgb(color) + 0xfffff;
                color_t const b1 = getb4rgb(color) + 0xfffff;
                color_t const pa1 = 255 + 0xfffff - a1;
                color_t const pr1 = 255 + 0xfffff - r1;
                color_t const pg1 = 255 + 0xfffff - g1;
                color_t const pb1 = 255 + 0xfffff - b1;
                blend_fun = [a1, r1, g1, b1, pa1, pr1, pg1, pb1](color_t& dst) {
                    color_t a2 = dst >> 24;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    if (a2 != 0) a2 = a1 - pa1 * (255 - a2) / a2;
                    if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                    if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                    if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                    a2 = (a2 < 0xfffff ? 0 : (a2 - 0xfffff) << 24);
                    r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                    g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                    b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                    dst = a2 | r2 | g2 | b2;
                };
                break;
            }
            // multiply
            case mcl_blend_t::Mult_rgb: {
                color_t const r1 = getr4rgb (color);
                color_t const g1 = getg4rgb (color);
                color_t const b1 = getb4rgb (color);
                if (m_data_[0]) {
                    blend_fun = [r1, g1, b1](color_t& dst) {
                        color_t r2 = getr4rgb(dst);
                        color_t g2 = getg4rgb(dst);
                        color_t b2 = getb4rgb(dst);
                        dst = (dst & 0xff000000) | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                    };
                    break;
                }
                blend_fun = [r1, g1, b1](color_t& dst) {
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    dst = (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            case mcl_blend_t::Mult_rgba: {
                color_t const a1 = color >> 24;
                color_t const r1 = getr4rgb (color);
                color_t const g1 = getg4rgb (color);
                color_t const b1 = getb4rgb (color);
                blend_fun = [a1, r1, g1, b1](color_t& dst) {
                    color_t a2 = dst >> 24;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    dst = (a1 * a2 / 255 << 24) | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            // xor
            case mcl_blend_t::Xor_rgb: {
                if (m_data_[0]) {
                    color &= 0xffffff;
                    blend_fun = [color](color_t& dst) {
                        dst = ((dst & 0xffffff) ^ color) | (dst & 0xff000000);
                    };
                    break;
                }
                MCL_FALLTHROUGH_CXX17
            }
            case mcl_blend_t::Xor_rgba: {
                blend_fun = [color](color_t& dst) { dst ^= color; };
                break;
            }
            // overlay
            case mcl_blend_t::Ovl_rgb: {
                color_t sa = color >> 24;
                if (0xff == sa) {
                // no alpha
                    blend_fun = [color](color_t& dst) { dst = color; };
                    break;
                }
                // have alpha
                color_t srsa = getr4rgb(color) * sa;
                color_t sgsa = getg4rgb(color) * sa;
                color_t sbsa = getb4rgb(color) * sa;
                color_t da   = 255 - sa;
                if (m_data_[0]) {
                    blend_fun = [srsa, sgsa, sbsa, da](color_t& dst) {
                        color_t r = (getr4rgb(dst) * da + srsa) / 255;
                        color_t g = (getg4rgb(dst) * da + sgsa) / 255;
                        color_t b = (getb4rgb(dst) * da + sbsa) / 255;
                        dst = (dst & 0xff000000) | (r << 16) | (g << 8) | b;
                    };
                    break;
                }
                blend_fun = [srsa, sgsa, sbsa, da](color_t& dst) {
                    color_t r = (getr4rgb(dst) * da + srsa) / 255;
                    color_t g = (getg4rgb(dst) * da + sgsa) / 255;
                    color_t b = (getb4rgb(dst) * da + sbsa) / 255;
                    dst = (r << 16) | (g << 8) | b;
                };
                break;
            }
            case mcl_blend_t::Ovl_rgba: {
                color_t sa = color >> 24;
                if (0xff == sa) { // no alpha
                    blend_fun = [color](color_t& dst) { dst = color; };
                    break;
                }
                // have alpha
                color_t srsa = getr4rgb(color) * sa;
                color_t sgsa = getg4rgb(color) * sa;
                color_t sbsa = getb4rgb(color) * sa;
                color_t psa  = 255 - sa;
                blend_fun = [srsa, sgsa, sbsa, sa, psa](color_t& dst) {
                    color_t da = (dst >> 24) * psa / 255;
                    color_t t = sa + da;
                    color_t r = (getr4rgb(dst) * da + srsa) / t;
                    color_t g = (getg4rgb(dst) * da + sgsa) / t;
                    color_t b = (getb4rgb(dst) * da + sbsa) / t;
                    dst = (t << 24) | (r << 16) | (g << 8) | b;
                };
                break;
            }
            // unknown
            default: return true;
        }
        return false;
    }

    /**
     * @function surface_t::fill <src/surface.h>
     * @brief Fill surface_t with a solid color
     * @param[in] color: color to fill with
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    fill (color_t color, void*, blend_t special_flags) noexcept {
        if (!this) return { 0, 0, 0, 0 };
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return { 0, 0, 0, 0 }; // display surface quit
        
        // blend flags
        std::function<void(color_t&)> blend_fun;
        if (mcl_switch_blend_fun_fill (blend_fun, color, special_flags, m_data_))
            return { 0, 0, 0, 0 };

        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock);
        if (!dataplus -> m_width)
            return { 0, 0, 0, 0 };

        // start filling
        for (color_t* p = dataplus -> m_pbuffer,
            *e = dataplus -> m_pbuffer
                + static_cast<long long>(dataplus -> m_width) * dataplus -> m_height;
            p != e; ++p) {
                blend_fun (*p);
        }

        return { 0, 0, dataplus -> m_width, dataplus -> m_height };
    }

    /**
     * @function surface_t::fill <src/surface.h>
     * @brief Fill surface_t with a solid color
     * @param[in] color: color to fill with
     * @param[in] recta: rect to fill
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    fill (color_t color, rect_t recta, blend_t special_flags) noexcept {
        if (!this) return { recta.x, recta.y, 0, 0 };
        if (!(recta.w && recta.h))
            return { recta.x, recta.y, 0, 0 }; 
        
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) // display surface quit
            return { recta.x, recta.y, 0, 0 }; 
        
        // choose blend function
        std::function<void(color_t&)> blend_fun;
        if (mcl_switch_blend_fun_fill (blend_fun, color, special_flags, m_data_))
            return { recta.x, recta.y, 0, 0 };

        // impact rect
        point1d_t x = recta.x, y = recta.y, w = recta.w, h = recta.h;
        if (w < 0) x += w + 1, w = -w;
        if (h < 0) y += h + 1, h = -h;
        
        // completely out of window
        if (x + w <= 0 || y + h <= 0)
            return { recta.x, recta.y, 0, 0 };

        // prepare for blending
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock);
        if (!dataplus -> m_width)
            return { recta.x, recta.y, 0, 0 }; 
        
        // completely out of window
        if (x >= dataplus -> m_width || y >= dataplus -> m_height)
            return { recta.x, recta.y, 0, 0 };

        // part of the area beyond window
        if (x + w > dataplus -> m_width)  w = dataplus -> m_width - x;
        if (y + h > dataplus -> m_height) h = dataplus -> m_height - y;
        if (x < 0) w += x, x = 0;
        if (y < 0) h += y, y = 0; 

        // start filling
        color_t *i = dataplus -> m_pbuffer + static_cast<long long>(y) * dataplus -> m_width + x, *j = 0;
        color_t *i0 = static_cast<long long>(h) * dataplus -> m_width + i, *j0 = 0;

        for (; i != i0; i += dataplus -> m_width)
            for (j = i, j0 = i + w; j != j0; ++j)
                blend_fun (*j);

        return { x, y, w, h };
    }

    /**
     * @function mcl_switch_blend_fun_bilt <src/surface.cpp>
     * @brief Choose blend function. for surface.bilt
     * @param[out] blend_func: blend function
     * @param[in] special_flags: special flags
     * @param[in] m_data_
     * @param[in] m_data_src
     * @return bool: true if failed
     */
    static bool mcl_switch_blend_fun_bilt(
        std::function<void(color_t&, color_t)>& blend_fun,
        blend_t special_flags, char* m_data_, char* m_data_src
    ) {
        if (!m_data_src[0]) {
            // the alpha is ignored unless the source surface uses per pixel alpha
            special_flags = (special_flags & ~0x300) | 0x100;
        }
        else if (!(special_flags & 0x300))
            special_flags |= (m_data_[0] ? 0x200 : 0x100);
        else if ((special_flags & 0x200) && !m_data_[0]) {
            // so is the destination
            special_flags = (special_flags & ~0x300) | 0x100;
        }
        // choose blend function
        switch (special_flags) {
            // copy
            case mcl_blend_t::Copy_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        dst = (src & 0xffffff) | (dst & 0xff000000);
                    };
                    break;
                }
                MCL_FALLTHROUGH_CXX17
            }
            case mcl_blend_t::Copy_rgba: {
                blend_fun = [](color_t& dst, color_t src) { dst = src; };
                break;
            }
            // darken
            case mcl_blend_t::Min_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        src &= 0xffffff;
                        if ((dst & 0xffffff) > src)
                            dst = src | (dst & 0xff000000);
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    src &= 0xffffff;
                    if ((dst & 0xffffff) > src)
                        dst = src;
                };
                break;
            }
            case mcl_blend_t::Min_rgba: {
                blend_fun = [](color_t& dst, color_t src) {
                    if (dst > src) dst = src;
                };
                break;
            }
            // lighten
            case mcl_blend_t::Max_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        src &= 0xffffff;                    
                        if ((dst & 0xffffff) < src)
                            dst = src | (dst & 0xff000000);
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    src &= 0xffffff;
                    if ((dst & 0xffffff) < src)
                        dst = src;
                };
                break;
            }
            case mcl_blend_t::Max_rgba: {
                blend_fun = [](color_t& dst, color_t src) {
                    if (dst < src) dst = src;
                };
                break;
            }
            // color dodge
            case mcl_blend_t::Add_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                        color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                        color_t b1 = src & 0xff,    b2 = dst & 0xff;
                        if ((r2 + 0x1) & 0xff) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 & ~0xff) r2 = 255; }
                        if ((g2 + 0x1) & 0xff) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 & ~0xff) g2 = 255; }
                        if ((b2 + 0x1) & 0xff) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 & ~0xff) b2 = 255; }
                        dst = (dst & 0xff000000) | (r2 << 16) | (g2 << 8) | b2;
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = src & 0xff,    b2 = dst & 0xff;
                    if ((r2 + 1) & 0xff) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 & ~0xff) r2 = 255; }
                    if ((g2 + 1) & 0xff) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 & ~0xff) g2 = 255; }
                    if ((b2 + 1) & 0xff) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 & ~0xff) b2 = 255; }
                    dst = (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            case mcl_blend_t::Add_rgba: {
                blend_fun = [](color_t& dst, color_t src) {
                    color_t a1 = src >> 24,     a2 = dst >> 24;
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                    if (a2 != 255) { a2 = a1 + a1 * a2 / (255 - a2); if (a2 > 255) a2 = 255; }
                    if (r2 != 255) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 > 255) r2 = 255; }
                    if (g2 != 255) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 > 255) g2 = 255; }
                    if (b2 != 255) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 > 255) b2 = 255; }
                    dst = (a2 << 24) | (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            // color burn
            case mcl_blend_t::Sub_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        color_t r1 = getr4rgb(src) + 0xfffff;
                        color_t g1 = getg4rgb(src) + 0xfffff;
                        color_t b1 = getb4rgb(src) + 0xfffff;
                        color_t pr1 = 255 + 0xfffff - r1;
                        color_t pg1 = 255 + 0xfffff - g1;
                        color_t pb1 = 255 + 0xfffff - b1;
                        color_t r2 = getr4rgb(dst);
                        color_t g2 = getg4rgb(dst);
                        color_t b2 = getb4rgb(dst);
                        if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                        if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                        if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                        r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                        g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                        b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                        dst = (dst & 0xff000000) | r2 | g2 | b2;
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    color_t r1 = getr4rgb(src) + 0xfffff;
                    color_t g1 = getg4rgb(src) + 0xfffff;
                    color_t b1 = getb4rgb(src) + 0xfffff;
                    color_t pr1 = 255 + 0xfffff - r1;
                    color_t pg1 = 255 + 0xfffff - g1;
                    color_t pb1 = 255 + 0xfffff - b1;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                    if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                    if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                    r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                    g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                    b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                    dst = r2 | g2 | b2;
                };
                break;
            }
            case mcl_blend_t::Sub_rgba: {
                blend_fun = [](color_t& dst, color_t src) {
                    color_t a1 = (src >> 24) + 0xfffff;
                    color_t r1 = getr4rgb(src) + 0xfffff;
                    color_t g1 = getg4rgb(src) + 0xfffff;
                    color_t b1 = getb4rgb(src) + 0xfffff;
                    color_t pa1 = 255 + 0xfffff - a1;
                    color_t pr1 = 255 + 0xfffff - r1;
                    color_t pg1 = 255 + 0xfffff - g1;
                    color_t pb1 = 255 + 0xfffff - b1;
                    color_t a2 = dst >> 24;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    if (a2 != 0) a2 = a1 - pa1 * (255 - a2) / a2;
                    if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                    if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                    if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                    a2 = (a2 < 0xfffff ? 0 : (a2 - 0xfffff) << 24);
                    r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                    g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                    b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                    dst = a2 | r2 | g2 | b2;
                };
                break;
            }
            // multiply
            case mcl_blend_t::Mult_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                        color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                        color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                        dst = (dst & 0xff000000) | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                    dst = (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            case mcl_blend_t::Mult_rgba: {
                blend_fun = [](color_t& dst, color_t src) {
                    color_t a1 = src >> 24,     a2 = dst >> 24;
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                    dst = (a1 * a2 / 255 << 24) | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            // xor
            case mcl_blend_t::Xor_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        dst = ((dst ^ src) & 0xffffff) | (dst & 0xff000000);
                    };
                    break;
                }
                MCL_FALLTHROUGH_CXX17
            }
            case mcl_blend_t::Xor_rgba: {
                blend_fun = [](color_t& dst, color_t src) { dst ^= src; };
                break;
            }
            // overlay
            case mcl_blend_t::Ovl_rgb: {
                if (m_data_[0]) {
                    blend_fun = [](color_t& dst, color_t src) {
                        color_t sa = src >> 24;
                        if (0xff == sa) {
                            dst = src;
                            return ;
                        }
                        color_t srsa = getr4rgb(src) * sa;
                        color_t sgsa = getg4rgb(src) * sa;
                        color_t sbsa = getb4rgb(src) * sa;
                        color_t da = 255 - sa;    
                        color_t r = (getr4rgb(dst) * da + srsa) / 255;
                        color_t g = (getg4rgb(dst) * da + sgsa) / 255;
                        color_t b = (getb4rgb(dst) * da + sbsa) / 255;
                        dst = (dst & 0xff000000) | (r << 16) | (g << 8) | b;
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    color_t sa = src >> 24;
                    if (0xff == sa) {
                        dst = src;
                        return;
                    }
                    color_t srsa = getr4rgb(src) * sa;
                    color_t sgsa = getg4rgb(src) * sa;
                    color_t sbsa = getb4rgb(src) * sa;
                    color_t da = 255 - sa;
                    color_t r = (getr4rgb(dst) * da + srsa) / 255;
                    color_t g = (getg4rgb(dst) * da + sgsa) / 255;
                    color_t b = (getb4rgb(dst) * da + sbsa) / 255;
                    dst = (r << 16) | (g << 8) | b;
                };
                break;
            }
            case mcl_blend_t::Ovl_rgba: {
                blend_fun = [](color_t& dst, color_t src) {
                    color_t sa = src >> 24;
                    if (0xff == sa) { // no alpha
                        dst = src;
                        return ;
                    }
                    // have alpha
                    color_t srsa = getr4rgb(src) * sa;
                    color_t sgsa = getg4rgb(src) * sa;
                    color_t sbsa = getb4rgb(src) * sa;
                    color_t psa = 255 - sa;
                    color_t da = (dst >> 24) * psa / 255;
                    color_t t = sa + da;
                    color_t r = (getr4rgb(dst) * da + srsa) / t;
                    color_t g = (getg4rgb(dst) * da + sgsa) / t;
                    color_t b = (getb4rgb(dst) * da + sbsa) / t;
                    dst = (t << 24) | (r << 16) | (g << 8) | b;
                };
                break;
            }
            // unknown
            default: return true;
        }
        return false;
    }
    
    /**
     * @function surface_t::bilt <src/surface.h>
     * @brief draw one image onto another
     * @param[in] source: source surface
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    bilt (surface_t& source, void*, void*, blend_t special_flags) noexcept{
        if (!this || !&source) return { 0, 0, 0, 0 };
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { 0, 0, 0, 0 }; // display surface quit
        
        // blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        if (mcl_switch_blend_fun_bilt (blend_fun, special_flags, m_data_, source.m_data_))
            return { 0, 0, 0, 0 };
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock);       
        point1d_t w = src -> m_width, h = src -> m_height;
        if (w > dst -> m_width)  w = dst -> m_width;
        if (h > dst -> m_height) h = dst -> m_height;
        if (!w) return { 0, 0, 0, 0 };
        
        // start bilting
        color_t *si = src -> m_pbuffer, *sj = 0;
        color_t *di = dst -> m_pbuffer, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { 0, 0, w, h };
    }

    /**
     * @function surface_t::bilt <src/surface.h>
     * @brief draw one image onto another
     * @param[in] source: source surface
     * @param[in] dest: destination position for the blit
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    bilt (surface_t& source, point2d_t dest, void*, blend_t special_flags) noexcept{
        if (!this || !&source) return { dest.x, dest.y, 0, 0 };
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { dest.x, dest.y, 0, 0 }; // display surface quit
        
        // blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        if (mcl_switch_blend_fun_bilt (blend_fun, special_flags, m_data_, source.m_data_))
            return { dest.x, dest.y, 0, 0 };
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock);
        
        point1d_t sx = 0, sy = 0, dx = dest.x, dy = dest.y;
        if (dx > dst -> m_width || dy > dst -> m_height)
            return { dest.x, dest.y, 0, 0 };
        if (dx < 0) { sx -= dx; dx = 0; }
        if (dy < 0) { sy -= dy; dy = 0; }
        
        point1d_t w = src -> m_width - sx, h = src -> m_height - sy;
        if (w > dst -> m_width - dx)  w = dst -> m_width - dx;
        if (w <= 0) return { dest.x, dest.y, 0, 0 };
        if (h > dst -> m_height - dy) h = dst -> m_height - dy;
        if (h <= 0) return { dest.x, dest.y, 0, 0 };
        
        // start bilting
        color_t *si = src -> m_pbuffer + static_cast<size_t>(sy) * src -> m_width + sx, *sj = 0;
        color_t *di = dst -> m_pbuffer + static_cast<size_t>(dy) * dst -> m_width + dx, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { dx, dy, w, h };
    }

    /**
     * @function surface_t::bilt <src/surface.h>
     * @brief draw one image onto another
     * @param[in] area: src area to bilt
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    bilt (surface_t& source, void*, rect_t area, blend_t special_flags) noexcept{
        if (!this || !&source) return { 0, 0, 0, 0 };
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { 0, 0, 0, 0 }; // display surface quit
        
        // blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        if (mcl_switch_blend_fun_bilt (blend_fun, special_flags, m_data_, source.m_data_))
            return { 0, 0, 0, 0 };

        // rect
        point1d_t sx = area.x, sy = area.y, w = area.w, h = area.h, dx = 0, dy = 0;
        if (!(w || h)) return { 0, 0, 0, 0 };
        if (w < 0) { sx += w; w = -w; }
        if (h < 0) { sy += h; h = -h; }
        if (sx < 0) { dx -= sx; w += sx; sx = 0; }
        if (sy < 0) { dy -= sy; h += sy; sy = 0; }
        if (sx + w > src -> m_width)  w = src -> m_width - sx;
        if (sy + h > src -> m_height) h = src -> m_height - sy;
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock); 
        
        if (w > dst -> m_width - dx)  w = dst -> m_width - dx;
        if (h > dst -> m_height - dy) h = dst -> m_height - dy;
        if (w <= 0 || h <= 0) return { 0, 0, 0, 0 };
        
        // start bilting
        color_t *si = src -> m_pbuffer + static_cast<size_t>(sy) * src -> m_width + sx, *sj = 0;
        color_t *di = dst -> m_pbuffer + static_cast<size_t>(dy) * dst -> m_width + dx, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { 0, 0, w, h };
    }

    /**
     * @function surface_t::bilt <src/surface.h>
     * @brief draw one image onto another
     * @param[in] source: source surface
     * @param[in] dest: destination position for the blit
     * @param[in] area: src area to bilt
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    bilt (surface_t& source, point2d_t dest, rect_t area, blend_t special_flags) noexcept{
        if (!this || !&source) return { dest.x, dest.y, 0, 0 };
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { dest.x, dest.y, 0, 0 }; // display surface quit
        
        // blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        if (mcl_switch_blend_fun_bilt (blend_fun, special_flags, m_data_, source.m_data_))
            return { dest.x, dest.y, 0, 0 };

        // rect
        point1d_t sx = area.x, sy = area.y, w = area.w, h = area.h, dx = 0, dy = 0;
        if (!(w || h)) return { 0, 0, 0, 0 };
        if (w < 0) { sx += w; w = -w; }
        if (h < 0) { sy += h; h = -h; }
        if (sx < 0) { dx -= sx; w += sx; sx = 0; }
        if (sy < 0) { dy -= sy; h += sy; sy = 0; }
        if (sx + w > src -> m_width)  w = src -> m_width - sx;
        if (sy + h > src -> m_height) h = src -> m_height - sy;
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock);
        
        dx += dest.x, dy += dest.y;
        if (dx > dst -> m_width || dy > dst -> m_height)
            return { dest.x, dest.y, 0, 0 };
        if (dx < 0) { sx -= dx; dx = 0; }
        if (dy < 0) { sy -= dy; dy = 0; }
        
        if (w > dst -> m_width - dx)  w = dst -> m_width - dx;
        if (h > dst -> m_height - dy) h = dst -> m_height - dy;
        if (w <= 0 || h <= 0) return { dest.x, dest.y, 0, 0 };
        
        // start bilting
        color_t *si = src -> m_pbuffer + static_cast<size_t>(sy) * src -> m_width + sx, *sj = 0;
        color_t *di = dst -> m_pbuffer + static_cast<size_t>(dy) * dst -> m_width + dx, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { dx, dy, w, h };
    }

    
}
