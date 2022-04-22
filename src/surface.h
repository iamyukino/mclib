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

    @file src/surface.h
    This is a C++11 header for representing any image.
*/


#ifndef MCL_SURFACE
# define MCL_SURFACE

# include "mclfwd.h"

namespace
mcl {

    /**
     * @class surface_t
     * @brief Class to represent image.
     *
     * @ingroup surface
     * @ingroup images
     * @ingroup mclib
     */
    class
    surface_t {
    public:
        explicit   surface_t (void* = 0, bool b_srcalpha = false) noexcept;
        explicit   surface_t (point2d_t size, bool b_srcalpha = false) noexcept;
                   surface_t (surface_t const& rhs, bool b_srcalpha = false) noexcept;
                   surface_t (surface_t&& rhs, bool b_srcalpha = false) noexcept;
                  ~surface_t () noexcept;
        surface_t& operator= (surface_t const& rhs) noexcept;
        surface_t& operator= (surface_t&& rhs) noexcept;

        operator   void*     () const noexcept;
        bool       operator! () const noexcept;

    private:
        void* m_dataplus_;
    };

} // namespace

#endif // MCL_SURFACE
