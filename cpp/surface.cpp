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
#include "../src/display.h"
#include "../src/clog4m.h"
#include "../src/colors.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    surface_t sf_nullptr;
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
    blend_t constexpr mcl_blend_t::Alpha;

    blend_t constexpr mcl_blend_t::PreMultiplied;

    blend_t constexpr mcl_blend_t::Copy_rgb;
    blend_t constexpr mcl_blend_t::Add_rgb;
    blend_t constexpr mcl_blend_t::Sub_rgb;
    blend_t constexpr mcl_blend_t::Mult_rgb;
    blend_t constexpr mcl_blend_t::Min_rgb;
    blend_t constexpr mcl_blend_t::Max_rgb;
    blend_t constexpr mcl_blend_t::Xor_rgb;
    blend_t constexpr mcl_blend_t::Alpha_rgb;

    blend_t constexpr mcl_blend_t::Copy_rgba;
    blend_t constexpr mcl_blend_t::Add_rgba;
    blend_t constexpr mcl_blend_t::Sub_rgba;
    blend_t constexpr mcl_blend_t::Mult_rgba;
    blend_t constexpr mcl_blend_t::Min_rgba;
    blend_t constexpr mcl_blend_t::Max_rgba;
    blend_t constexpr mcl_blend_t::Xor_rgba;
    blend_t constexpr mcl_blend_t::Alpha_rgba;

    surface_t::type constexpr surface_t::SwSurface;
    surface_t::type constexpr surface_t::SrcAlpha;
    surface_t::type constexpr surface_t::SrcColorKey;
#endif

    /**
     * @function mcl_createbmp <cpp/surface.cpp>
     * @brief Create a new bitmap.
     * @return HBITMAP
     */
    static HBITMAP
    mcl_createbmp (point1d_t m_width, point1d_t m_height, PDWORD* m_pbuffer) {
        HBITMAP m_hbmp = nullptr;
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
            reinterpret_cast<void**>(m_pbuffer),
            nullptr, 0u
        );
        return m_hbmp;
    }

    static void
    mcl_release_imgbuf (mcl_imagebuf_t* imgbuf) {
        ::DeleteDC (imgbuf -> m_hdc);
        ::DeleteObject (imgbuf -> m_hbmp);
    }
    
    /**
     * @function mcl_imagebuf_t::init <src/surface.cpp>
     * @brief initialize buffer for surface.
     * @return bool
     */
    bool mcl_imagebuf_t::
    init () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lk(m_nrtlock, L"mcl_imagebuf_t::init");
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
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return false;
        }
        
        // create a new bitmap
        m_hbmp = mcl_createbmp (m_width, m_height, &m_pbuffer);
        if (!m_hbmp) {
            clog4m[cll4m.Warn] << L"surface.init()\n"
                L"    warning:  CreateDIBSection Failed. [-Wsurface-winapi-"
                << ::GetLastError() << L"]\n";
            ::DeleteDC (m_hdc);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return false;
        }
        ::SelectObject (m_hdc, m_hbmp);
        
        // init the bitmap
        ::SetBkMode (m_hdc, TRANSPARENT);

        if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
        return true;
    }

    void mcl_imagebuf_t::
    uninit () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lk(m_nrtlock, L"mcl_imagebuf_t::uninit");
        if (m_width) {
            m_width = 0;
            mcl_release_imgbuf (this);
        }
    }

    /**
     * @function mcl_imagebuf_t::mcl_imagebuf_t <src/surface.cpp>
     * @brief Constructor.
     * @return none
     */
    mcl_imagebuf_t::mcl_imagebuf_t (point1d_t width, point1d_t height) noexcept
    : m_width (width > 0 ? width : 1), m_height (height > 0 ? height : 1), m_alpha (0xff) {
        if (!this -> init ()) m_width = 0;
    }

    /**
     * @function mcl_imagebuf_t::~mcl_imagebuf_t <src/surface.cpp>
     * @brief Destructor.
     * @return none
     */
    mcl_imagebuf_t::~mcl_imagebuf_t() noexcept {
        this -> uninit ();
    }
    


    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Constructor.
     * @param[in] b_srcalpha: True if the pixel format need
     *     including a per-pixel alpha
     * @return none
     */
    surface_t::
    surface_t (void*, type special_flags) noexcept
        : m_dataplus_ (nullptr), m_data_ { char(special_flags & ~SrcColorKey) }{ }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Constructor.
     * @param[in] size: Surface size
     * @param[in] b_srcalpha: True if the pixel format need
     *     including a per-pixel alpha.
     * @return none
     */
    surface_t::
    surface_t (point2d_t size, type special_flags) noexcept
      : m_dataplus_ (new(std::nothrow) mcl_imagebuf_t(size.x, size.y)),
        m_data_{ char(special_flags & ~SrcColorKey) } {
        if (!m_dataplus_) return ;
        mcl_imagebuf_t* dataplus = static_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus -> m_width) {
            delete dataplus;
            m_dataplus_ = nullptr;
            return ;
        }
        ::memset (dataplus -> m_pbuffer, 0, static_cast<size_t>
            (dataplus -> m_width) * dataplus -> m_height * 4u);
    }
    
    /**
     * @function surface_t::~surface_t <src/surface.h>
     * @brief Destructor.
     * @return none
     */
    surface_t::
    ~surface_t () noexcept {
        if (!m_dataplus_) return ;
        mcl_imagebuf_t* p = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        m_dataplus_ = nullptr;
        delete p;
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Copy constructor.
     * @retrun none
     */
    surface_t::
    surface_t (surface_t const& src) noexcept
      : m_dataplus_ (0), m_data_{ src.m_data_[0] } {
        if (!src.m_dataplus_) return ;
        mcl_imagebuf_t* dsrc = static_cast<mcl_imagebuf_t*>(src.m_dataplus_);

        mcl_simpletls_ns::mcl_spinlock_t lk(dsrc -> m_nrtlock, L"surface_t::surface_t");
        if (!dsrc -> m_width) return ;

        m_dataplus_ = new(std::nothrow)
            mcl_imagebuf_t(dsrc -> m_width, dsrc -> m_height);
        if (!m_dataplus_) return ;
        
        mcl_imagebuf_t* ddst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!ddst -> m_width) {
            delete ddst;
            m_dataplus_ = nullptr;
            return ;
        }
        ::BitBlt (ddst -> m_hdc, 0, 0, ddst -> m_width, ddst -> m_height,
            dsrc -> m_hdc, 0, 0, SRCCOPY);
        
        ddst -> m_alpha = dsrc -> m_alpha;
        if (!(m_data_[0] & SrcAlpha))
            ddst -> m_colorkey = dsrc -> m_colorkey;
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Copy constructor.
     * @param b_srcalpha: True if the pixel format need
     * @retrun none
     */
    surface_t::
    surface_t (surface_t const& src, type special_flags) noexcept
      : surface_t (src) {
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus || !dataplus -> m_width)
            return ;

        type b_sa_lhs = special_flags & SrcAlpha;
        type b_sa_rhs = m_data_[0] & SrcAlpha;
        type b_sck_rhs = m_data_[0] & SrcColorKey;

        // copy the alpha info of src(no srcalpha) to dst(has srcalpha)
        if (b_sa_lhs && !b_sa_rhs) {
            color_t* buf = dataplus -> m_pbuffer;
            color_t* bufe = buf + dataplus -> m_width * dataplus -> m_height;

            if (b_sck_rhs) {
                while (buf != bufe) {
                    if ((*buf & 0xffffff) == dataplus -> m_colorkey)
                        *buf = 0;
                    else
                        *buf |= 0xff000000;
                    ++ buf;
                }
            } else {
                while (buf != bufe)
                    *buf |= 0xff000000, ++ buf;
            }
        }
        m_data_[0] = b_sa_lhs;
        if (!b_sa_lhs) m_data_[0] |= b_sck_rhs;
    }

    /**
     * @function surface_t::surface_t <src/surface.h>
     * @brief Move constructor.
     * @retrun none
     */
    surface_t::
    surface_t (surface_t&& src) noexcept
      : m_dataplus_ (src.m_dataplus_),
        m_data_{ src.m_data_[0] } {
        src.m_dataplus_ = nullptr;
    }
    
    /**
     * @function surface_t::operator= <src/surface.h>
     * @brief Copy assignment operator.
     * @return none
     */
    surface_t& surface_t::
    operator= (surface_t const& rhs) noexcept {
        m_data_[0] = rhs.m_data_[0];
        if (m_dataplus_ == rhs.m_dataplus_)
            return *this;

        mcl_imagebuf_t* old_dp = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!rhs.m_dataplus_) {
            mcl_simpletls_ns::mcl_spinlock_t lk(old_dp -> m_nrtlock, L"surface_t::operator=");
            if (old_dp -> m_width) mcl_release_imgbuf (old_dp);
            return *this;
        }
        if (m_dataplus_) {
            mcl_simpletls_ns::mcl_spinlock_t lk(old_dp -> m_nrtlock, L"surface_t::operator=");
            
            // create compatible surface
            mcl_imagebuf_t* dsrc = static_cast<mcl_imagebuf_t*>(rhs.m_dataplus_);
            mcl_imagebuf_t new_dp(dsrc -> m_width, dsrc -> m_height);
            if (!new_dp.m_width) return *this;
            
            // release mem of old surface
            if (old_dp -> m_width) mcl_release_imgbuf (old_dp);
            
            // blit to new surface
            ::BitBlt (new_dp.m_hdc, 0, 0, new_dp.m_width, new_dp.m_height,
            dsrc -> m_hdc, 0, 0, SRCCOPY);
            
            old_dp -> m_pbuffer = new_dp.m_pbuffer;
            old_dp -> m_hbmp    = new_dp.m_hbmp;
            old_dp -> m_hdc     = new_dp.m_hdc;
            old_dp -> m_height  = new_dp.m_height;
            old_dp -> m_width   = new_dp.m_width;

            old_dp -> m_alpha = dsrc -> m_alpha;
            if (!(m_data_[0] & SrcAlpha))
                old_dp -> m_colorkey = dsrc -> m_colorkey;

            // prevent temp imgbuf from calling mem destructor
            new_dp.m_width = 0;
            return *this;
        }
        // quit or never created (same as copy constructor)
        mcl_imagebuf_t* dsrc = static_cast<mcl_imagebuf_t*>(rhs.m_dataplus_);
        mcl_imagebuf_t* new_dp = new (std::nothrow)
            mcl_imagebuf_t (dsrc -> m_width, dsrc -> m_height);
        if (!new_dp) return *this;
        if (!new_dp -> m_width) { delete new_dp; return *this; }
        
        // blit to new surface
        ::BitBlt (new_dp -> m_hdc, 0, 0, new_dp -> m_width, new_dp -> m_height,
            dsrc -> m_hdc, 0, 0, SRCCOPY);
        m_dataplus_ = new_dp;

        new_dp -> m_alpha = dsrc -> m_alpha;
        if (!(m_data_[0] & SrcAlpha))
            new_dp -> m_colorkey = dsrc -> m_colorkey;
        return *this;
    }
    
    /**
     * @function surface_t::operator= <src/surface.h>
     * @brief Move assignment operator.
     * @return none
     */
    surface_t& surface_t::
    operator= (surface_t&& rhs) noexcept {
        m_data_[0] = rhs.m_data_[0];
        if (m_dataplus_ == rhs.m_dataplus_)
            return *this;
        
        // move to this
        mcl_imagebuf_t* cpy_dp = static_cast<mcl_imagebuf_t*>(m_dataplus_);
        m_dataplus_ = rhs.m_dataplus_;
        rhs.m_dataplus_ = nullptr;
        
        // free prev dataplus
        if (cpy_dp) delete cpy_dp;
        return *this;
    }

    /**
     * @function surface_t::operator void* <src/surface.h>
     * @return bool
     */
    surface_t::
    operator void* () const noexcept {
        return m_dataplus_ ?
            const_cast<surface_t*>(this) : 0;
    }

    /**
     * @function surface_t::operator! <src/surface.h>
     * @return bool
     */
    bool surface_t::
    operator! () const noexcept {
        return !m_dataplus_;
    }

    /**
     * @function surface_t::convert_alpha <src/surface.h>
     * @return surface_t
     */
    surface_t surface_t::
    convert_alpha () const noexcept{
        type frhs = display.get_surface().m_data_[0] & SrcAlpha;
        type flhs = this -> m_data_[0] & ~SrcAlpha;
        return surface_t(*this, flhs | frhs);
    }
    surface_t surface_t::
    convert_alpha (bool b_srcalpha) const noexcept{
        type frhs = b_srcalpha ? SrcAlpha : 0;
        type flhs = this -> m_data_[0] & ~SrcAlpha;
        return surface_t (*this, flhs | frhs);
    }
    surface_t surface_t::
    convert_alpha (surface_t const& source) const noexcept{
        type frhs = source.m_data_[0] & SrcAlpha;
        type flhs = this -> m_data_[0] & ~SrcAlpha;
        return surface_t (*this, flhs | frhs);
    }
    surface_t surface_t::
    copy () const noexcept{
        return surface_t(*this);
    }

    /**
     * @function surface_t::set_colorkey <src/surface.h>
     * @return none
     */
    void surface_t::
    set_colorkey () noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return ;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::set_colorkey");
        if (!dataplus -> m_width) return ;
        m_data_[0] &= ~SrcColorKey;
    }
    void surface_t::
    set_colorkey (color_t colorkey) noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return ;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::set_colorkey");
        if (!dataplus -> m_width || (m_data_[0] & SrcAlpha)) return ;
        m_data_[0] |= SrcColorKey;
        dataplus -> m_colorkey = colorkey & 0xffffff;
    }
    bool surface_t::
    get_colorkey (color_t* pcolorkey) const noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus || !dataplus -> m_width || (m_data_[0] & SrcAlpha)) return false;
        bool bck = m_data_[0] & SrcColorKey;
        if (bck && pcolorkey) *pcolorkey = dataplus -> m_colorkey | 0xff000000;
        return bck;
    }

    /**
     * @function surface_t::set_alpha <src/surface.h>
     * @return none
     */
    void surface_t::
    set_alpha () noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return ;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::set_alpha");
        if (!dataplus -> m_width) return ;
        m_data_[0] &= ~SrcAlpha;
        m_data_[0] &= ~SrcColorKey;
        dataplus -> m_alpha = 0xff;
    }
    void surface_t::
    set_alpha (color_t alpha) noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return ;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::set_alpha");
        if (!dataplus -> m_width) return ;
        dataplus -> m_alpha = (alpha <= 0xff ? alpha : 0xff);
    }
    color_t surface_t::
    get_alpha () const noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus || !dataplus -> m_width) return 0xff;
        return dataplus -> m_alpha;
    }

    /**
     * @function surface_t::lock <src/surface.h>
     * @return surface_t&
     */
    surface_t& surface_t::
    lock () noexcept {
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
        return m_dataplus_ && reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_nrtlock;
    }
    
    /**
     * @function surface_t::get_at <src/surface.h>
     * @brief Get the color value at a single pixel
     * @param pos
     * @return color_t
     */
    color_t surface_t::
    get_at (point2d_t pos) const noexcept {
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return opaque; // display surface quit
        if (pos.x < 0 || pos.y < 0) return opaque;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::get_at");
        if (pos.x >= dataplus -> m_width || pos.y >= dataplus -> m_height)
            return opaque; // out of clip area
        
        color_t clr = dataplus -> m_pbuffer[pos.x + dataplus -> m_width * pos.y];
        if (!(m_data_[0] & SrcAlpha)) clr |= 0xff000000; // no per pixel alpha
        return clr;
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
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return opaque; // display surface quit
        if (pos.x < 0 || pos.y < 0) return opaque;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::set_at");
        if (pos.x >= dataplus -> m_width || pos.y >= dataplus -> m_height)
            return opaque; // out of clip area

        return (dataplus -> m_pbuffer[pos.x + dataplus -> m_width * pos.y] = color);
    }

    /**
     * @function surface_t::get_size <src/surface.h>
     * @return point2d_t
     */
    point2d_t surface_t::
    get_size () const noexcept {
        return m_dataplus_ ? point2d_t {
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
        return m_dataplus_ ?
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width : 0;
    }
    
    /**
     * @function surface_t::get_height <src/surface.h>
     * @return point1d_t
     */
    point1d_t surface_t::
    get_height () const noexcept {
        return m_dataplus_ ?
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_height : 0;
    }

    /**
     * @function surface_t::get_rect <src/surface.h>
     * @param center: position that surface centered at
     * @return rect_t
     */
    rect_t surface_t::
    get_rect (void*) const noexcept {
        if (!m_dataplus_) return { 0, 0, 0, 0 }; // display surface quit
        point1d_t rw = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width;
        point1d_t rh = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_height;
        return { 0, 0, rw, rh };
    }
    rect_t surface_t::
    get_rect (point2d_t center) const noexcept {
        if (!m_dataplus_) // display surface quit
            return { center.x, center.y, 0, 0 };
        point1d_t rw = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_width;
        point1d_t rh = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_height;
        return { center.x - (rw >> 1), center.y - (rh >> 1), rw, rh };
    }

    /**
     * @function surface_t::get_at <src/surface.h>
     * @brief Get the additional flags used for the Surface
     * @return char
     */
    surface_t::type surface_t::
    get_flags () const noexcept {
        return m_dataplus_ ? m_data_[0] : 0;
    }

    /**
     * @function surface_t::_pixels_address <src/surface.h>
     */
    color_t* surface_t::
    _pixels_address () noexcept{
        return m_dataplus_ ?
            reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_) -> m_pbuffer
            : nullptr;
    }


    /**
     * @function mcl_switch_blend_fun_blit <src/surface.cpp>
     * @brief Choose blend function. for surface.blit
     * @param[out] blend_func: blend function
     * @param[in] special_flags: special flags
     * @return bool: true if failed
     */
    static bool
    mcl_switch_blend_fun_blit(
        std::function<void(color_t&, color_t)>& blend_fun,
        blend_t special_flags, char* m_data_, char const* m_data_rhs,
        mcl_imagebuf_t* m_dataplus_, mcl_imagebuf_t* m_dataplus_rhs
    ) {
        // alpha info
        color_t lhs_ctrans = (m_data_[0] & surface_t::SrcAlpha) ? 0 : (
            (m_data_[0] & surface_t::SrcColorKey) ? 0xffffff|
                m_dataplus_ -> m_colorkey : (m_dataplus_ -> m_pbuffer[0] & 0xffffff)
            );
        color_t lhs_sa = color_t(m_data_[0] & surface_t::SrcAlpha);
        color_t lhs_ck = m_dataplus_ -> m_colorkey;
        color_t lhs_b_useck = color_t(!lhs_sa && (m_data_[0] & surface_t::SrcColorKey));
        color_t rhs_sa = color_t(m_data_rhs[0] & surface_t::SrcAlpha);
        color_t rhs_ck = m_dataplus_rhs -> m_colorkey;
        color_t rhs_alpha = m_dataplus_rhs -> m_alpha << 24;
        color_t rhs_b_useck = color_t(!rhs_sa && (m_data_rhs[0] & surface_t::SrcColorKey));
        
        // flags info
        bool b_premult = (special_flags & blend.PreMultiplied) && rhs_sa && rhs_alpha == 0xff000000;
        if (!(special_flags & 0x300)) special_flags |= 0x200;
        if ((special_flags & 0x300) == 0x300) special_flags &= ~0x100;
        special_flags &= ~blend.PreMultiplied;
        
        if (special_flags == mcl_blend_t::Alpha_rgba && !rhs_sa && rhs_alpha == 0xff000000)
            special_flags = (special_flags & 0xff) | 0x100;
        if (b_premult) {
            if (special_flags == mcl_blend_t::Copy_rgba || special_flags == mcl_blend_t::Min_rgba
              || special_flags == mcl_blend_t::Max_rgba)
                special_flags = (special_flags & 0xff) | 0x100;
        }
        
        // choose blend function
        switch (special_flags) {
            // copy
            case mcl_blend_t::Copy_rgb: {
                if (rhs_b_useck) { 
                // use colorkey
                    blend_fun = [rhs_ck, lhs_ctrans](color_t& dst, color_t src) {
                        src &= 0xffffff;
                        if (src == rhs_ck) dst = lhs_ctrans;
                        else               dst = src | 0xff000000;
                    };
                    break;
                }
                if (lhs_sa) {
                // use per pixel alpha
                    blend_fun = [](color_t& dst, color_t src) {
                        dst = src | 0xff000000;
                    };
                    break;
                }
                return true; // only copy
            }
            case mcl_blend_t::Copy_rgba: {
                if (rhs_b_useck) {
                // use colorkey
                    blend_fun = [rhs_ck, lhs_ctrans, rhs_alpha](color_t& dst, color_t src) {
                        src &= 0xffffff;
                        if (src == rhs_ck) dst = lhs_ctrans;
                        else               dst = src | rhs_alpha;
                    };
                    break;
                }
                // test if need alpha value
                if (lhs_sa && !rhs_sa) { // no per pixel alpha
                    if (rhs_alpha == 0xff000000) {
                        blend_fun = [](color_t& dst, color_t src) {
                            dst = src | 0xff000000;
                        };
                        break;
                    }
                    blend_fun = [rhs_alpha](color_t& dst, color_t src) {
                        dst = (src & 0xffffff) | rhs_alpha;
                    };
                    break;
                }
                if (lhs_sa && rhs_sa && rhs_alpha != 0xff000000) { // alpha blend
                    blend_fun = [m_dataplus_rhs](color_t& dst, color_t src) {
                        dst = ((src >> 24) * m_dataplus_rhs -> m_alpha / 255) << 24;
                        dst |= src & 0xffffff;
                    };
                    break;
                }
                return true; // only copy
            }
            
            // overlay(alpha blend)
            case mcl_blend_t::Alpha_rgb: {
                if (rhs_b_useck) {
                // use colorkey
                    blend_fun = [rhs_ck](color_t& dst, color_t src) {
                        src &= 0xffffff;
                        if (src != rhs_ck) dst = src | 0xff000000;
                    };
                    break;
                }
                if (lhs_sa) {
                // use per pixel alpha
                    blend_fun = [](color_t& dst, color_t src) {
                        dst = src | 0xff000000;
                    };
                    break;
                }
                return true; // only copy
            }
            case mcl_blend_t::Alpha_rgba: {
                if (b_premult) {
                    blend_fun = [lhs_sa, lhs_b_useck, lhs_ck]
                    (color_t& dst, color_t src) {
                        // calc the alpha of src
                        color_t sa = src >> 24;
                        if (sa == 255) { // src is opaque
                            dst = src;
                            return ;
                        }
                        // change if dst has no srcalpha
                        if (!lhs_sa) {
                            dst &= 0xffffff;
                            if (!lhs_b_useck || dst != lhs_ck) dst |= 0xff000000;
                        }
                        // translucent overlay
                        color_t srsa = getr4rgb(src) * 255;
                        color_t sgsa = getg4rgb(src) * 255;
                        color_t sbsa = getb4rgb(src) * 255;
                        color_t psa = 255 - sa;
                        color_t da = (dst >> 24) * psa / 255;
                        color_t t = sa + da;
                        if (!t) { dst = 0; return ; }
                        color_t r = (getr4rgb(dst) * da + srsa) / t;
                        color_t g = (getg4rgb(dst) * da + sgsa) / t;
                        color_t b = (getb4rgb(dst) * da + sbsa) / t;
                        dst = (t << 24) | (r << 16) | (g << 8) | b;
                    };
                    break;
                }
                blend_fun = [m_dataplus_rhs, lhs_sa, lhs_b_useck,
                    lhs_ck, rhs_sa, rhs_b_useck, rhs_ck]
                (color_t& dst, color_t src) {
                    // calc the alpha of src
                    color_t sa = 0;
                    if (rhs_sa) {
                        sa = src >> 24;
                        if (m_dataplus_rhs -> m_alpha != 255)
                            sa = sa * m_dataplus_rhs -> m_alpha / 255;
                    } else {
                        if (!rhs_b_useck || src != rhs_ck)
                            sa = m_dataplus_rhs -> m_alpha;
                    }
                    if (sa == 255) { // src is opaque
                        dst = src;
                        return ;
                    }
                    // change if dst has no srcalpha
                    if (!lhs_sa) {
                        dst &= 0xffffff;
                        if (!lhs_b_useck || dst != lhs_ck) dst |= 0xff000000;
                    }
                    // translucent overlay
                    color_t srsa = getr4rgb(src) * sa;
                    color_t sgsa = getg4rgb(src) * sa;
                    color_t sbsa = getb4rgb(src) * sa;
                    color_t psa = 255 - sa;
                    color_t da = (dst >> 24) * psa / 255;
                    color_t t = sa + da;
                    if (!t) { dst = 0; return ; }
                    color_t r = (getr4rgb(dst) * da + srsa) / t;
                    color_t g = (getg4rgb(dst) * da + sgsa) / t;
                    color_t b = (getb4rgb(dst) * da + sbsa) / t;
                    dst = (t << 24) | (r << 16) | (g << 8) | b;
                };
                break;
            }
            
            // darken
            // for rgb value: dst = dst < src ? dst : src
            // alpha value does not participate in comparison
            case mcl_blend_t::Min_rgb: {
                if (lhs_b_useck || rhs_b_useck) {
                    if (lhs_b_useck) lhs_ck |= 0xff000000;
                    if (rhs_b_useck) rhs_ck |= 0xff000000;
                    blend_fun = [lhs_ck, rhs_ck](color_t& dst, color_t src) {
                        src |= 0xff000000;
                        dst |= 0xff000000;
                        if (src == rhs_ck) return ;
                        if (dst == lhs_ck || dst > src) dst = src;
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    src |= 0xff000000;
                    dst |= 0xff000000;
                    if (dst > src) dst = src;
                };
                break;
            }
            case mcl_blend_t::Min_rgba: {
                blend_fun = [m_dataplus_rhs, rhs_sa, lhs_b_useck, 
                    rhs_b_useck, lhs_ck, rhs_ck, rhs_alpha]
                (color_t& dst, color_t src) {
                    color_t cdst = dst & 0xffffff;
                    color_t csrc = src & 0xffffff;
                    if (rhs_b_useck && csrc == rhs_ck) return ;
                    if ((lhs_b_useck && cdst == lhs_ck) || cdst > csrc) {
                        if (rhs_alpha != 0xff000000) {
                            if (rhs_sa)
                                src = csrc | (((src >> 24) * m_dataplus_rhs -> m_alpha / 255) << 24);
                            else
                                src = csrc | rhs_alpha;
                        }
                        dst = src;
                    }
                };
                break;
            }
            
            // lighten
            // for rgb value: dst = dst > src ? dst : src
            // alpha value does not participate in comparison
            case mcl_blend_t::Max_rgb: {
                if (lhs_b_useck || rhs_b_useck) {
                    if (lhs_b_useck) lhs_ck |= 0xff000000;
                    if (rhs_b_useck) rhs_ck |= 0xff000000;
                    blend_fun = [lhs_ck, rhs_ck](color_t& dst, color_t src) {
                        src |= 0xff000000;
                        dst |= 0xff000000;
                        if (src == rhs_ck) return ;
                        if (dst == lhs_ck || dst < src) dst = src;
                    };
                    break;
                }
                blend_fun = [](color_t& dst, color_t src) {
                    src |= 0xff000000;
                    dst |= 0xff000000;
                    if (dst < src) dst = src;
                };
                break;
            }
            case mcl_blend_t::Max_rgba: {
                blend_fun = [m_dataplus_rhs, rhs_sa, lhs_b_useck, 
                    rhs_b_useck, lhs_ck, rhs_ck, rhs_alpha]
                (color_t& dst, color_t src) {
                    color_t cdst = dst & 0xffffff;
                    color_t csrc = src & 0xffffff;
                    if (rhs_b_useck && csrc == rhs_ck) return ;
                    if ((lhs_b_useck && cdst == lhs_ck) || cdst < csrc) {
                        if (rhs_alpha != 0xff000000) {
                            if (rhs_sa)
                                src = csrc | (((src >> 24) * m_dataplus_rhs -> m_alpha / 255) << 24);
                            else
                                src = csrc | rhs_alpha;
                        }
                        dst = src;
                    }
                };
                break;
            }
            
            // color dodge
            // dst = src + src * dst / (255 - dst)
            case mcl_blend_t::Add_rgb: {
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                if (rhs_b_useck) rhs_ck |= 0xff000000;
                blend_fun = [lhs_ck, rhs_ck](color_t& dst, color_t src) {
                    src |= 0xff000000;
                    dst |= 0xff000000;
                    if (src == rhs_ck) return ;
                    if (dst == lhs_ck) return ;
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = src & 0xff,    b2 = dst & 0xff;
                    if ((r2 + 1) & 0xff) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 & ~0xff) r2 = 255; }
                    if ((g2 + 1) & 0xff) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 & ~0xff) g2 = 255; }
                    if ((b2 + 1) & 0xff) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 & ~0xff) b2 = 255; }
                    dst = 0xff000000 | (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            case mcl_blend_t::Add_rgba: {
                blend_fun = [m_dataplus_rhs, lhs_b_useck,
                    rhs_b_useck, lhs_ck, rhs_ck, rhs_sa, rhs_alpha]
                (color_t& dst, color_t src) {
                    if (rhs_b_useck && (src & 0xffffff) == rhs_ck) return ;
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
                    if (!rhs_sa) src |= 0xff000000;
                    color_t a1 = src >> 24,     a2 = dst >> 24;
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                    if (rhs_alpha != 0xff000000) {
                        if (rhs_sa) a1 = (src >> 24) * m_dataplus_rhs -> m_alpha / 255;
                        else        a1 = m_dataplus_rhs -> m_alpha;
                    }
                    if (a2 != 255) { a2 = a1 + a1 * a2 / (255 - a2); if (a2 > 255) a2 = 255; }
                    if (r2 != 255) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 > 255) r2 = 255; }
                    if (g2 != 255) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 > 255) g2 = 255; }
                    if (b2 != 255) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 > 255) b2 = 255; }
                    dst = (a2 << 24) | (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            
            // color burn
            // dst = src - (255 - src) * (255 - dst) / dst
            case mcl_blend_t::Sub_rgb: {
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                if (rhs_b_useck) rhs_ck |= 0xff000000;
                blend_fun = [lhs_ck, rhs_ck](color_t& dst, color_t src) {
                    src |= 0xff000000;
                    dst |= 0xff000000;
                    if (src == rhs_ck) return ;
                    if (dst == lhs_ck) return ;
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
                    dst = 0xff000000 | r2 | g2 | b2;
                };
                break;
            }
            case mcl_blend_t::Sub_rgba: {
                blend_fun = [m_dataplus_rhs, lhs_b_useck,
                    rhs_b_useck, lhs_ck, rhs_ck, rhs_sa, rhs_alpha]
                (color_t& dst, color_t src) {
                    if (rhs_b_useck && (src & 0xffffff) == rhs_ck) return ;
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
                    if (!rhs_sa) src |= 0xff000000;
                    color_t a1 = (src >> 24) + 0xfffff;
                    color_t r1 = getr4rgb(src) + 0xfffff;
                    color_t g1 = getg4rgb(src) + 0xfffff;
                    color_t b1 = getb4rgb(src) + 0xfffff;
                    if (rhs_alpha != 0xff000000) {
                        if (rhs_sa) a1 = (src >> 24) * m_dataplus_rhs -> m_alpha / 255;
                        else        a1 = m_dataplus_rhs -> m_alpha;
                    }
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
            // dst = src * dst / 255
            case mcl_blend_t::Mult_rgb: {
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                if (rhs_b_useck) rhs_ck |= 0xff000000;
                blend_fun = [lhs_ck, rhs_ck](color_t& dst, color_t src) {
                    src |= 0xff000000;
                    dst |= 0xff000000;
                    if (src == rhs_ck) return ;
                    if (dst == lhs_ck) return ;
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                    dst = 0xff000000 | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            case mcl_blend_t::Mult_rgba: {
                blend_fun = [m_dataplus_rhs, lhs_b_useck,
                    rhs_b_useck, lhs_ck, rhs_ck, rhs_sa, rhs_alpha]
                (color_t& dst, color_t src) {
                    if (rhs_b_useck && (src & 0xffffff) == rhs_ck) return ;
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
                    if (!rhs_sa) src |= 0xff000000;
                    color_t a1 = src >> 24,     a2 = dst >> 24;
                    color_t r1 = getr4rgb(src), r2 = getr4rgb(dst);
                    color_t g1 = getg4rgb(src), g2 = getg4rgb(dst);
                    color_t b1 = getb4rgb(src), b2 = getb4rgb(dst);
                    if (rhs_alpha != 0xff000000) {
                        if (rhs_sa) a1 = (src >> 24) * m_dataplus_rhs -> m_alpha / 255;
                        else        a1 = m_dataplus_rhs -> m_alpha;
                    }
                    dst = (a1 * a2 / 255 << 24) | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            
            // xor
            case mcl_blend_t::Xor_rgb: {
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                if (rhs_b_useck) rhs_ck |= 0xff000000;
                blend_fun = [lhs_ck, rhs_ck](color_t& dst, color_t src) {
                    src |= 0xff000000;
                    dst |= 0xff000000;
                    if (src == rhs_ck) return ;
                    if (dst == lhs_ck) return ;
                    dst = 0xff000000 | (dst ^ src);
                };
                break;
            }
            case mcl_blend_t::Xor_rgba: {
                if (!lhs_b_useck && !rhs_b_useck && rhs_alpha == 0xff000000) {
                    blend_fun = [](color_t& dst, color_t src) { dst ^= src; };
                    break;
                }
                blend_fun = [m_dataplus_rhs, lhs_b_useck,
                    rhs_b_useck, lhs_ck, rhs_ck, rhs_sa, rhs_alpha]
                (color_t& dst, color_t src) {
                    if (rhs_b_useck && (src & 0xffffff) == rhs_ck) return ;
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
                    if (!rhs_sa) src |= 0xff000000;
                    if (rhs_alpha != 0xff000000) {
                        if (rhs_sa) src = (src & 0xffffff) | (((src >> 24) * m_dataplus_rhs -> m_alpha / 255) << 24);
                        else        src = (src & 0xffffff) | rhs_alpha;
                    }
                    dst ^= src;
                };
                break;
            }
            
            // unknown
            default: return true;
        }
        return false;
    }
    
    /**
     * @function surface_t::blit <src/surface.h>
     * @brief draw one image onto another
     * @param[in] source: source surface
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    blit (surface_t const& source, void*, void*, blend_t special_flags) noexcept{
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { 0, 0, 0, 0 }; // display surface quit

        // TO BE OPTIMIZED: a blit to self
        surface_t rev;
        if (src == dst) {
            rev = source;
            dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
            src = reinterpret_cast<mcl_imagebuf_t*>(rev.m_dataplus_);
            src -> m_alpha = 255;
            if (!(src && dst)) return { 0, 0, 0, 0 };
        }
        
        // lock
        std::function<void(color_t& dst, color_t src)> blend_fun;
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock, L"surface_t::blit");
        if (!(dst -> m_width && source.m_dataplus_ && src -> m_width))
            return { 0, 0, 0, 0 };
        
        // check blend flags
        bool ret = mcl_switch_blend_fun_blit
            (blend_fun, special_flags, m_data_, source.m_data_, dst, src);
        
        // restrict area within the surface
        point1d_t w = src -> m_width, h = src -> m_height;
        if (w > dst -> m_width)  w = dst -> m_width;
        if (h > dst -> m_height) h = dst -> m_height;

        // only copy
        if (ret) {
            ::BitBlt (dst -> m_hdc, 0, 0, w, h, src -> m_hdc, 0, 0, SRCCOPY);
            return { 0, 0, w, h };
        }

        // start bliting
        color_t *si = src -> m_pbuffer, *sj = 0;
        color_t *di = dst -> m_pbuffer, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { 0, 0, w, h };
    }

    /**
     * @function surface_t::blit <src/surface.h>
     * @brief draw one image onto another
     * @param[in] source: source surface
     * @param[in] dest: destination position for the blit
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    blit (surface_t const& source, point2d_t dest, void*, blend_t special_flags) noexcept{
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { dest.x, dest.y, 0, 0 }; // display surface quit

        // TO BE OPTIMIZED: a blit to self
        surface_t rev;
        if (src == dst) {
            rev = source;
            dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
            src = reinterpret_cast<mcl_imagebuf_t*>(rev.m_dataplus_);
            src -> m_alpha = 255;
            if (!(src && dst)) return { dest.x, dest.y, 0, 0 };
        }

        // check blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        bool ret = mcl_switch_blend_fun_blit(blend_fun, special_flags, m_data_, source.m_data_, dst, src);
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock, L"surface_t::blit");
        if (!(dst -> m_width && src && src -> m_width))
            return { dest.x, dest.y, 0, 0 };
        
        // restrict area within the surface
        point1d_t sx = 0, sy = 0, dx = dest.x, dy = dest.y;
        if (dx > dst -> m_width || dy > dst -> m_height)
            return { dest.x, dest.y, 0, 0 };
        if (dx < 0) { sx -= dx; dx = 0; }
        if (dy < 0) { sy -= dy; dy = 0; }
        
        point1d_t w  = src -> m_width - sx, h  = src -> m_height - sy;
        point1d_t dw = dst -> m_width - dx, dh = dst -> m_height - dy;
        w = (w > dw ? dw : w); h = (h > dh ? dh : h);
        if (w <= 0 || h <= 0) return { dest.x, dest.y, 0, 0 };

        // only copy
        if (ret) {
            ::BitBlt (dst -> m_hdc, dx, dy, w, h, src -> m_hdc, sx, sy, SRCCOPY);
            return { dx, dy, w, h };
        }

        // start bliting
        color_t *si = src -> m_pbuffer + static_cast<size_t>(sy) * src -> m_width + sx, *sj = 0;
        color_t *di = dst -> m_pbuffer + static_cast<size_t>(dy) * dst -> m_width + dx, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { dx, dy, w, h };
    }

    /**
     * @function surface_t::blit <src/surface.h>
     * @brief draw one image onto another
     * @param[in] area: src area to blit
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    blit (surface_t const& source, void*, rect_t area, blend_t special_flags) noexcept{
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { 0, 0, 0, 0 }; // display surface quit
        
        // TO BE OPTIMIZED: a blit to self
        surface_t rev;
        if (src == dst) {
            rev = source;
            dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
            src = reinterpret_cast<mcl_imagebuf_t*>(rev.m_dataplus_);
            src -> m_alpha = 255;
            if (!(src && dst)) return { 0, 0, 0, 0 };
        }

        // check blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        bool ret = mcl_switch_blend_fun_blit(blend_fun, special_flags, m_data_, source.m_data_, dst, src);

        // restrict area within the surface
        point1d_t sx = area.x, sy = area.y, w = area.w, h = area.h, dx = 0, dy = 0;
        if (!(w || h)) return { 0, 0, 0, 0 };
        if (w < 0) { sx += w; w = -w; }
        if (h < 0) { sy += h; h = -h; }
        if (sx < 0) { dx -= sx; w += sx; sx = 0; }
        if (sy < 0) { dy -= sy; h += sy; sy = 0; }
        point1d_t tw = src -> m_width - sx, th = src -> m_height - sy;
        w = (w > tw ? tw : w); h = (h > th ? th : h);
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock, L"surface_t::blit");
        if (!(dst -> m_width && source.m_dataplus_ && src -> m_width))
            return { 0, 0, 0, 0 };
        
        // restrict area within the surface
        tw = dst -> m_width - dx, th = dst -> m_height - dy;
        w = (w > tw ? tw : w); h = (h > th ? th : h);
        if (w <= 0 || h <= 0) return { 0, 0, 0, 0 };

        // only copy
        if (ret) {
            ::BitBlt (dst -> m_hdc, dx, dy, w, h, src -> m_hdc, sx, sy, SRCCOPY);
            return { 0, 0, w, h };
        }

        // start bliting
        color_t *si = src -> m_pbuffer + static_cast<size_t>(sy) * src -> m_width + sx, *sj = 0;
        color_t *di = dst -> m_pbuffer + static_cast<size_t>(dy) * dst -> m_width + dx, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { 0, 0, w, h };
    }

    /**
     * @function surface_t::blit <src/surface.h>
     * @brief draw one image onto another
     * @param[in] source: source surface
     * @param[in] dest: destination position for the blit
     * @param[in] area: src area to blit
     * @param[in] special_flags: blend flags
     * @return rect_t
     */
    rect_t surface_t::
    blit (surface_t const& source, point2d_t dest, rect_t area, blend_t special_flags) noexcept{
        mcl_imagebuf_t* dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        mcl_imagebuf_t* src = reinterpret_cast<mcl_imagebuf_t*>(source.m_dataplus_);
        if (!(src && dst)) return { dest.x, dest.y, 0, 0 }; // display surface quit
        
        // TO BE OPTIMIZED: a blit to self
        surface_t rev;
        if (src == dst) {
            rev = source;
            dst = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
            src = reinterpret_cast<mcl_imagebuf_t*>(rev.m_dataplus_);
            src -> m_alpha = 255;
            if (!(src && dst)) return { dest.x, dest.y, 0, 0 };
        }

        // check blend flags
        std::function<void(color_t& dst, color_t src)> blend_fun;
        bool ret = mcl_switch_blend_fun_blit(blend_fun, special_flags, m_data_, source.m_data_, dst, src);

        // restrict area within the surface
        point1d_t sx = area.x, sy = area.y, w = area.w, h = area.h, dx = 0, dy = 0;
        if (!(w || h)) return { 0, 0, 0, 0 };
        if (w < 0) { sx += w; w = -w; }
        if (h < 0) { sy += h; h = -h; }
        if (sx < 0) { dx -= sx; w += sx; sx = 0; }
        if (sy < 0) { dy -= sy; h += sy; sy = 0; }
        point1d_t tw = src -> m_width - sx, th = src -> m_height - sy;
        w = (w > tw ? tw : w); h = (h > th ? th : h);
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dst -> m_nrtlock, L"surface_t::blit");
        if (!(dst -> m_width && source.m_dataplus_ && src -> m_width))
            return { dest.x, dest.y, 0, 0 };
        
        // restrict area within the surface
        dx += dest.x, dy += dest.y;
        if (dx > dst -> m_width || dy > dst -> m_height)
            return { dest.x, dest.y, 0, 0 };
        if (dx < 0) { sx -= dx; dx = 0; }
        if (dy < 0) { sy -= dy; dy = 0; }
        
        tw = dst -> m_width - dx, th = dst -> m_height - dy;
        w = (w > tw ? tw : w); h = (h > th ? th : h);
        if (w <= 0 || h <= 0) return { dest.x, dest.y, 0, 0 };
        
        // only copy
        if (ret) {
            ::BitBlt (dst -> m_hdc, dx, dy, w, h, src -> m_hdc, sx, sy, SRCCOPY);
            return { dx, dy, w, h };
        }

        // start bliting
        color_t *si = src -> m_pbuffer + static_cast<size_t>(sy) * src -> m_width + sx, *sj = 0;
        color_t *di = dst -> m_pbuffer + static_cast<size_t>(dy) * dst -> m_width + dx, *dj = 0;
        color_t *di0 = di + static_cast<size_t>(h) * dst -> m_width, *dj0 = 0;

        for (; di != di0; si += src -> m_width, di += dst -> m_width)
            for (sj = si, dj = di, dj0 = di + w; dj != dj0; ++ sj, ++ dj)
                blend_fun (*dj, *sj);
        
        return { dx, dy, w, h };
    }

    /**
     * @function surface_t::resize <src/surface.h>
     * @brief Resize the surface
     * @param size
     * @param b_keep
     * @return point2d_t
     */
    point2d_t surface_t::
    resize (point2d_t size, bool b_fast) noexcept {
        if (this == &sf_nullptr) return { 0, 0 };
        if (size.x <= 0) size.x = 1;
        if (size.y <= 0) size.y = 1;

        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) {
        // display surface quit or never created
        // create a new surface
            m_dataplus_ = dataplus = new(std::nothrow) mcl_imagebuf_t(size.x, size.y);
            if (!dataplus) return { 0, 0 };
            if (!dataplus -> m_width) {
                delete dataplus;
                m_dataplus_ = nullptr;
                return { 0, 0 };
            }
            if (!b_fast)
                ::memset (dataplus -> m_pbuffer, 0, static_cast<size_t>
                    (dataplus -> m_width) * dataplus -> m_height * 4u);
            return size;
        }
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::resize");
        if (size.x == dataplus -> m_width && size.y == dataplus -> m_height || !size.x)
            return { 0, 0 }; // no change
        
        if (b_fast) {
        // just resize the buffer
            PDWORD  bmp_buf = nullptr;
            HBITMAP bmp     = mcl_createbmp (size.x, size.y, &bmp_buf);
            if (!bmp) return { 0, 0 };

            HBITMAP old_bmp = reinterpret_cast<HBITMAP>(
                ::SelectObject(dataplus -> m_hdc, bmp));
            ::DeleteObject (old_bmp);
            
            dataplus -> m_pbuffer = bmp_buf;
            dataplus -> m_hbmp    = bmp;
            dataplus -> m_height  = size.y;
            dataplus -> m_width   = size.x;
            
            return size; // no change
        }

        // keep the old content
        surface_t surf (size, m_data_[0]);
        if (!surf) return { 0, 0 };
        mcl_imagebuf_t* surf_dataplus = reinterpret_cast<mcl_imagebuf_t*>(surf.m_dataplus_);
        if (!surf_dataplus -> m_width) return { 0, 0 };
        ::BitBlt (surf_dataplus -> m_hdc, 0, 0, size.x, size.y, dataplus -> m_hdc, 0, 0, SRCCOPY);
        
        // delete the old surface
        mcl_release_imgbuf (dataplus);
        
        dataplus -> m_hbmp    = surf_dataplus -> m_hbmp;
        dataplus -> m_pbuffer = surf_dataplus -> m_pbuffer;
        dataplus -> m_hdc     = surf_dataplus -> m_hdc;
        dataplus -> m_height  = surf_dataplus -> m_height;
        dataplus -> m_width   = surf_dataplus -> m_width;
        
        // delete the tmp surface without destructor
        surf_dataplus -> m_width = 0; 
        delete surf_dataplus;
        surf.m_dataplus_ = nullptr;
        
        return this -> get_size ();
    }
    

    /**
     * @function mcl_switch_blend_fun_fill <src/surface.cpp>
     * @brief Choose blend function. for surface.fill
     * @param[out] blend_func: blend function
     * @param[in] color: src color value
     * @param[in] special_flags: special flags
     * @return bool: true if failed
     */
    static bool mcl_switch_blend_fun_fill(
        std::function<void(color_t&)>& blend_fun, color_t color,
        blend_t special_flags, char* m_data_, mcl_imagebuf_t* m_dataplus_
    ) {
        // alpha info
        color_t lhs_sa = color_t(m_data_[0] & surface_t::SrcAlpha);
        color_t lhs_ck = m_dataplus_ -> m_colorkey;
        color_t lhs_b_useck = color_t(!lhs_sa && (m_data_[0] & surface_t::SrcColorKey));
        
        // flags info
        bool b_premult = special_flags & blend.PreMultiplied;
        if (!(special_flags & 0x300)) special_flags |= 0x200;
        if ((special_flags & 0x300) == 0x300) special_flags &= ~0x100;
        special_flags &= ~blend.PreMultiplied;
        
        if (b_premult) {
            if (special_flags == mcl_blend_t::Copy_rgba || special_flags == mcl_blend_t::Min_rgba
              || special_flags == mcl_blend_t::Max_rgba)
                special_flags = (special_flags & 0xff) | 0x100;
        }

        // choose blend function
        switch (special_flags) {
            // copy
            case mcl_blend_t::Copy_rgb: {
                color |= 0xff000000;
                blend_fun = [color](color_t& dst) { dst = color; };
                break;
            }
            case mcl_blend_t::Copy_rgba: {
                blend_fun = [color](color_t& dst) { dst = color; };
                break;
            }

            // overlay
            case mcl_blend_t::Alpha_rgb: {
                color |= 0xff000000;
                blend_fun = [color](color_t& dst) { dst = color; };
                break;
            }
            case mcl_blend_t::Alpha_rgba: {
                color_t sa = color >> 24;
                if (0xff == sa) { // no alpha
                    blend_fun = [color](color_t& dst) { dst = color; };
                    break;
                }
                color_t psa  = 255 - sa;
                color_t pmsa = b_premult ? 255 : sa;
                color_t srsa = getr4rgb(color) * pmsa;
                color_t sgsa = getg4rgb(color) * pmsa;
                color_t sbsa = getb4rgb(color) * pmsa;
                if (!lhs_sa) {
                    if (lhs_b_useck) {
                        blend_fun = [color, srsa, sgsa, sbsa, psa, lhs_ck]
                        (color_t& dst) {
                            if ((dst & 0xffffff) == lhs_ck) {
                                dst = color; return ;
                            }
                            color_t r = (getr4rgb(dst) * psa + srsa) / 255;
                            color_t g = (getg4rgb(dst) * psa + sgsa) / 255;
                            color_t b = (getb4rgb(dst) * psa + sbsa) / 255;
                            dst = (r << 16) | (g << 8) | b;
                        };
                        break;
                    }
                    blend_fun = [srsa, sgsa, sbsa, psa]
                    (color_t& dst) {
                        color_t r = (getr4rgb(dst) * psa + srsa) / 255;
                        color_t g = (getg4rgb(dst) * psa + sgsa) / 255;
                        color_t b = (getb4rgb(dst) * psa + sbsa) / 255;
                        dst = (r << 16) | (g << 8) | b;
                    };
                    break;
                }
                blend_fun = [srsa, sgsa, sbsa, sa, psa](color_t& dst) {
                    color_t da = (dst >> 24) * psa / 255;
                    color_t t = sa + da;
                    if (!t) { dst = 0; return ; }
                    color_t r = (getr4rgb(dst) * da + srsa) / t;
                    color_t g = (getg4rgb(dst) * da + sgsa) / t;
                    color_t b = (getb4rgb(dst) * da + sbsa) / t;
                    dst = (t << 24) | (r << 16) | (g << 8) | b;
                };
                break;
            }

            // darken
            case mcl_blend_t::Min_rgb: {
                color |= 0xff000000;
                if (lhs_b_useck) {
                    if (lhs_b_useck) lhs_ck |= 0xff000000;
                    blend_fun = [color, lhs_ck](color_t& dst) {
                        dst |= 0xff000000;
                        if (dst == lhs_ck || dst > color) dst = color;
                    };
                    break;
                }
                blend_fun = [color](color_t& dst) {
                    dst |= 0xff000000;
                    if (dst > color) dst = color;
                };
                break;
            }
            case mcl_blend_t::Min_rgba: {
                blend_fun = [color, lhs_b_useck, lhs_ck] (color_t& dst) {
                    color_t cdst = dst & 0xffffff;
                    color_t csrc = color & 0xffffff;
                    if ((lhs_b_useck && cdst == lhs_ck) || cdst > csrc)
                        dst = color;
                };
                break;
            }

            // lighten
            case mcl_blend_t::Max_rgb: {
                color |= 0xff000000;
                if (lhs_b_useck) {
                    if (lhs_b_useck) lhs_ck |= 0xff000000;
                    blend_fun = [color, lhs_ck](color_t& dst) {
                        dst |= 0xff000000;
                        if (dst == lhs_ck || dst < color) dst = color;
                    };
                    break;
                }
                blend_fun = [color](color_t& dst) {
                    dst |= 0xff000000;
                    if (dst < color) dst = color;
                };
                break;
            }
            case mcl_blend_t::Max_rgba: {
                color_t csrc = color & 0xffffff;
                blend_fun = [color, csrc, lhs_b_useck, lhs_ck] (color_t& dst) {
                    color_t cdst = dst & 0xffffff;
                    if ((lhs_b_useck && cdst == lhs_ck) || cdst < csrc)
                        dst = color;
                };
                break;
            }

            // color dodge
            case mcl_blend_t::Add_rgb: {
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                color_t r1 = getr4rgb(color);
                color_t g1 = getg4rgb(color);
                color_t b1 = color & 0xff;
                blend_fun = [lhs_ck, r1, g1, b1](color_t& dst) {
                    dst |= 0xff000000;
                    if (dst == lhs_ck) return ;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = dst & 0xff;
                    if ((r2 + 1) & 0xff) { r2 = r1 + r1 * r2 / (255 - r2); if (r2 & ~0xff) r2 = 255; }
                    if ((g2 + 1) & 0xff) { g2 = g1 + g1 * g2 / (255 - g2); if (g2 & ~0xff) g2 = 255; }
                    if ((b2 + 1) & 0xff) { b2 = b1 + b1 * b2 / (255 - b2); if (b2 & ~0xff) b2 = 255; }
                    dst = 0xff000000 | (r2 << 16) | (g2 << 8) | b2;
                };
                break;
            }
            case mcl_blend_t::Add_rgba: {
                color_t a1 = color >> 24;
                color_t r1 = getr4rgb(color);
                color_t g1 = getg4rgb(color);
                color_t b1 = color & 0xff;
                blend_fun = [lhs_b_useck, lhs_ck, a1, r1, g1, b1](color_t& dst) {
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
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
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                color_t r1 = getr4rgb(color) + 0xfffff;
                color_t g1 = getg4rgb(color) + 0xfffff;
                color_t b1 = getb4rgb(color) + 0xfffff;
                color_t pr1 = 255 + 0xfffff - r1;
                color_t pg1 = 255 + 0xfffff - g1;
                color_t pb1 = 255 + 0xfffff - b1;
                blend_fun = [lhs_ck, r1, g1, b1, pr1, pg1, pb1](color_t& dst) {
                    dst |= 0xff000000;
                    if (dst == lhs_ck) return ;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    if (r2 != 0) r2 = r1 - pr1 * (255 - r2) / r2;
                    if (g2 != 0) g2 = g1 - pg1 * (255 - g2) / g2;
                    if (b2 != 0) b2 = b1 - pb1 * (255 - b2) / b2;
                    r2 = (r2 < 0xfffff ? 0 : (r2 - 0xfffff) << 16);
                    g2 = (g2 < 0xfffff ? 0 : (g2 - 0xfffff) << 8 );
                    b2 = (b2 < 0xfffff ? 0 : (b2 - 0xfffff)      );
                    dst = 0xff000000 | r2 | g2 | b2;
                };
                break;
            }
            case mcl_blend_t::Sub_rgba: {
                color_t a1 = (color >> 24) + 0xfffff;
                color_t r1 = getr4rgb(color) + 0xfffff;
                color_t g1 = getg4rgb(color) + 0xfffff;
                color_t b1 = getb4rgb(color) + 0xfffff;
                color_t pa1 = 255 + 0xfffff - a1;
                color_t pr1 = 255 + 0xfffff - r1;
                color_t pg1 = 255 + 0xfffff - g1;
                color_t pb1 = 255 + 0xfffff - b1;
                blend_fun = [lhs_b_useck, lhs_ck, a1,
                    r1, g1, b1, pa1, pr1, pg1, pb1]
                (color_t& dst) {
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
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
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                color_t r1 = getr4rgb(color);
                color_t g1 = getg4rgb(color);
                color_t b1 = getb4rgb(color);
                blend_fun = [lhs_ck, r1, g1, b1](color_t& dst) {
                    dst |= 0xff000000;
                    if (dst == lhs_ck) return ;
                    color_t r2 = getr4rgb(dst);
                    color_t g2 = getg4rgb(dst);
                    color_t b2 = getb4rgb(dst);
                    dst = 0xff000000 | (r1 * r2 / 255 << 16) | (g1 * g2 / 255 << 8) | (b1 * b2 / 255);
                };
                break;
            }
            case mcl_blend_t::Mult_rgba: {
                color_t a1 = color >> 24;
                color_t r1 = getr4rgb(color);
                color_t g1 = getg4rgb(color);
                color_t b1 = getb4rgb(color);
                blend_fun = [lhs_b_useck, lhs_ck, a1, r1, g1, b1](color_t& dst) {
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
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
                if (lhs_b_useck) lhs_ck |= 0xff000000;
                color |= 0xff000000;
                blend_fun = [color, lhs_ck](color_t& dst) {
                    dst |= 0xff000000;
                    if (dst == lhs_ck) return ;
                    dst = 0xff000000 | (dst ^ color);
                };
                break;
            }
            case mcl_blend_t::Xor_rgba: {
                blend_fun = [color, lhs_b_useck, lhs_ck](color_t& dst) {
                    if (lhs_b_useck && (dst & 0xffffff) == lhs_ck) return ;
                    dst ^= color;
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
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return { 0, 0, 0, 0 }; // display surface quit
        
        // check blend flags
        std::function<void(color_t&)> blend_fun;
        if (mcl_switch_blend_fun_fill (blend_fun, color, special_flags, m_data_, dataplus))
            return { 0, 0, 0, 0 };

        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::fill");
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
        if (!(recta.w && recta.h))
            return { recta.x, recta.y, 0, 0 }; 
        
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) // display surface quit
            return { recta.x, recta.y, 0, 0 }; 
        
        // choose blend function
        std::function<void(color_t&)> blend_fun;
        if (mcl_switch_blend_fun_fill (blend_fun, color, special_flags, m_data_, dataplus))
            return { recta.x, recta.y, 0, 0 };

        // impact rect
        point1d_t x = recta.x, y = recta.y, w = recta.w, h = recta.h;
        if (w < 0) x += w + 1, w = -w;
        if (h < 0) y += h + 1, h = -h;
        
        // completely out of window
        if (x + w <= 0 || y + h <= 0)
            return { recta.x, recta.y, 0, 0 };

        // prepare for blending
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::fill");
        if (!dataplus -> m_width)
            return { recta.x, recta.y, 0, 0 }; 
        
        // completely out of window
        if (x >= dataplus -> m_width || y >= dataplus -> m_height)
            return { recta.x, recta.y, 0, 0 };

        // part of area is outside the surface
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
     * @function surface_t::get_bounding_rect <src/surface.h>
     * @brief Find the smallest rect containing data.
     * @param[in] min_alpha
     * @return rect_t
     */
    rect_t surface_t::
    get_bounding_rect (color_t min_alpha) const noexcept{
        mcl_imagebuf_t* dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!dataplus) return { 0, 0, 0, 0 }; 
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"surface_t::get_bounding_rect");
        if (!dataplus -> m_width) return { 0, 0, 0, 0 }; 

        color_t*  src = dataplus -> m_pbuffer;
        point1d_t tx1 = dataplus -> m_width, ty1 = dataplus -> m_height;
        point1d_t tx2 = -1, ty2 = -1, x = 0, y = 0;

        if (m_data_[0] & SrcAlpha) {
            // alpha blend
            if (dataplus -> m_alpha != 255) {
                min_alpha = color_t(float(min_alpha * 255) / float(dataplus -> m_alpha) + .5f);
                if (min_alpha > 255) min_alpha = 255;
            }
            // calc
            for (; y != dataplus -> m_height; ++ y)
                for (x = 0; x != dataplus -> m_width; ++ x, ++ src)
                    if ((*src >> 24) >= min_alpha) {
                        if (tx1 > x) tx1 = x;
                        if (tx2 < x) tx2 = x;
                        if (ty1 > y) ty1 = y;
                        if (ty2 < y) ty2 = y;
                    }
            if (tx1 == dataplus -> m_width)
                return { 0, 0, 0, 0 };
            return { tx1, ty1, tx2 - tx1 + 1, ty2 - ty1 + 1 };
        }

        if (min_alpha > dataplus -> m_alpha) // all under min_alpha
            return { 0, 0, 0, 0 };
        if (!(m_data_[0] & SrcColorKey)) // all beyond min_alpha
            return { 0, 0, dataplus -> m_width, dataplus -> m_height };

        // calc
        for (; y != dataplus -> m_height; ++ y)
            for (x = 0; x != dataplus -> m_width; ++ x, ++ src)
                if ((*src & 0xffffff) != dataplus -> m_colorkey) {
                    if (tx1 > x) tx1 = x;
                    if (tx2 < x) tx2 = x;
                    if (ty1 > y) ty1 = y;
                    if (ty2 < y) ty2 = y;
                }
        if (tx1 == dataplus -> m_width)
            return { 0, 0, 0, 0 };
        return { tx1, ty1, tx2 - tx1 + 1, ty2 - ty1 + 1 };
    }

    /**
     * @function surface_t::premul_alpha <src/surface.h>
     * @brief Returns a copy of the surface with the
     *     RGB channels pre-multiplied by the alpha channel.
     * @return surface_t
     */
    surface_t surface_t::
    premul_alpha () const noexcept{
        mcl_imagebuf_t* src_dataplus = reinterpret_cast<mcl_imagebuf_t*>(m_dataplus_);
        if (!src_dataplus) return sf_nullptr;

        mcl_simpletls_ns::mcl_spinlock_t lk(src_dataplus -> m_nrtlock, L"surface_t::premul_alpha");
        if (!src_dataplus -> m_width) return sf_nullptr;

        surface_t res({src_dataplus -> m_width, src_dataplus -> m_height}, SrcAlpha);
        mcl_imagebuf_t* dst_dataplus = reinterpret_cast<mcl_imagebuf_t*>(res.m_dataplus_);
        if (!dst_dataplus || !dst_dataplus -> m_width) return sf_nullptr;
        
        // alpha info
        color_t* src = src_dataplus -> m_pbuffer;
        color_t* dst = dst_dataplus -> m_pbuffer;
        color_t* srce = src + src_dataplus -> m_width * src_dataplus -> m_height;
        bool b_useck = m_data_[0] & SrcColorKey;
        bool b_sa    = m_data_[0] & SrcAlpha;
        color_t m_alpha = src_dataplus -> m_alpha << 24;
        color_t m_ck = src_dataplus -> m_colorkey;

        // no per pixel alpha
        if (!b_sa) {
            // if (!b_useck && src_dataplus -> m_alpha == 255)
            //     return sf_nullptr;

            while (src != srce) {
                *dst = *src & 0xffffff;
                *dst = (b_useck && *dst == m_ck) ?
                    0 : mcl::premul_alpha (*dst | m_alpha);
                ++ src, ++ dst;
            }
            return res;
        }

        // no alpha blend
        if (m_alpha == 0xff000000) {
            while (src != srce) {
                *dst = mcl::premul_alpha (*dst);
                ++ src, ++ dst;
            }
            return res;
        }

        // has global alpha
        while (src != srce) {
            *dst = *dst & 0xffffff | ((*dst >> 24) * src_dataplus -> m_alpha / 255) << 24;
            *dst = mcl::premul_alpha (*dst);
            ++ src, ++ dst;
        }
        return res;
    }

}
