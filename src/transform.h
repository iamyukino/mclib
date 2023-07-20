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

    @file src/transform.h
    This is a C++11 header for surfaces transfer.
*/


#ifndef MCL_TRANSFORM
# define MCL_TRANSFORM

# include "mclfwd.h"

namespace
mcl {

   /**
    * @class mcl_transform_t <src/transform.h>
    * @brief module for surfaces transfer
    * 
    * @include transform
    * @ingroup surface
    * @ingroup mclib
    * @{
    */
    class
    mcl_transform_t { public:
        explicit   mcl_transform_t ()                           = default;
                  ~mcl_transform_t ()                           = default;
                   mcl_transform_t (mcl_transform_t const& rhs) = delete;
        mcl_transform_t& operator= (mcl_transform_t const& rhs) = delete;
        operator   void*           () const noexcept;
        bool       operator!       () const noexcept;  

        // flip vertically and horizontally
        surface_t  flip     (surface_t const& surface, bool flip_x, bool flip_y) noexcept;
        // resize to new resolution. smooth_ipt in (0,1,2)
        surface_t  scale    (surface_t const& surface, point2d_t size, short smooth_ipt = 1) noexcept;
        // resize to new resolution, using scalar(s). smooth_ipt in (0,1,2)
        surface_t  scale_by (surface_t const& surface, float factor_x, float factor_y, short smooth_ipt = 1) noexcept;
    };
    extern mcl_transform_t transform; // Module for surfaces transfer.

} // namespace

#endif // MCL_TRANSFORM
