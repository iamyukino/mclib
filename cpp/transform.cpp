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

    @file cpp/transform.cpp
    This is a C++11 implementation file for surfaces transfer.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/transform.h"
#include "../src/surface.h"
#include "mcl_control.h"
#include <cmath>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for surface transfer.
    mcl_transform_t transform;
    
    mcl_transform_t::operator void* () const noexcept{ return const_cast<mcl_transform_t*>(this); }
    bool mcl_transform_t::operator! () const noexcept{ return false; }

    /**
     * @function mcl_transform_t::flip <src/transform.cpp>
     * @brief flip vertically and horizontally
     * @param[in] surface
     * @param[in] flip_x, flip_y: whether to flip each axis
     * @return surface_t
     */
    surface_t mcl_transform_t::
    flip (surface_t const& surface, bool flip_x, bool flip_y) noexcept{
        surface_t res (surface); // no lock required
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&res);
        if (!dataplus || !dataplus -> m_width) return sf_nullptr;
        
        if (dataplus -> m_width  == 1) flip_x = false;
        if (dataplus -> m_height == 1) flip_y = false;
        if (!(flip_x || flip_y)) return res;
        
        point1d_t sdx = 1, sdy = 0, ddx = -1, ddy = 0;
        color_t *src = dataplus -> m_pbuffer, *dst = dataplus -> m_pbuffer;
        color_t *ex  = dataplus -> m_pbuffer, *ey  = dataplus -> m_pbuffer;
        
        // map direction
        if (flip_x && flip_y) {
            ey += (dataplus -> m_height >> 1) * dataplus -> m_width;
            dst += dataplus -> m_height * dataplus -> m_width - 1;
        } else if (flip_x) {
            sdy = dataplus -> m_width - (dataplus -> m_width >> 1);
            ddy = dataplus -> m_width + (dataplus -> m_width >> 1);
            ex -= sdy;
            ey += dataplus -> m_height * dataplus -> m_width;
            dst += dataplus -> m_width - 1;
        } else {
            ddx = 1;
            ddy = -(dataplus -> m_width << 1);
            ey += (dataplus -> m_height >> 1) * dataplus -> m_width;
            dst += (dataplus -> m_height - 1) * dataplus -> m_width;
        }
        
        // flip surface
        color_t temp = 0;
        for (; src != ey; src += sdy, dst += ddy)
            for (ex += dataplus -> m_width; src != ex; src += sdx, dst += ddx)
                temp = *dst, *dst = *src, *src = temp;
        
        // middle line
        if (flip_x && flip_y && (dataplus -> m_height & 1))
            for (ex += dataplus -> m_width >> 1; src != ex; ++ src, -- dst)
                temp = *dst, *dst = *src, *src = temp;
        return res;
    }

    /**
     * @function mcl_transform_t::scale <src/transform.cpp>
     * @brief resize to new resolution
     * @param[in] surface
     * @param[in] size: new size
     * @param[in] ipt: interpolation times
     * @return surface_t
     */
    surface_t mcl_transform_t::
    scale (surface_t const& surface, point2d_t size, point2d_t* offset, short smooth_ipt) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        char*           data     = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!(size.x > 0 && size.y > 0 && dataplus)) return sf_nullptr;
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::scale");
        if (!dataplus -> m_width) return sf_nullptr;
        
        // create compatible surface
        surface_t res (size, data[0]);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char*           res_data     = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;
        
        // copy alpha info
        res_data[0] = data[0];
        res_dataplus -> m_colorkey = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;
        bool b_sa = res_data[0] & surface_t::SrcAlpha;
        bool b_ck = res_data[0] & surface_t::SrcColorKey;
        color_t m_ck = res_dataplus -> m_colorkey;
        
        // scaling calc
        color_t *src = dataplus -> m_pbuffer, *dst = res_dataplus -> m_pbuffer;
        double kx = double(size.x) / double(dataplus -> m_width);
        double ky = double(size.y) / double(dataplus -> m_height);
        
        if (smooth_ipt >= 2 && (size.x < 3 || size.y < 3))
            smooth_ipt = 1;
        if (offset) {
            offset -> x = (dataplus -> m_width  - size.x) >> 1;
            offset -> y = (dataplus -> m_height - size.y) >> 1;
        }
        
        if (smooth_ipt <= 0) {
        // nearest neighbor
            point1d_t i = 0, j = 0, y = 0;
            for (; i != size.y; ++ i, dst += size.x) {
                y = point1d_t(double(i) / ky + .5f);
                src = dataplus -> m_pbuffer + y * dataplus -> m_width;
                for (j = 0; j != size.x; ++ j)
                    dst[j] = src[point1d_t(double(j) / kx + .5f)];
            }
        } else if (smooth_ipt == 1) {
        // bilinear interpolation
            // lerp ratio
            point1d_t i = 0, j = 0;
            point1d_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            double xm = 0.f, ym = 0.f;
            double dx = 0.f, dy = 0.f, fdx = 0.f, fdy = 0.f;
            // color value
            color_t cs1 = 0, cs2 = 0, cs3 = 0, cs4 = 0;
            double  fca = 1.f, fcr = 0.f, fcg = 0.f, fcb = 0.f;
            double  a1 = 0.f, a2 = 0.f, a3 = 0.f, a4 = 0.f;

            // start interpolation
            for (; i != size.y; ++ i, dst += size.x) {
                ym = double(i) / ky;
                y1 = point1d_t(ym), y2 = y1 + 1;
                dy = ym - double(y1), fdy = 1.f - dy;
                if (y2 == dataplus -> m_height)
                    y2 = dataplus -> m_height - 1;

                for (j = 0; j != size.x; ++ j) {
                    xm = double(j) / kx;
                    x1 = point1d_t(xm), x2 = x1 + 1;
                    dx = xm - double(x1), fdx = 1.f - dx;
                    if (x2 == dataplus -> m_width)
                        x2 = dataplus -> m_width - 1;
                    
                    // lerp pixel
                    cs1 = src[y1 * dataplus -> m_width + x1];
                    cs2 = src[y1 * dataplus -> m_width + x2];
                    cs3 = src[y2 * dataplus -> m_width + x1];
                    cs4 = src[y2 * dataplus -> m_width + x2];

                    a1 = fdx * fdy, a2 = dx * fdy;
                    a3 = fdx * dy,  a4 = dx * dy;

                    // premult alpha
                    if (b_sa) {
                        a1 *= double(cs1 >> 24) / 255.f;
                        a2 *= double(cs2 >> 24) / 255.f;
                        a3 *= double(cs3 >> 24) / 255.f;
                        a4 *= double(cs4 >> 24) / 255.f;
                        fca = a1 + a2 + a3 + a4; // alpha test
                        if (fca <= .0f) { dst[j] = 0; continue; }
                    } else if (b_ck) {
                        if ((cs1 & 0xffffff) == m_ck) a1 = 0;
                        if ((cs2 & 0xffffff) == m_ck) a2 = 0;
                        if ((cs3 & 0xffffff) == m_ck) a3 = 0;
                        if ((cs4 & 0xffffff) == m_ck) a4 = 0;
                        fca = a1 + a2 + a3 + a4; // alpha test
                        if (fca < .5f) { dst[j] = m_ck; continue; }
                    }

                    // calc rgb value & unpremult alpha
                    fcr = double((cs1 >> 16) & 0xff) * a1 +
                          double((cs2 >> 16) & 0xff) * a2 +
                          double((cs3 >> 16) & 0xff) * a3 +
                          double((cs4 >> 16) & 0xff) * a4;
                    fcg = double((cs1 >> 8)  & 0xff) * a1 +
                          double((cs2 >> 8)  & 0xff) * a2 +
                          double((cs3 >> 8)  & 0xff) * a3 +
                          double((cs4 >> 8)  & 0xff) * a4;
                    fcb = double( cs1        & 0xff) * a1 +
                          double( cs2        & 0xff) * a2 +
                          double( cs3        & 0xff) * a3 +
                          double( cs4        & 0xff) * a4;
                    fcr /= fca, fcg /= fca, fcb /= fca;

                    // update rgba
                    dst[j] = (b_sa ? color_t(fca * 255.f + .5f) : 255) << 24 |
                        color_t(fcr + .5f) << 16 | color_t(fcg + .5f) << 8 | color_t(fcb + .5f);
                }
            }
        } else {
        // bicubic_interpolation
            // lerp ratio
            point1d_t i = 0, j = 0;
            point1d_t x0 = 0, x1 = 0, x2 = 0, x3 = 0;
            point1d_t y0 = 0, y1 = 0, y2 = 0, y3 = 0;
            double xm = 0, ym = 0;
            double dx0 = 0.f, dx1 = 0.f, dx2 = 0.f, dx3 = 0.f;
            double dy0 = 0.f, dy1 = 0.f, dy2 = 0.f, dy3 = 0.f;
            double dx = 0.f, fdx = 0.f, dy = 0.f, fdy = 0.f;
            // color value
            color_t *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;
            double fca = 1.f, fcr = 0.f, fcg = 0.f, fcb = 0.f;
            color_t pa = 0, pr = 0, pg = 0, pb = 0;
            double a01 = 0.f, a02 = 0.f, a03 = 0.f, a04 = 0.f;
            double a05 = 0.f, a06 = 0.f, a07 = 0.f, a08 = 0.f;
            double a09 = 0.f, a10 = 0.f, a11 = 0.f, a12 = 0.f;
            double a13 = 0.f, a14 = 0.f, a15 = 0.f, a16 = 0.f;
            
            // interpolation function
            auto fpBiCubicPoly = []
            (double x) -> double {
                double const abs_x = x > 0.f ? x : -x;
                double constexpr a = -.5f;
                return (abs_x <= 1.f) ? 
                    (a + 2.f) * (abs_x * abs_x * abs_x) -
                    (a + 3.f) * (abs_x * abs_x) + 1: (
                       (abs_x <  2.f) ?
                    (a) * (abs_x * abs_x * abs_x) - (5.f * a) * (abs_x * abs_x) +
                    (8.f * a) * abs_x - 4.f * a : 0.f);
            };

            // start interpolation
            for (; i != size.y; ++ i, dst += size.x) {
                ym = double(i) / double(ky);
                y0 = point1d_t(ym) - 1, y1 = point1d_t(ym);
                y2 = point1d_t(ym) + 1, y3 = point1d_t(ym) + 2;

                dy0 = fpBiCubicPoly (ym - double(y0));
                dy1 = fpBiCubicPoly (ym - double(y1));
                dy2 = fpBiCubicPoly (ym - double(y2));
                dy3 = fpBiCubicPoly (ym - double(y3));

                for (j = 0; j != size.x; ++ j) {
                    xm = double(j) / double(kx);
                    x0 = point1d_t(xm) - 1, x1 = point1d_t(xm);
                    x2 = point1d_t(xm) + 1, x3 = point1d_t(xm) + 2;
                    
                    // test if pixels exist
                    bool b_border = (x0 >= 0 && x3 < dataplus -> m_width
                        && y0 >= 0 && y3 < dataplus -> m_height);
                    if (b_border) {

                        dx0 = fpBiCubicPoly (xm - double(x0));
                        dx1 = fpBiCubicPoly (xm - double(x1));
                        dx2 = fpBiCubicPoly (xm - double(x2));
                        dx3 = fpBiCubicPoly (xm - double(x3));

                        // lerp pixel
                        q0 = src + y0 * dataplus -> m_width;
                        q1 = src + y1 * dataplus -> m_width;
                        q2 = src + y2 * dataplus -> m_width;
                        q3 = src + y3 * dataplus -> m_width;
                        
                        a01 = dx0 * dy0, a02 = dx0 * dy1, a03 = dx0 * dy2, a04 = dx0 * dy3;
                        a05 = dx1 * dy0, a06 = dx1 * dy1, a07 = dx1 * dy2, a08 = dx1 * dy3;
                        a09 = dx2 * dy0, a10 = dx2 * dy1, a11 = dx2 * dy2, a12 = dx2 * dy3;
                        a13 = dx3 * dy0, a14 = dx3 * dy1, a15 = dx3 * dy2, a16 = dx3 * dy3;
                        
                        // premult alpha
                        if (b_sa) {
                            a01 *= double(q0[x0] >> 24) / 255.f;
                            a02 *= double(q1[x0] >> 24) / 255.f;
                            a03 *= double(q2[x0] >> 24) / 255.f;
                            a04 *= double(q3[x0] >> 24) / 255.f;
                            a05 *= double(q0[x1] >> 24) / 255.f;
                            a06 *= double(q1[x1] >> 24) / 255.f;
                            a07 *= double(q2[x1] >> 24) / 255.f;
                            a08 *= double(q3[x1] >> 24) / 255.f;
                            a09 *= double(q0[x2] >> 24) / 255.f;
                            a10 *= double(q1[x2] >> 24) / 255.f;
                            a11 *= double(q2[x2] >> 24) / 255.f;
                            a12 *= double(q3[x2] >> 24) / 255.f;
                            a13 *= double(q0[x3] >> 24) / 255.f;
                            a14 *= double(q1[x3] >> 24) / 255.f;
                            a15 *= double(q2[x3] >> 24) / 255.f;
                            a16 *= double(q3[x3] >> 24) / 255.f;
                            fca = a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 + 
                                  a09 + a10 + a11 + a12 + a13 + a14 + a15 + a16;
                            if (fca <= .0f) { dst[j] = 0; continue; } // alpha test
                        } else if (b_ck) {
                            if ((q0[x0] & 0xffffff) == m_ck) a01 = 0;
                            if ((q1[x0] & 0xffffff) == m_ck) a02 = 0;
                            if ((q2[x0] & 0xffffff) == m_ck) a03 = 0;
                            if ((q3[x0] & 0xffffff) == m_ck) a04 = 0;
                            if ((q0[x1] & 0xffffff) == m_ck) a05 = 0;
                            if ((q1[x1] & 0xffffff) == m_ck) a06 = 0;
                            if ((q2[x1] & 0xffffff) == m_ck) a07 = 0;
                            if ((q3[x1] & 0xffffff) == m_ck) a08 = 0;
                            if ((q0[x2] & 0xffffff) == m_ck) a09 = 0;
                            if ((q1[x2] & 0xffffff) == m_ck) a10 = 0;
                            if ((q2[x2] & 0xffffff) == m_ck) a11 = 0;
                            if ((q3[x2] & 0xffffff) == m_ck) a12 = 0;
                            if ((q0[x3] & 0xffffff) == m_ck) a13 = 0;
                            if ((q1[x3] & 0xffffff) == m_ck) a14 = 0;
                            if ((q2[x3] & 0xffffff) == m_ck) a15 = 0;
                            if ((q3[x3] & 0xffffff) == m_ck) a16 = 0;
                            fca = a01 + a02 + a03 + a04 + a05 + a06 + a07 + a08 + 
                                  a09 + a10 + a11 + a12 + a13 + a14 + a15 + a16;
                            if (fca < .5f) { dst[j] = m_ck; continue; } // alpha test
                        }

                        // calc rgb value & unpremult alpha
                        fcr = 
                            double((q0[x0] >> 16) & 0xff) * a01 +
                            double((q1[x0] >> 16) & 0xff) * a02 +
                            double((q2[x0] >> 16) & 0xff) * a03 +
                            double((q3[x0] >> 16) & 0xff) * a04 +
                            double((q0[x1] >> 16) & 0xff) * a05 +
                            double((q1[x1] >> 16) & 0xff) * a06 +
                            double((q2[x1] >> 16) & 0xff) * a07 +
                            double((q3[x1] >> 16) & 0xff) * a08 +
                            double((q0[x2] >> 16) & 0xff) * a09 +
                            double((q1[x2] >> 16) & 0xff) * a10 +
                            double((q2[x2] >> 16) & 0xff) * a11 +
                            double((q3[x2] >> 16) & 0xff) * a12 +
                            double((q0[x3] >> 16) & 0xff) * a13 +
                            double((q1[x3] >> 16) & 0xff) * a14 +
                            double((q2[x3] >> 16) & 0xff) * a15 +
                            double((q3[x3] >> 16) & 0xff) * a16;
                        fcg = 
                            double((q0[x0] >> 8) & 0xff) * a01 +
                            double((q1[x0] >> 8) & 0xff) * a02 +
                            double((q2[x0] >> 8) & 0xff) * a03 +
                            double((q3[x0] >> 8) & 0xff) * a04 +
                            double((q0[x1] >> 8) & 0xff) * a05 +
                            double((q1[x1] >> 8) & 0xff) * a06 +
                            double((q2[x1] >> 8) & 0xff) * a07 +
                            double((q3[x1] >> 8) & 0xff) * a08 +
                            double((q0[x2] >> 8) & 0xff) * a09 +
                            double((q1[x2] >> 8) & 0xff) * a10 +
                            double((q2[x2] >> 8) & 0xff) * a11 +
                            double((q3[x2] >> 8) & 0xff) * a12 +
                            double((q0[x3] >> 8) & 0xff) * a13 +
                            double((q1[x3] >> 8) & 0xff) * a14 +
                            double((q2[x3] >> 8) & 0xff) * a15 +
                            double((q3[x3] >> 8) & 0xff) * a16;
                        fcb = 
                            double(q0[x0] & 0xff) * a01 +
                            double(q1[x0] & 0xff) * a02 +
                            double(q2[x0] & 0xff) * a03 +
                            double(q3[x0] & 0xff) * a04 +
                            double(q0[x1] & 0xff) * a05 +
                            double(q1[x1] & 0xff) * a06 +
                            double(q2[x1] & 0xff) * a07 +
                            double(q3[x1] & 0xff) * a08 +
                            double(q0[x2] & 0xff) * a09 +
                            double(q1[x2] & 0xff) * a10 +
                            double(q2[x2] & 0xff) * a11 +
                            double(q3[x2] & 0xff) * a12 +
                            double(q0[x3] & 0xff) * a13 +
                            double(q1[x3] & 0xff) * a14 +
                            double(q2[x3] & 0xff) * a15 +
                            double(q3[x3] & 0xff) * a16;
                        fcr /= fca, fcg /= fca, fcb /= fca;

                        // update rgba
                        pa = fca > 0.f ? color_t(fca * 255.f + .5f) : 0;
                        pr = fcr > 0.f ? color_t(fcr + .5f) : 0;
                        pg = fcg > 0.f ? color_t(fcg + .5f) : 0;
                        pb = fcb > 0.f ? color_t(fcb + .5f) : 0;
                        dst[j] = (pa < 255 ? pa : 255) << 24 | (pr < 255 ? pr : 255) << 16 |
                                 (pg < 255 ? pg : 255) << 8  | (pb < 255 ? pb : 255);
                    } else {
                        if (x2 == dataplus -> m_width)  x2 = dataplus -> m_width  - 1;
                        if (y2 == dataplus -> m_height) y2 = dataplus -> m_height - 1;
                        dx = xm - double(x1), fdx = 1.f - dx;
                        dy = ym - double(y1), fdy = 1.f - dy; 

                        // lerp pixel
                        q0 = src + (y1 * dataplus -> m_width + x1);
                        q1 = src + (y1 * dataplus -> m_width + x2);
                        q2 = src + (y2 * dataplus -> m_width + x1);
                        q3 = src + (y2 * dataplus -> m_width + x2);

                        a01 = fdx * fdy, a02 = dx * fdy;
                        a03 = fdx * dy,  a04 = dx * dy;

                        // premult alpha
                        if (b_sa) {
                            a01 *= double(*q0 >> 24) / 255.f;
                            a02 *= double(*q1 >> 24) / 255.f;
                            a03 *= double(*q2 >> 24) / 255.f;
                            a04 *= double(*q3 >> 24) / 255.f;
                            fca = a01 + a02 + a03 + a04; // alpha test
                            if (fca <= .0f) { dst[j] = 0; continue; }
                        } else if (b_ck) {
                            if ((*q0 & 0xffffff) == m_ck) a01 = 0;
                            if ((*q1 & 0xffffff) == m_ck) a02 = 0;
                            if ((*q2 & 0xffffff) == m_ck) a03 = 0;
                            if ((*q3 & 0xffffff) == m_ck) a04 = 0;
                            fca = a01 + a02 + a03 + a04; // alpha test
                            if (fca < .5f) { dst[j] = m_ck; continue; }
                        }

                        // calc rgb value & unpremult alpha
                        fcr = double((*q0 >> 16) & 0xff) * a01 +
                              double((*q1 >> 16) & 0xff) * a02 +
                              double((*q2 >> 16) & 0xff) * a03 +
                              double((*q3 >> 16) & 0xff) * a04;
                        fcg = double((*q0 >> 8)  & 0xff) * a01 +
                              double((*q1 >> 8)  & 0xff) * a02 +
                              double((*q2 >> 8)  & 0xff) * a03 +
                              double((*q3 >> 8)  & 0xff) * a04;
                        fcb = double( *q0        & 0xff) * a01 +
                              double( *q1        & 0xff) * a02 +
                              double( *q2        & 0xff) * a03 +
                              double( *q3        & 0xff) * a04;
                        fcr /= fca, fcg /= fca, fcb /= fca;

                        // update rgba
                        dst[j] = (b_sa ? color_t(fca * 255.f + .5f) : 255) << 24 |
                            color_t(fcr + .5f) << 16 | color_t(fcg + .5f) << 8 | color_t(fcb + .5f);
                    }
                }
            }
        }
        return res;
    }
    surface_t mcl_transform_t::
    scale_by (surface_t const& surface, float factor_x, float factor_y, point2d_t* offset, short smooth_ipt) noexcept{
        factor_x *= static_cast<float>(surface.get_width ());
        factor_y *= static_cast<float>(surface.get_height ());
        return scale (surface, { point1d_t(factor_x), point1d_t(factor_y) }, offset, smooth_ipt);
    }
    
    /**
     * @function mcl_transform_t::rotate <src/transform.cpp>
     * @brief rotate an image
     * @param[in] surface
     * @param[in] angle
     * @param[out] offset
     * @return surface_t
     */
    surface_t mcl_transform_t::
    rotate (surface_t const& surface, float angle, point2d_t* offset) noexcept{
        return rotozoom (surface, angle, 1, offset);
    }

    /**
     * @function mcl_transform_t::rotozoom <src/transform.cpp>
     * @brief filtered scale and rotation
     * @param[in] surface
     * @param[in] angle
     * @param[in] scale
     * @param[out] offset
     * @return surface_t
     */
    surface_t mcl_transform_t::
    rotozoom (surface_t const& surface, float angle, float fscale, point2d_t* offset) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        char*           data     = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!(fscale > 0.f && dataplus)) return sf_nullptr;

        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::rotozoom");
        if (!dataplus -> m_width) return sf_nullptr;

        // rotate info
        double constexpr pi = 3.1415926535897932384626433832795f;
        double const    rad = double(angle) * pi / 180.f;
        point2d_t size = {
            point1d_t(.5f + fscale * (
                abs(double(dataplus -> m_width)  * cos(rad))
              + abs(double(dataplus -> m_height) * sin(rad))
            )),
            point1d_t(.5f + fscale * (
                abs(double(dataplus -> m_width)  * sin(rad))
              + abs(double(dataplus -> m_height) * cos(rad))
            ))
        };
        
        // create compatible surface
        surface_t res (size, data[0]);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char*           res_data     = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;
        
        // copy alpha info
        res_data[0] = data[0];
        res_dataplus -> m_colorkey = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;
        bool b_sa = res_data[0] & surface_t::SrcAlpha;
        bool b_ck = res_data[0] & surface_t::SrcColorKey;
        color_t m_ck = res_dataplus -> m_colorkey;

        // scaling calc
        color_t* src = dataplus -> m_pbuffer;
        color_t* dst = res_dataplus -> m_pbuffer;
        double vx0 = double(dataplus -> m_width - 1) / 2.f;
        double vy0 = double(dataplus -> m_height - 1) / 2.f;
        double vx1 = double(size.x - 1) / 2.f;
        double vy1 = double(size.y - 1) / 2.f;
        if (offset) {
            offset -> x = point1d_t(vx0 - vx1 + .5f);
            offset -> y = point1d_t(vy0 - vy1 + .5f);
        }

        // lerp ratio
        double fx0 = 0.f, fy0 = 0.f, fx1 = 0.f, fy1 = 0.f;
        double R = 0.f, sita = 0.f;
        double p = 0.f, q = 0.f, fp = 0.f, fq = 0.f;
        point1d_t x1 = 0, y1 = 0;
        point1d_t ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
        // color value
        color_t *cs1 = 0, *cs2 = 0, *cs3 = 0, *cs4 = 0;
        color_t trans = res_data[0] ? 0 : dataplus -> m_pbuffer[0]; // 0xff00ff00
        double  fca = 1.f, fcr = 0.f, fcg = 0.f, fcb = 0.f;
        double  a1 = 0.f, a2 = 0.f, a3 = 0.f, a4 = 0.f;

        // start interpolation
        for (; y1 != size.y; ++ y1, dst += size.x) {
            fy1 = (double(y1) - vy1) / fscale;
            for (x1 = 0; x1 != size.x; ++ x1) {
                fx1 = (double(x1) - vx1) / fscale;

                // exchange between polar cos and ordinary cos
                R = sqrt(fx1 * fx1 + fy1 * fy1);
                sita = atan2(fy1, fx1) + rad;
                fx0 = R * cos(sita) + vx0; 
                fy0 = R * sin(sita) + vy0;
                ix0 = point1d_t(fx0), ix1 = ix0 + 1;
                iy0 = point1d_t(fy0), iy1 = iy0 + 1;
                
                // lerp ratio
                p = fx0 - double(ix0), fp = 1 - p, fx0 += .5f;
                q = fy0 - double(iy0), fq = 1 - q, fy0 += .5f;

                // test if pixels exist
                bool b_border = (fx0 >= 0.f && fx0 <= double(dataplus -> m_width)
                    && fy0 >= 0.f && fy0 <= double(dataplus -> m_height));
                if (b_border) {
                    // test if pixel is partly out of rect
                    if (fx0 < .5f)                                 ix0 = ix1;
                    if (fx0 >= double(dataplus -> m_width) - .5f)  ix1 = ix0;
                    if (fy0 < .5f)                                 iy0 = iy1;
                    if (fy0 >= double(dataplus -> m_height) - .5f) iy1 = iy0;

                    // lerp pixel
                    cs1 = src + (iy0 * dataplus -> m_width + ix0);
                    cs2 = src + (iy0 * dataplus -> m_width + ix1);
                    cs3 = src + (iy1 * dataplus -> m_width + ix0);
                    cs4 = src + (iy1 * dataplus -> m_width + ix1);
                    
                    a1 = fp * fq, a2 = p * fq;
                    a3 = fp * q,  a4 = p * q;

                    // premult alpha
                    if (b_sa) {
                        a1 *= double(*cs1 >> 24) / 255.f;
                        a2 *= double(*cs2 >> 24) / 255.f;
                        a3 *= double(*cs3 >> 24) / 255.f;
                        a4 *= double(*cs4 >> 24) / 255.f;
                        fca = a1 + a2 + a3 + a4; // alpha test
                        if (fca <= .0f) { dst[x1] = 0; continue; }
                    } else if (b_ck) {
                        if ((*cs1 & 0xffffff) == m_ck) a1 = 0;
                        if ((*cs2 & 0xffffff) == m_ck) a2 = 0;
                        if ((*cs3 & 0xffffff) == m_ck) a3 = 0;
                        if ((*cs4 & 0xffffff) == m_ck) a4 = 0;
                        fca = a1 + a2 + a3 + a4; // alpha test
                        if (fca < .5f) { dst[x1] = m_ck; continue; }
                    }

                    // calc rgb value & unpremult alpha
                    fcr = double((*cs1 >> 16) & 0xff) * a1 +
                          double((*cs2 >> 16) & 0xff) * a2 +
                          double((*cs3 >> 16) & 0xff) * a3 +
                          double((*cs4 >> 16) & 0xff) * a4;
                    fcg = double((*cs1 >> 8)  & 0xff) * a1 +
                          double((*cs2 >> 8)  & 0xff) * a2 +
                          double((*cs3 >> 8)  & 0xff) * a3 +
                          double((*cs4 >> 8)  & 0xff) * a4;
                    fcb = double( *cs1        & 0xff) * a1 +
                          double( *cs2        & 0xff) * a2 +
                          double( *cs3        & 0xff) * a3 +
                          double( *cs4        & 0xff) * a4;
                    fcr /= fca, fcg /= fca, fcb /= fca;
                    
                    // update rgba
                    dst[x1] = (b_sa ? color_t(fca * 255.f + .5f) : 255) << 24 |
                        color_t(fcr + .5f) << 16 | color_t(fcg + .5f) << 8 | color_t(fcb + .5f);
                } else
                    dst[x1] = trans;
            }
        }
        return res;
    }

    /**
     * @function mcl_transform_t::scale2x <src/transform.cpp>
     * @brief specialized image doubler
     * @param[in] surface
     * @return surface_t
     */
    surface_t mcl_transform_t::
    scale2x (surface_t const& surface, point2d_t* offset) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        char*           data     = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;

        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::scale2x");
        if (!dataplus -> m_width) return sf_nullptr;

        // create compatible surface
        point1d_t dstw = dataplus -> m_width << 1;
        point1d_t dsth = dataplus -> m_height << 1;

        surface_t res ({dstw, dsth}, data[0]);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char*           res_data     = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        // copy alpha info
        res_data[0] = data[0];
        res_dataplus -> m_colorkey = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;

        // AdvanceMAME Scale2X algorithm
        auto fp2x32 = []
        (color_t* dst, color_t const* src0, color_t const* src1,
            color_t const* src2, point1d_t count)
        {
            // first pixel
            if (src0[0] != src2[0] && src1[0] != src1[1]) {
                dst[0] = src1[0] == src0[0] ? src0[0] : src1[0];
                dst[1] = src1[1] == src0[0] ? src0[0] : src1[0];
            } else {
                dst[0] = src1[0];
                dst[1] = src1[0];
            }
            ++ src0, ++ src1, ++ src2;
            dst += 2, count -= 2;;

            // central pixels
            while (count) {
                if (src0[0] != src2[0] && src1[-1] != src1[1]) {
                    dst[0] = src1[-1] == src0[0] ? src0[0] : src1[0];
                    dst[1] = src1[1] == src0[0] ? src0[0] : src1[0];
                } else {
                    dst[0] = src1[0];
                    dst[1] = src1[0];
                }
                ++ src0, ++ src1, ++ src2;
                dst += 2, -- count;
            }

            // last pixel
            if (src0[0] != src2[0] && src1[-1] != src1[0]) {
                dst[0] = src1[-1] == src0[0] ? src0[0] : src1[0];
                dst[1] = src1[0] == src0[0] ? src0[0] : src1[0];
            } else {
                dst[0] = src1[0];
                dst[1] = src1[0];
            }
        };
        
        // scaling calc
        color_t* dst0 = res_dataplus -> m_pbuffer;
        color_t* dst1 = dst0 + dstw;
        color_t* src0 = dataplus -> m_pbuffer;
        color_t* src1 = src0 + dataplus -> m_width;
        color_t* src2 = src1 + dataplus -> m_width;
        color_t* srce = src0 + dataplus -> m_height * dataplus -> m_width;
        
        if (offset) {
            offset -> x = -(dataplus -> m_width >> 1);
            offset -> y = -(dataplus -> m_height >> 1);
        }
        
        // src with single pixel side
        if (dstw == 2) {
            do {
                dst0 = src0; ++ dst0;
                dst0 = src0; ++ dst0;
                dst0 = src0; ++ dst0;
                dst0 = src0; ++ dst0;
            } while (++ src0 != srce);
            return res;
        }
        if (dsth == 2) {
            do {
                dst0 = src0; ++ dst0;
                dst0 = src0; ++ dst0;
                dst1 = src0; ++ dst1;
                dst1 = src0; ++ dst1;
            } while (++ src0 != srce);
            return res;
        }

        // first line
        point1d_t dstw2x = dstw << 1;
        fp2x32 (dst0, src0, src0, src1, dstw);
        fp2x32 (dst1, src1, src0, src0, dstw);
        dst0 += dstw2x;
        dst1 += dstw2x;
        // central lines
        while (src2 != srce) {
            fp2x32 (dst0, src0, src1, src2, dstw);
            fp2x32 (dst1, src2, src1, src0, dstw);
            dst0 += dstw2x;
            dst1 += dstw2x;
            src0 += dataplus -> m_width;
            src1 += dataplus -> m_width;
            src2 += dataplus -> m_width;
        }
        // last line
        fp2x32 (dst0, src0, src1, src1, dstw);
        fp2x32 (dst1, src1, src1, src0, dstw);
        return res;
    }

    /**
     * @function mcl_transform_t::chop <src/transform.cpp>
     * @brief gets a copy of an image with an interior area removed
     * @param[in] surface
     * @param[in] rect
     * @return surface_t
     */
    surface_t mcl_transform_t::
    chop (surface_t const& surface, rect_t rect) noexcept{
        if (!rect.w || !rect.h) return sf_nullptr;
        point1d_t x = rect.x, y = rect.y, w = rect.w, h = rect.h;
        if (w < 0) x += w + 1, w = -w;
        if (h < 0) y += h + 1, h = -h;
        
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        char*           data     = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::chop");
        if (!dataplus -> m_width) return sf_nullptr;

        // map direction
        point1d_t sx1 = 0, ex1 = x, sx2 = x + w, ex2 = dataplus -> m_width;
        point1d_t sy1 = 0, ey1 = y, sy2 = y + h, ey2 = dataplus -> m_height;
        if (ex1 < sx1) ex1 = sx1; if (ex1 > ex2) ex1 = ex2;
        if (ey1 < sy1) ey1 = sy1; if (ey1 > ey2) ey1 = ey2;
        if (sx2 > ex2) sx2 = ex2; if (sx2 < sx1) sx2 = sx1;
        if (sy2 > ey2) sy2 = ey2; if (sy2 < sy1) sy2 = sy1;

        // create compatible surface
        surface_t res ({ex1 - sx1 + ex2 - sx2, ey1 - sy1 + ey2 - sy2}, data[0]);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char*           res_data     = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        // copy alpha info
        res_data[0] = data[0];
        res_dataplus -> m_colorkey = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;
        
        // map direction
        color_t *dst = res_dataplus -> m_pbuffer, *src = dataplus -> m_pbuffer;
        color_t *srcey1 = src + ey1 * dataplus -> m_width, *srcex1 = 0;
        color_t *srcsy2 = src + sy2 * dataplus -> m_width, *srcsx2 = 0;
        color_t *srcey2 = src + ey2 * dataplus -> m_width, *srcex2 = 0;

        // y in [0, ey1)
        while (src != srcey1) {
            srcex1 = src + ex1;
            srcsx2 = src + sx2;
            srcex2 = src + ex2;
            while (src != srcex1)
                *dst = *src, ++ dst, ++ src;
            src = srcsx2;
            while (src != srcex2)
                *dst = *src, ++ dst, ++ src;
        }
        // y in [sy2, ey2)
        src = srcsy2;
        while (src != srcey2) {
            srcex1 = src + ex1;
            srcsx2 = src + sx2;
            srcex2 = src + ex2;
            while (src != srcex1)
                *dst = *src, ++ dst, ++ src;
            src = srcsx2;
            while (src != srcex2)
                *dst = *src, ++ dst, ++ src;
        }
        return res;
    }

    /**
     * @function mcl_transform_t::clip <src/transform.cpp>
     * @brief gets a copy of part of an image
     * @param[in] surface
     * @param[in] rect
     * @return surface_t
     */
    surface_t mcl_transform_t::
    clip (surface_t const& surface, rect_t rect) noexcept{
        if (!rect.w || !rect.h) return sf_nullptr;
        if (rect.w < 0) rect.x += rect.w + 1, rect.w = -rect.w;
        if (rect.h < 0) rect.y += rect.h + 1, rect.h = -rect.h;

        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        char*           data     = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;

        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::chop");
        if (!dataplus -> m_width) return sf_nullptr;

        // create compatible surface
        surface_t res ({ rect.w, rect.h }, data[0]);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char*           res_data     = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        // copy alpha info
        res_data[0] = data[0];
        res_dataplus -> m_colorkey = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;
        bool    b_sa = res_data[0] & surface_t::SrcAlpha;
        bool    b_ck = res_data[0] & surface_t::SrcColorKey;
        color_t trans = b_sa ? 0 : (b_ck ?
            res_dataplus -> m_colorkey : res_dataplus -> m_pbuffer[0]);

        // restrict area within the surface
        point1d_t sx = rect.x, sy = rect.y, tw = 0, th = 0;
        point1d_t dx = 0, dy = 0, dw = rect.w, dh = rect.h;
        if (sx < 0) { dx -= sx; dw += sx; sx = 0; }
        if (sy < 0) { dy -= sy; dh += sy; sy = 0; }
        tw = dataplus -> m_width - sx,      dw = (dw > tw ? tw : dw);
        th = dataplus -> m_height - sy,     dh = (dh > th ? th : dh);
        tw = res_dataplus -> m_width - dx,  dw = (dw > tw ? tw : dw);
        th = res_dataplus -> m_height - dy, dh = (dh > th ? th : dh);
        
        // start bliting
        if (dx || dy || dw < rect.w || dh < rect.h) {
            color_t* dj = res_dataplus -> m_pbuffer;
            color_t* di = dj + static_cast<size_t>(dy) * res_dataplus -> m_width + dx;
            color_t *si = dataplus -> m_pbuffer + static_cast<size_t>(sy) * dataplus -> m_width + sx, *sj = 0;
            color_t *di0 = di + static_cast<size_t>(dh) * res_dataplus -> m_width, *dj0 = 0;
            if (dw > 0 && dh > 0) {
                for (; di != di0; di += res_dataplus -> m_width, si += dataplus -> m_width) {
                    for (; dj != di; ++ dj) *dj = trans;
                    for (sj = si, dj0 = di + dw; dj != dj0; ++ sj, ++ dj) *dj = *sj;
                }
            }
            di = res_dataplus -> m_pbuffer + res_dataplus -> m_width * res_dataplus -> m_height;
            for (; dj != di; ++ dj) *dj = trans;
            return res;
        }
        ::BitBlt (res_dataplus -> m_hdc, dx, dy, dw, dh, dataplus -> m_hdc, sx, sy, SRCCOPY);
        return res;
    }

    /**
     * @function mcl_transform_t::laplacian <src/transform.cpp>
     * @brief find edges in a surface using the laplacian algorithm
     * @param[in] surface
     * @return surface_t
     */
    surface_t mcl_transform_t::
    laplacian (surface_t const& surface) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::laplacian");
        char* data = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus -> m_width) return sf_nullptr;
        
        // create compatible surface
        surface_t res ({dataplus -> m_width, dataplus -> m_height}, data[0]);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        // copy alpha info
        color_t b_sa = color_t(data[0] & surface_t::SrcAlpha);
        color_t b_ck = color_t(data[0] & surface_t::SrcColorKey);
        color_t m_ck = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;
        
        // laplacian lines
        auto h1gray = [b_sa, b_ck, m_ck] // grayscale
        (color_t* dst, color_t* src, point1d_t count) {
            do {
                *dst = ((*src >> 16) & 0xff) * 299 +
                       ((*src >> 8)  & 0xff) * 587 +
                       ( *src        & 0xff) * 114;
                if (b_sa) *dst = *dst * (*src >> 24) / 255000;
                else if (b_ck && (*src & 0xffffff) == m_ck) *dst = 0;
                else      *dst /= 1000;
                ++ src; ++ dst;
            } while (-- count);
        };
        auto h1laplacian = [] // laplacian
        (color_t* dst, color_t* src0, color_t* src1, color_t* src2, point1d_t count) {
            -- count;
            while (-- count) {
                ++ src0, ++ src1, ++ src2, ++ dst;
                dst[0] = color_t(abs(long(src1[0] << 2) - long(src0[0] + src1[-1] + src1[1] + src2[0])));
            }
        };
        auto h1clear = [] // empty line
        (color_t* dst, point1d_t count) {
            do {
                *dst = 0xff000000;
                ++ dst;
            } while (-- count);
        };
        auto h1copy = [] // grayscale to rgb
        (color_t* dst, color_t* src, point1d_t count) {
            count -= 2;
            dst[0] = 0xff000000;
            do {
                ++ src, ++ dst;
                dst[0] = (src[0] << 16) | (src[0] << 8) | (src[0]) | 0xff000000;
            } while (-- count);
            dst[1] = 0xff000000;
        };
        
        // map direction
        color_t *src = dataplus -> m_pbuffer;
        color_t *srce = src + (dataplus -> m_height - 1) * dataplus -> m_width;
        color_t *dst0 = res_dataplus -> m_pbuffer;
        color_t *dst1 = dst0 + res_dataplus -> m_width;
        color_t *dst2 = dst0;

        // too small
        if (dataplus -> m_width <= 2 || dataplus -> m_height <= 2) {
            dst2 += res_dataplus -> m_height * res_dataplus -> m_width;
            while (dst0 != dst2)
                *dst0 = 0xff000000, ++ dst0;
            return res;
        }

        // preload first two rows
        h1gray (dst2, src, dataplus -> m_width); 
        src += dataplus -> m_width;
        dst2 += res_dataplus -> m_width;
        h1gray (dst2, src, dataplus -> m_width);

        // load and filter remaining rows
        do {
            src += dataplus -> m_width;
            dst2 += res_dataplus -> m_width;
            h1gray (dst2, src, dataplus -> m_width);
            h1laplacian (dst0, dst0, dst1, dst2, dataplus -> m_width);
            dst0 += res_dataplus -> m_width;
            dst1 += res_dataplus -> m_width;
        } while (src != srce);

        // grayscale to rgb
        src  = dst0;
        srce = res_dataplus -> m_pbuffer;
        h1clear (dst2, dataplus -> m_width);

        do {
            src -= dataplus -> m_width;
            h1copy (dst0, src, dataplus -> m_width);
            dst0 = src;
        } while (src != srce);

        h1clear (src, dataplus -> m_width);
        return res;
    }

    /**
     * @function mcl_transform_t::average_color <src/transform.cpp>
     * @brief finds the average color of a surface
     * @param[in] surface
     * @return color_t
     */
    color_t mcl_transform_t::
    average_color (surface_t const& surface) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!dataplus) return 0;
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::average_color");
        char* data = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus -> m_width) return 0;

        // color
        long long sa = 0, sr = 0, sg = 0, sb = 0, srca = 0;
        point1d_t sum = dataplus -> m_height * dataplus -> m_width;
        color_t* src = dataplus -> m_pbuffer + sum;

        // alpha info
        color_t b_sa = color_t(data[0] & surface_t::SrcAlpha);
        color_t b_ck = color_t(data[0] & surface_t::SrcColorKey);
        color_t m_ck = dataplus -> m_colorkey;

        if (b_sa) {
            while (src != dataplus -> m_pbuffer) {
                -- src;
                srca = static_cast<long long>(*src >> 24);
                sa += srca;
                sr += static_cast<long long>((*src >> 16) & 0xff) * srca / 255;
                sg += static_cast<long long>((*src >> 8)  & 0xff) * srca / 255;
                sb += static_cast<long long>( *src        & 0xff) * srca / 255;
            }
            sum = point1d_t(sa / 255);
        } else {
            while (src != dataplus -> m_pbuffer) {
                -- src;
                if (b_ck && (*src & 0xffffff) == m_ck) { -- sum; continue; }
                sr += static_cast<long long>((*src >> 16) & 0xff);
                sg += static_cast<long long>((*src >> 8)  & 0xff);
                sb += static_cast<long long>( *src        & 0xff);
            }
        }
        if (sum != 0) {
            sr /= sum, sg /= sum, sb /= sum;
            return color_t(sr << 16 | sg << 8 | sb | 0xff000000);
        }
        return 0;
    }

    /**
     * @function mcl_transform_t::grayscale <src/transform.cpp>
     * @brief grayscale a surface
     * @param[in] surface
     * @return surface_t
     */
    surface_t mcl_transform_t::
    grayscale (surface_t const& surface) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;
        
        // lock
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::grayscale");
        char* data = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus -> m_width) return sf_nullptr;
        
        // create compatible surface
        surface_t res ({dataplus -> m_width, dataplus -> m_height}, data[0]);
        char*           res_data     = mcl_get_surface_data (&res);
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        // copy alpha info
        res_data[0] = data[0];
        res_dataplus -> m_colorkey = dataplus -> m_colorkey;
        res_dataplus -> m_alpha = dataplus -> m_alpha;
        color_t b_ck = color_t(data[0] & surface_t::SrcColorKey);
        color_t m_ck = dataplus -> m_colorkey;

        // map direction
        color_t *src = dataplus -> m_pbuffer, *dst = res_dataplus -> m_pbuffer;
        color_t *se = src + dataplus -> m_height * dataplus -> m_width;

        while (src != se) {
            *dst = ( ((*src >> 16) & 0xff) * 299 +
                     ((*src >> 8)  & 0xff) * 587 +
                     ( *src        & 0xff) * 114 ) / 1000;
            if (b_ck) {
                if ((*src & 0xffffff) == m_ck) *dst = m_ck;
                else {
                    *dst |= (*dst << 16) | (*dst << 8);
                    if (*dst == m_ck) {
                        *dst &= 0xff;
                        if (*dst == 0xff) *dst = 0xfefefe;
                        else ++ *dst, *dst |= (*dst << 16) | (*dst << 8);
                    }
                }
            } else *dst |= (*dst << 16) | (*dst << 8) | (*src & 0xff000000);
            ++ src, ++ dst;
        }
        return res;
    }
    
    /**
     * @function mcl_transform_t::threshold <src/transform.cpp>
     * @brief finds which, and how many pixels in a surface are within
     *     a threshold of a 'search_color' or a 'search_surf'.
     * @return size_t: num_threshold_pixels
     */
    size_t mcl_transform_t::
    threshold (void* dest_surf, surface_t const& surf, color_t search_color,
      color_t nthreshold, color_t set_color, int set_behavior,
      void const* search_surf, bool inverse_set) noexcept{
        char*           data     = mcl_get_surface_data (&const_cast<surface_t&>(surf));
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surf));
        if (!dataplus) return 0;
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::threshold");
        if (!dataplus -> m_width) return 0;

        // alpha info of src
        color_t b_sa = color_t(data[0] & surface_t::SrcAlpha);
        color_t b_ck = color_t(data[0] & surface_t::SrcColorKey);
        color_t m_ck = dataplus -> m_colorkey;
        color_t m_alpha = dataplus -> m_alpha << 24;

        // search_surf
        surface_t* sf_srch = reinterpret_cast<surface_t*>(const_cast<void*>(search_surf));
        mcl_imagebuf_t* srch_dataplus = sf_srch ? mcl_get_surface_dataplus(sf_srch) : 0;

        // dest_surf
        surface_t* sf_dst = reinterpret_cast<surface_t*>(dest_surf);
        mcl_imagebuf_t* dst_dataplus = sf_dst ? mcl_get_surface_dataplus(sf_dst) : 0;
        bool b_dst_na = (!dst_dataplus || !dst_dataplus -> m_width);
        if (set_behavior != 0 && b_dst_na) return 0;
        // if (set_behavior == 0 && !b_dst_na) return 0;

        // calc rgba ranges & test if in range
        color_t ta =  nthreshold >> 24;
        color_t tr = (nthreshold >> 16) & 0xff;
        color_t tg = (nthreshold >> 8)  & 0xff;
        color_t tb =  nthreshold        & 0xff;
        color_t mina = 0, minr = 0, ming = 0, minb = 0;
        color_t maxa = 0, maxr = 0, maxg = 0, maxb = 0;
        auto fcalc = [&mina, &minr, &ming, &minb, &maxa, &maxr, &maxg, &maxb, ta, tr, tg, tb]
        (color_t color) {
            mina =  color >> 24;
            minr = (color >> 16) & 0xff;
            ming = (color >> 8)  & 0xff;
            minb =  color        & 0xff;
            maxa = mina + ta; if (maxa > 255) maxa = 255;
            maxr = minr + tr; if (maxr > 255) maxr = 255;
            maxg = ming + tg; if (maxg > 255) maxg = 255;
            maxb = minb + tb; if (maxb > 255) maxb = 255;
            mina = (mina >= ta ? mina - ta : 0);
            minr = (minr >= tr ? minr - tr : 0);
            ming = (ming >= tg ? ming - tg : 0);
            minb = (minb >= tb ? minb - tb : 0);
        };
        auto fck = [&mina, &minr, &ming, &minb, &maxa, &maxr, &maxg, &maxb, b_ck, m_ck, b_sa, m_alpha]
        (color_t color) -> bool {
            if (!b_sa) {
                color &= 0xffffff;
                if (!b_ck || color != m_ck) color |= m_alpha;
            } else if (m_alpha != 0xff000000) {
                color = (color & 0xffffff) | ((color >> 24) * (m_alpha >> 24) / 255) << 24;
            }
            color_t a =  color >> 24;
            color_t r = (color >> 16) & 0xff;
            color_t g = (color >> 8)  & 0xff;
            color_t b =  color        & 0xff;
            return (a >= mina && a <= maxa && r >= minr && r <= maxr
                 && g >= ming && g <= maxg && b >= minb && b <= maxb);
        };
        
        // switch function behavior
        size_t cnt = 0;
        std::function<void(color_t&, color_t)> fblit;
        std::function<void(color_t*, color_t*)> fcnt;
        if (set_behavior == 0) {
            // we do not change 'dest_surf', just count
            fcnt = [&fck, &cnt]
            (color_t*, color_t* src) {
                if (fck(*src)) ++ cnt;
            };
        } else if (set_behavior == 1) {
            // pixels in dest_surface will be changed to 'set_color'
            color_t bis = color_t(inverse_set);
            fcnt = [&fck, &cnt, bis, set_color]
            (color_t* dst, color_t* src) {
                bool fc = fck(*src), fis = bool(bis);
                if (dst && fc == fis) *dst = set_color;
                if (fc) ++ cnt;
            };
        } else if (set_behavior == 2) {
            // pixels set in 'dest_surf' will be from 'surf'
            // so we need to switch blit functions
            char* dst_data = mcl_get_surface_data(sf_dst);
            bool dst_sa = dst_data[0] & surface_t::SrcAlpha;
            color_t dst_ctrans = dst_sa ? 0 : (
            (dst_data[0] & surface_t::SrcColorKey) ? 0xffffff |
                dst_dataplus -> m_colorkey : (dst_dataplus -> m_pbuffer[0] & 0xffffff)
            );
            if (b_ck) {
                fblit = [m_ck, dst_ctrans, m_alpha](color_t& dst, color_t src)
                { dst = ((src &= 0xffffff) == m_ck ? dst_ctrans : src | m_alpha); };
            } else if (dst_sa && !b_sa) {
                if (m_alpha == 0xff000000)
                    fblit = [](color_t& dst, color_t src) { dst = src | 0xff000000; };
                else
                    fblit = [m_alpha](color_t& dst, color_t src) { dst = (src & 0xffffff) | m_alpha; };
            } else if (dst_sa && b_sa && m_alpha != 0xff000000) {
                fblit = [m_alpha](color_t& dst, color_t src) {
                    dst = ((src >> 24) * (m_alpha >> 24) / 255) << 24;
                    dst |= src & 0xffffff;
                };
            } else fblit = [](color_t& dst, color_t src) { dst = src; };

            // then blit
            long long bis = static_cast<long long>(inverse_set);
            fcnt = [&fck, &fblit, &cnt, bis]
            (color_t* dst, color_t* src) {
                bool fc = fck(*src), fis = bool(bis);
                if (dst && fc == fis) fblit(*dst, *src);
                if (fc) ++ cnt;
            };
        } else {
            // unknown behavior
            return 0;
        }
        
        // map direction
        color_t *src = dataplus -> m_pbuffer, *dst = 0, *ld = 0;
        point1d_t i = 0, j = 0;
        point1d_t sw = dataplus -> m_width, sh = dataplus -> m_height;
        point1d_t lw = 0, lh = 0;
        if (set_behavior) {
            dst = dst_dataplus -> m_pbuffer;
            if (sw > dst_dataplus -> m_width)
                lw = sw - dst_dataplus -> m_width,  sw = dst_dataplus -> m_width;
            if (sh > dst_dataplus -> m_height)
                lh = sh - dst_dataplus -> m_height, sh = dst_dataplus -> m_height;
        }

        // do not use search_surf
        if (!sf_srch) {
            // start bliting
            fcalc (search_color);
            for (i = sh; i; -- i, dst = ld) {
                if (set_behavior) ld = dst + dst_dataplus -> m_width;
                for (j = sw; j; -- j, ++ src) fcnt (dst ++, src);
                for (j = lw; j; -- j, ++ src) fcnt (0, src);
            }
            // out of dst. only count
            for (i = lh * dataplus -> m_width; i; -- i, ++ src)
                fcnt (0, src);
            return cnt;
        }

        // use search_surf
        // get rgba from search_surf
        if (!srch_dataplus || !srch_dataplus -> m_width) return 0;
        std::function<color_t(color_t)> fcalc_color;
        char* srch_data = mcl_get_surface_data(sf_srch);
        color_t srch_b_sa = color_t(srch_data[0] & surface_t::SrcAlpha);
        color_t srch_b_ck = color_t(srch_data[0] & surface_t::SrcColorKey);
        color_t srch_m_ck = srch_dataplus -> m_colorkey;
        color_t srch_m_alpha = srch_dataplus -> m_alpha << 24;
        if (srch_b_ck) {
            fcalc_color = [srch_m_ck, srch_m_alpha]
            (color_t color) -> color_t {
                return (color &= 0xffffff) == srch_m_ck ? color : color | srch_m_alpha;
            };
        } else if (!srch_b_sa) {
            fcalc_color = [srch_m_alpha]
            (color_t color) -> color_t {
                return (color & 0xffffff) | srch_m_alpha;
            };
        } else if (srch_m_alpha != 0xff000000) {
            fcalc_color = [srch_m_alpha]
            (color_t color) -> color_t {
                return (color & 0xffffff) | ((color >> 24) * (srch_m_alpha >> 24) / 255) << 24;
            };
        } else {
            fcalc_color = []
            (color_t color) -> color_t {
                return color;
            };
        }

        color_t *ls = 0, *sch = srch_dataplus -> m_pbuffer, *lsch = 0;
        if (sh > srch_dataplus -> m_height) sh = srch_dataplus -> m_height;
        // start bliting
        for (i = sh; i; -- i, dst = ld, src = ls, sch = lsch) {
            if (set_behavior) ld = dst + dst_dataplus -> m_width;
            ls = src + dataplus -> m_width;
            lsch = sch + srch_dataplus -> m_width;
            for (j = sw; j && sch != lsch; -- j, ++ src, ++ sch)
                fcalc (fcalc_color (*sch)), fcnt (dst ++, src);
            for (j = lw; j && sch != lsch; -- j, ++ src, ++ sch)
                fcalc (fcalc_color (*sch)), fcnt (0, src);
        }
        // out of dst. only count
        if (lh && srch_dataplus -> m_height > dst_dataplus -> m_height) {
            if (dataplus -> m_height > srch_dataplus -> m_height)
                lh = srch_dataplus -> m_height - dst_dataplus -> m_height;
            for (i = lh; i; -- i, src = ls, sch = lsch) {
                ls = src + dataplus -> m_width;
                lsch = sch + srch_dataplus -> m_width;
                for (j = sw + lw; j && sch != lsch; -- j, ++ src, ++ sch)
                    fcalc (fcalc_color (*sch)), fcnt (0, src);
            }
        }
        return cnt;
    }

}
