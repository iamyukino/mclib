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

    @file src/image.h
    This is a C++11 header for image transfer.
*/


#ifndef MCL_IMAGE
# define MCL_IMAGE

# include "mclfwd.h"
# include <cstdio>

namespace
mcl {

   /**
    * @unimplemented
    *     pygame.image.get_extended()
    *     pygame.image.tostring()
    *     pygame.image.tobytes()
    *     pygame.image.fromstring()
    *     pygame.image.frombytes()
    * 
    * @unfinished
    *     pygame.image.load()
    *     pygame.image.save()
    */

   /**
    * @class mcl_image_t <src/image.h>
    * @brief module for image transfer
    * 
    * @ingroup images
    * @ingroup mclib
    * @{
    */
    class
    mcl_image_t { public:
        explicit      mcl_image_t ()                        = default;
                     ~mcl_image_t ()                        = default;
                      mcl_image_t (mcl_image_t const& rhs) = delete;
        mcl_image_t&  operator=   (mcl_image_t const& rhs) = delete;
        operator      void*       () const noexcept;
        bool          operator!   () const noexcept;  

        // load image only supports uncompressed BMP ICO CUR image
        // (currently don't support 1, 2, 4, 8, 16, 24 bits image) 

        // load new image from a file
        surface_t     load        (char const* filename, void* = 0) noexcept;
        // load new image from a file
        surface_t     load        (wchar_t const* filename, void* = 0) noexcept;
        // load new image from a file
        surface_t     load        (char const* filename, point2d_t size) noexcept;
        // load new image from a file
        surface_t     load        (wchar_t const* filename, point2d_t size) noexcept;
        
        // save image only supports uncompressed BMP image
        // (currently don't support 1, 2, 4, 8, 16, 24 bits image) 

        // save an image to file
        bool          save        (surface_t const& surface, char const* filename) noexcept;
        // save an image to file
        bool          save        (surface_t const& surface, wchar_t const* filename) noexcept;
        // save an image to file
        bool          save        (surface_t const& surface, FILE* fileobj) noexcept;

        // create a new surface that shares data inside a bytes buffer
        surface_t     frombuffer  (color_t* bytes, point2d_t size) noexcept;
    };
    extern mcl_image_t image; // Module for image transfer.

} // namespace

#endif // MCL_IMAGE
