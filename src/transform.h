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
    * @unimplemented
    *     pygame.transform.get_smoothscale_backend()
    *     pygame.transform.set_smoothscale_backend()
    *     pygame.transform.average_surfaces()
    *     pygame.transform.threshold()
    * 
    * @feature
    *     mcl::transform.clip()
    */

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
        surface_t  flip      (surface_t const& surface, bool flip_x, bool flip_y) noexcept;
        // resize to new resolution. smooth_ipt in (0,1,2)
        surface_t  scale     (surface_t const& surface, point2d_t size,
                                point2d_t* offset = nullptr, short smooth_ipt = 1) noexcept;
        // resize to new resolution, using scalar(s). smooth_ipt in (0,1,2)
        surface_t  scale_by  (surface_t const& surface, float factor_x, float factor_y,
                                point2d_t* offset = nullptr, short smooth_ipt = 1) noexcept;
        // rotate an image
        surface_t  rotate    (surface_t const& surface, float angle,
                                point2d_t* offset = nullptr) noexcept;
        // filtered scale and rotation
        surface_t  rotozoom  (surface_t const& surface, float angle, float scale,
                                point2d_t* offset = nullptr) noexcept;
        // specialized image doubler
        surface_t  scale2x   (surface_t const& surface, point2d_t* offset = nullptr) noexcept;
        // gets a copy of an image with an interior area removed
        surface_t  chop      (surface_t const& surface, rect_t rect) noexcept;
        // gets a copy of part of an image
        surface_t  clip      (surface_t const& surface, rect_t rect) noexcept;
        // find edges in a surface using the laplacian algorithm
        surface_t  laplacian (surface_t const& surface) noexcept;
        // finds the average color of a surface
        color_t    average_color (surface_t const& surface) noexcept;
        // grayscale a surface
        surface_t  grayscale (surface_t const& surface) noexcept;

    };
    extern mcl_transform_t transform; // Module for surfaces transfer.

} // namespace

#endif // MCL_TRANSFORM
