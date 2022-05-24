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
    
    @file src/draw.h
    This is a C++11 header for drawing shapes.
*/


#ifndef MCL_DRAW
# define MCL_DRAW

# include "colors.h"

namespace
mcl {

   /**
    * @class mcl_draw_t <src/draw.h>
    * @brief module for drawing shapes
    * 
    * @ingroup draw
    * @ingroup images
    * @ingroup display
    * @{
    */
    class
    mcl_draw_t { public:
        explicit       mcl_draw_t ()                      = default;
                      ~mcl_draw_t ()                      = default;
                       mcl_draw_t (mcl_draw_t const& rhs) = delete;
        mcl_draw_t&    operator=  (mcl_draw_t const& rhs) = delete;
        operator       void*      () const noexcept;
        bool           operator!  () const noexcept; 

        rect_t         rect       ( surface_t& surface, color_t color,
                                    rect_t recta, point1d_t width = 0,
                                    point1d_t border_radius = 0,
                                    point1d_t border_top_left_radius = -1,
                                    point1d_t border_top_right_radius = -1,
                                    point1d_t border_bottom_left_radius = -1,
                                    point1d_t border_bottom_right_radius = -1
                                ) const noexcept;
    };
    extern mcl_draw_t draw; // Module for drawing shapes.

} // namespace

#endif // MCL_DRAW
