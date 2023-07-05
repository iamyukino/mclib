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
#include "../src/colors.h"
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

    // @brief Add two colors together. use alpha.
#define MCL_COLOR_ADD(dst, sa, srsa, sgsa, sbsa, bIgnoreDstAlpha) \
    do { \
        color_t da  = bIgnoreDstAlpha ? 255 - sa : (*dst >> 24) * (255 - sa) / 255; \
        color_t t = sa + da; \
        color_t r = (getr4rgb(*dst) * da + srsa) / t; \
        color_t g = (getg4rgb(*dst) * da + sgsa) / t; \
        color_t b = (getb4rgb(*dst) * da + sbsa) / t; \
        *dst = (t << 24) | (r << 16) | (g << 8) | b; \
    } while (0)

   /**
    * @function mcl_draw_t::rect <src/timer.h>
    * @brief draw a rectangle
    * @param surface: surface to draw on
    * @param color: color to draw with
    * @param recta: rectangle to draw, position and dimensions
    * @param width: (optional) used for line thickness or to
    *   indicate that the rectangle is to be filled (not to be
    *   confused with the width value of the rect parameter).
    *   - if(width == 0) then (default) fill the rectangle.
    *   - if(width > 0) then used for line thickness. 
    *   - if(width < 0) then nothing will be drawn.
    * @param border_radius: (optional) used for drawing rectangle
    *   with rounded corners. The supported range is [0,
    *   min(height, width) / 2], with 0 representing a rectangle
    *   without rounded corners.
    * @param border_top_left_radius: (optional) used for setting
    *   the value of top left border. If you don't set this value,
    *   it will use the border_radius value.
    * @param border_top_right_radius: (optional) used for setting
    *   the value of top right border. If you don't set this value,
    *   it will use the border_radius value.
    * @param border_bottom_left_radius: (optional) used for setting
    *   the value of bottom left border. If you don't set this
    *   value, it will use the border_radius value.
    * @param border_bottom_right_radius: (optional) used for
    *   setting the value of bottom right border. If you don't set
    *   this value, it will use the border_radius value.
    *
    * @return rect_t: a rect bounding the changed pixels, if
    *   nothing is drawn the bounding rect's position will be
    *   the position of the given rect parameter and its width
    *   and height will be 0.
    */
    rect_t mcl_draw_t::
    rect (surface_t& surface, color_t color, rect_t recta,
        point1d_t width, point1d_t border_radius,
        point1d_t border_top_left_radius, point1d_t border_top_right_radius,
        point1d_t border_bottom_left_radius, point1d_t border_bottom_right_radius
    ) const noexcept {
        // preparation
        if (width < 0 || !(&surface && recta.w && recta.h && (color & 0xff000000)))
            return { recta.x, recta.y, 0, 0 };
                // display.get_surface() may return reference of 0
        
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&surface);
        char*           data     = mcl_get_surface_data (&surface);
        if (!dataplus)
            return { recta.x, recta.y, 0, 0 };

        // impact rect
        point1d_t x = recta.x, y = recta.y, w = recta.w, h = recta.h;
        point1d_t hf1 = 0, hf2 = 0;
        if (w < 0) x += w + 1, w = -w;
        if (h < 0) y += h + 1, h = -h;
        
        if (width > 1) {
        // add line width to rect
            hf1 = (width - 1) >> 1;
            hf2 = width >> 1;
            if (hf1 + hf2 + 2 >= w || hf1 + hf2 + 2 >= h)
                width = 0;
            x -= hf1;       y -= hf1;
            w += hf1 + hf2; h += hf1 + hf2;
        }
        // completely out of window
        if (x + w <= 0 || y + h <= 0)
            return { recta.x, recta.y, 0, 0 };

        // start drawing
        mcl_simpletls_ns::mcl_spinlock_t lock (dataplus -> m_nrtlock, L"mcl_draw_t::rect");
        if (!dataplus -> m_width)
            return { recta.x, recta.y, 0, 0 };

        // completely out of window
        if (x >= dataplus -> m_width || y >= dataplus -> m_height)
            return { recta.x, recta.y, 0, 0 };
        
        if (!width) {
        // (default) fill the rectangle
            // out of window
            if (x + w > dataplus -> m_width)  w = dataplus -> m_width - x;
            if (y + h > dataplus -> m_height) h = dataplus -> m_height - y;
            if (x < 0) w += x, x = 0;
            if (y < 0) h += y, y = 0; 

            color_t *i = dataplus -> m_pbuffer + static_cast<long long>(y) * dataplus -> m_width + x, *j = 0;
            color_t *i0 = static_cast<long long>(h) * dataplus -> m_width + i, *j0 = 0;

            color_t sa = color >> 24;
            if (0xff == sa) {
            // no alpha
                for (; i != i0; i += dataplus -> m_width)
                    for (j = i, j0 = i + w; j != j0; ++ j)
                        *j = color;
            } else {
            // have alpha
                color_t srsa = getr4rgb(color) * sa;
                color_t sgsa = getg4rgb(color) * sa;
                color_t sbsa = getb4rgb(color) * sa;
                if (data[0]) { // need dst alpha
                    for (; i != i0; i += dataplus -> m_width)
                        for (j = i, j0 = i + w; j != j0; ++ j)
                            MCL_COLOR_ADD (j, sa, srsa, sgsa, sbsa, false);
                } else { // ignore dst alpha
                    for (; i != i0; i += dataplus -> m_width)
                        for (j = i, j0 = i + w; j != j0; ++ j)
                            MCL_COLOR_ADD (j, sa, srsa, sgsa, sbsa, true);
                }
            }
            return { x, y, w, h };
        }
        // TODO: 圆角和边框 
        hf1 =
        border_top_left_radius = border_top_right_radius = 
        border_bottom_left_radius = border_bottom_right_radius =
        border_radius;

        return { x, y, w, h }; // TODO: 未处理边界外情况
    }

#undef MCL_COLOR_ADD

} // namespace    
