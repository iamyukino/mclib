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
    
    mcl_draw_t::operator void* () const
        noexcept{ return const_cast<mcl_draw_t*>(this); }
    
    bool mcl_draw_t::operator! () const
        noexcept{ return false; }

    /**
     * @function mcl_draw_t::rect <src/timer.h>
     * @brief draw a rectangle
     * @param surface: surface to draw on
     * @param color: color to draw with
     * @param recta: rectangle to draw, position and dimensions
     * @param width: used for line thickness or to indicate that
            the rectangle is to be filled (not to be confused with
            the width value of the rect parameter).
            - if(width == 0) then (default) fill the rectangle.
            - if(width > 0) then used for line thickness. 
            - if(width < 0) then nothing will be drawn.
     * @return rect_t: a rect bounding the changed pixels, if
            nothing is drawn the bounding rect's position will be
            the position of the given rect parameter and its width
            and height will be 0.
     */
    rect_t mcl_draw_t::
    rect (surface_t& surface, color_t color, rect_t recta,
        point1d_t width
    ) const noexcept {
        // TODO 
        return width && surface && color ? recta : rect_t{ 0, 0, 0, 0 };
    }

} // namespace    
