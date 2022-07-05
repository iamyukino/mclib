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

    @file src/bufferproxy.cpp
    This is a C++11 implementation file for exporting a surface 
    buffer through an array protocol.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/surface.h"
#include "../src/bufferproxy.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    struct mcl_proxy_impl_t {
        surface_t* surf;
        size_t refcnt;
    };
    
    /**
     * @function bufferproxy_t::bufferproxy_t <src/bufferproxy.h>
     * @brief Constructor.
     * @return none
     */
    bufferproxy_t::
    bufferproxy_t (surface_t* surface) noexcept
      : m_dataplus_ (0), m_data_(0) {
        if (!surface) return ;
        surface -> lock ();
        
        mcl_imagebuf_t* pbuf = mcl_get_surface_dataplus (surface);
        if (!pbuf) return ;

        mcl_proxy_impl_t* pimpl = new (std::nothrow) mcl_proxy_impl_t;
        if (!pimpl) {
            surface -> unlock ();
            return ;
        }
        pimpl -> surf = surface;
        pimpl -> refcnt = 1;
        m_data_ = pbuf -> m_pbuffer;
        
        m_dataplus_ = pimpl;
    }

    /**
     * @function bufferproxy_t::bufferproxy_t <src/bufferproxy.h>
     * @brief Copy constructor.
     * @return none
     */
    bufferproxy_t::
    bufferproxy_t(bufferproxy_t const& rhs) noexcept
        : m_dataplus_(rhs.m_dataplus_), m_data_(rhs.m_data_) {
        if (m_dataplus_)
            ++ reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_) -> refcnt;
    }

    /**
     * @function bufferproxy_t::bufferproxy_t <src/bufferproxy.h>
     * @brief Move constructor.
     * @return none
     */
    bufferproxy_t::
    bufferproxy_t (bufferproxy_t&& rhs) noexcept
      : m_dataplus_ (rhs.m_dataplus_), m_data_(rhs.m_data_) {
        rhs.m_dataplus_ = 0;
    }
    
    /**
     * @function bufferproxy_t::~bufferproxy_t <src/bufferproxy.h>
     * @brief Destructor.
     * @return none
     */
    bufferproxy_t::
    ~bufferproxy_t () noexcept {
        if (!m_dataplus_) return ;

        mcl_proxy_impl_t*& pimpl =
            *reinterpret_cast<mcl_proxy_impl_t**>(&m_dataplus_);
        if (!-- pimpl -> refcnt) {
            pimpl -> surf -> unlock ();
            delete pimpl;
        }
        
        m_dataplus_ = nullptr;
    }

    /**
     * @function bufferproxy_t::length <src/bufferproxy.h>
     * @brief Get the size of the buffer.
     * @return size_t: The size, in bytes, of the exported buffer.
     */
    size_t bufferproxy_t::
    length () const noexcept {
        if (!m_dataplus_) return 0;

        mcl_imagebuf_t* psurf = mcl_get_surface_dataplus (
            reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_) -> surf);
        
        return static_cast<size_t>(psurf -> m_width)
             * static_cast<size_t>(psurf -> m_height) << 2;
    }
    
    /**
     * @function bufferproxy_t::get_surface <src/bufferproxy.h>
     * @brief Return wrapped exporting object.
     * @return The Surface which returned the BufferProxy object 
     *    or the object passed to a BufferProxy call.
     */
    surface_t& bufferproxy_t::
    get_surface () const noexcept {
        if (!m_dataplus_)
            return *reinterpret_cast<surface_t*>(reinterpret_cast<void*>(0));
        return *reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_) -> surf;
    }

    /**
     * @function bufferproxy_t::operator= <src/bufferproxy.h>
     * @brief Move assignment operator.
     * @return bufferproxy_t&
     */
    bufferproxy_t& bufferproxy_t::
    operator= (bufferproxy_t&& rhs) noexcept {
        if (this -> m_dataplus_ == rhs.m_dataplus_) {
            m_data_ = rhs.m_data_;
            rhs.m_data_ = nullptr;
            return *this;
        }
        if (m_dataplus_ && !-- reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_) -> refcnt)
            delete reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_);
        
        m_dataplus_ = rhs.m_dataplus_;
        rhs.m_dataplus_ = nullptr;
        return *this;
    }
    
    /**
     * @function bufferproxy_t::operator= <src/bufferproxy.h>
     * @brief Copy assignment operator.
     * @return bufferproxy_t&
     */
    bufferproxy_t& bufferproxy_t::
    operator= (bufferproxy_t const& rhs) noexcept {
        if (this -> m_dataplus_ == rhs.m_dataplus_) {
            m_data_ = rhs.m_data_;
            return *this;
        }
        if (m_dataplus_ && !-- reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_) -> refcnt)
            delete reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_);
        
        m_dataplus_ = rhs.m_dataplus_;
        m_data_ = rhs.m_data_;
        if (m_dataplus_)
            ++ reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_) -> refcnt;
        return *this;
    }

    /**
     * @function bufferproxy_t::operator++ <src/bufferproxy.h>
     * @brief Postfix increment operator.
     * @return bufferproxy_t&
     */
    bufferproxy_t bufferproxy_t::
    operator++ (int) noexcept {
        bufferproxy_t tmp = *this;
        ++ m_data_;
        return tmp;
    }

    /**
     * @function bufferproxy_t::operator-- <src/bufferproxy.h>
     * @brief Postfix decrement operator.
     * @return bufferproxy_t&
     */
    bufferproxy_t bufferproxy_t::
    operator-- (int) noexcept {
        bufferproxy_t tmp = *this;
        -- m_data_;
        return tmp;
    }

    /**
     * @function bufferproxy_t::operator+ <src/bufferproxy.h>
     * @brief Addition operator.
     * @return bufferproxy_t
     */
    bufferproxy_t bufferproxy_t::
    operator+ (point1d_t rhs) const noexcept {
        bufferproxy_t tmp = *this;
        tmp += rhs;
        return tmp;
    }
    
    /**
     * @function bufferproxy_t::operator- <src/bufferproxy.h>
     * @brief Subtraction operator.
     * @return bufferproxy_t
     */
    bufferproxy_t bufferproxy_t::
    operator- (point1d_t rhs) const noexcept {
        bufferproxy_t tmp = *this;
        tmp -= rhs;
        return tmp;
    }
    
    // foreach
    bufferproxy_t bufferproxy_t::
    end () const noexcept {
        if (!m_dataplus_) return begin ();
        
        mcl_proxy_impl_t* pimpl =
            reinterpret_cast<mcl_proxy_impl_t*>(m_dataplus_);
        mcl_imagebuf_t* psurf = mcl_get_surface_dataplus(pimpl -> surf);
        
        bufferproxy_t tmp = *this;
        tmp.m_data_ = psurf -> m_pbuffer +
            static_cast<size_t>(psurf -> m_width) * psurf -> m_height;
        return tmp;
    }

}
