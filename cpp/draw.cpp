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
    
    @file cpp/timer.cpp
    This is a C++11 implementation file for monitoring time.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/draw.h"
#include "../src/surface.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {
    
    // Module for drawing shapes.
    mcl_draw_t draw;

    mcl_draw_t::operator void* () const noexcept{ return const_cast<mcl_draw_t*>(this); }
    bool mcl_draw_t::operator! () const noexcept{ return false; }

   /**
    * @function mcl_transform_t::draw <src/transform.cpp>
    * @brief draws a rectangle on the given surface
    * @param[in] surface: surface to draw on
    * @param[in] color: color to draw with
    * @param[in] recta: rectangle to draw, position and dimensions
    * @param[in] width: (optional) used for line thickness or to
    *   indicate that the rectangle is to be filled (not to be
    *   confused with the width value of the rect parameter).
    *   - if(width == 0) then (default) fill the rectangle.
    *   - if(width > 0) then used for line thickness. 
    *   - if(width < 0) then nothing will be drawn.
    * @param[in] border_radius: (optional) used for drawing rectangle
    *   with rounded corners. The supported range is [0,
    *   min(height, width) / 2], with 0 representing a rectangle
    *   without rounded corners.
    * @param[in] border_top_left_radius: (optional) used for setting
    *   the value of top left border. If you don't set this value,
    *   it will use the border_radius value.
    * @param[in] border_top_right_radius: (optional) used for setting
    *   the value of top right border. If you don't set this value,
    *   it will use the border_radius value.
    * @param[in] border_bottom_left_radius: (optional) used for setting
    *   the value of bottom left border. If you don't set this
    *   value, it will use the border_radius value.
    * @param[in] border_bottom_right_radius: (optional) used for
    *   setting the value of bottom right border. If you don't set
    *   this value, it will use the border_radius value.
    * @return rect_t: a rect bounding the changed pixels, if
    *   nothing is drawn the bounding rect's position will be
    *   the position of the given rect parameter and its width
    *   and height will be 0.
    */
    rect_t mcl_draw_t::
    rect (surface_t& surface, color_t color, rect_t recta) noexcept{
        return surface.fill (color, recta, blend.Alpha_rgba);
    }
    /*
    rect_t mcl_draw_t::
    rect (surface_t& surface, color_t color, rect_t recta,
        point1d_t width, point1d_t border_radius = 0,
        point1d_t border_top_left_radius = -1,
        point1d_t border_top_right_radius = -1,
        point1d_t border_bottom_left_radius = -1,
        point1d_t border_bottom_right_radius = -1
    ) noexcept {
        return { 0, 0, 0, 0 };
    }
    */

} // namespace    
