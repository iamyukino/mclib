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

    @file src/bufferproxy.h
    This is a C++11 header for exporting a surface buffer
    through an array protocol.
*/


#ifndef MCL_BUFFERPROXY
# define MCL_BUFFERPROXY

# include "mclfwd.h"

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4365)
#endif

# include <cstdlib>
# include <ostream>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {
    
    /**
     * @class bufferproxy_t
     * @brief Class to export a surface buffer through
     *    an array protocol. You can use this class as
     *    a pointer to the surface buffer.
     * @ingroup surface
     * @ingroup mclib
     */
    class
    bufferproxy_t {
    public:
                       bufferproxy_t (bufferproxy_t&& rhs) noexcept;
                       bufferproxy_t (bufferproxy_t const& rhs) noexcept;
                      ~bufferproxy_t () noexcept;
        
    public:
        // Return wrapped exporting object.
        surface_t&     parent () const noexcept;
        // The size, in bytes, of the exported buffer.
        size_t         length () const noexcept;
        
    public:
        // You can use this class as a pointer or an array.
        inline bufferproxy_t   begin      () const noexcept{ return *this; }
        bufferproxy_t          end        () const noexcept;
        inline color_t&        operator[] (point1d_t offset) const noexcept{ return m_data_[offset]; }
        inline color_t&        operator*  () const noexcept{ return *m_data_; }
        inline bufferproxy_t&  operator++ () noexcept{ ++ m_data_; return *this; }
        inline bufferproxy_t&  operator-- () noexcept{ -- m_data_; return *this; }
        bufferproxy_t          operator++ (int) noexcept;
        bufferproxy_t          operator-- (int) noexcept;
        inline bufferproxy_t&  operator+= (point1d_t rhs) noexcept{ m_data_ += rhs; return *this; }
        inline bufferproxy_t&  operator-= (point1d_t rhs) noexcept{ m_data_ -= rhs; return *this; }
        bufferproxy_t          operator+  (point1d_t rhs) const noexcept;
        bufferproxy_t          operator-  (point1d_t rhs) const noexcept;
        friend bufferproxy_t   operator+  (point1d_t lhs, bufferproxy_t const& rhs) noexcept{ return rhs + lhs; }
        friend bufferproxy_t   operator-  (point1d_t lhs, bufferproxy_t const& rhs) noexcept{ return rhs - lhs; }
        
        bufferproxy_t& operator=  (bufferproxy_t&& rhs) noexcept;
        bufferproxy_t& operator=  (bufferproxy_t const& rhs) noexcept;
        explicit       operator color_t* () const noexcept{ return m_data_; }
        inline bool    operator!  () const noexcept{ return !m_data_; }
        inline bool    operator== (bufferproxy_t const& rhs) const noexcept{ return m_data_ == rhs.m_data_; }
        inline bool    operator!= (bufferproxy_t const& rhs) const noexcept{ return m_data_ != rhs.m_data_; }
        inline bool    operator<  (bufferproxy_t const& rhs) const noexcept{ return m_data_ < rhs.m_data_;  }
        inline bool    operator>  (bufferproxy_t const& rhs) const noexcept{ return m_data_ > rhs.m_data_;  }
        inline bool    operator<= (bufferproxy_t const& rhs) const noexcept{ return m_data_ <= rhs.m_data_; }
        inline bool    operator>= (bufferproxy_t const& rhs) const noexcept{ return m_data_ >= rhs.m_data_; }
        template<typename T> friend constexpr std::basic_ostream<T>&
        operator<< (std::basic_ostream<T>& os, bufferproxy_t const& rhs) noexcept{ return os << rhs.m_data_; }

    private:
        // for surface_t::get_buffer()
        explicit bufferproxy_t (surface_t* surface) noexcept;
        friend class surface_t;
        
    private:
        void* m_dataplus_;
        color_t* m_data_;
    };

} // namespace

#endif // MCL_BUFFERPROXY
