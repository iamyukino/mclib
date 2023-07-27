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

        color_t temp = 0;
        for (; src != ey; src += sdy, dst += ddy)
            for (ex += dataplus -> m_width; src != ex; src += sdx, dst += ddx)
                temp = *dst, *dst = *src, *src = temp;

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
        if (!(size.x > 0 && size.y > 0 && dataplus)) return sf_nullptr;

        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::scale");
        if (!dataplus -> m_width) return sf_nullptr;

        surface_t res (size, bool(mcl_get_surface_data (&const_cast<surface_t&>(surface))[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char* res_data = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;
        
        color_t *src = dataplus -> m_pbuffer, *dst = res_dataplus -> m_pbuffer;
        double kx = double(size.x) / double(dataplus -> m_width);
        double ky = double(size.y) / double(dataplus -> m_height);

        if (smooth_ipt >= 2 && (size.x < 3 || size.y < 3))
            smooth_ipt = 1;
        if (offset) {
            offset -> x = (dataplus -> m_width - size.x)  >> 1;
            offset -> y = (dataplus -> m_height - size.y) >> 1;
        }

        if (smooth_ipt <= 0) {
        // nearest_neighbor
            point1d_t i = 0, j = 0, x = 0, y = 0;
            for (; i != size.x; ++ i) {
                x = point1d_t(double(i) / kx + .5f);
                for (j = 0; j != size.y; ++ j) {
                    y = point1d_t(double(j) / ky + .5f);
                    dst[j * size.x + i] = src[y * dataplus -> m_width + x];
                }
            }
        } else if (smooth_ipt == 1) {
        // bilinear_interpolation
            point1d_t i = 0, j = 0;
            point1d_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
            double xm = 0.f, ym = 0.f;
            double dx = 0.f, dy = 0.f, fdx = 0.f, fdy = 0.f;

            color_t *cs1 = 0, *cs2 = 0, *cs3 = 0, *cs4 = 0;
            color_t ca = 0, cr = 0, cg = 0, cb = 0;

            for (; i != size.x; ++i) {
                xm = double(i) / kx;
                x1 = point1d_t(xm), x2 = x1 + 1;
                dx = xm - double(x1), fdx = 1.f - dx;
                
                if (x2 == dataplus -> m_width)
                    x2 = dataplus -> m_width - 1;

                for (j = 0; j != size.y; ++ j) {
                    ym = double(j) / ky;
                    y1 = point1d_t(ym), y2 = y1 + 1;
                    dy = ym - double(y1), fdy = 1.f - dy;
                
                    if (y2 == dataplus -> m_height)
                        y2 = dataplus -> m_height - 1;

                    cs1 = src + (y1 * dataplus -> m_width + x1);
                    cs2 = src + (y1 * dataplus -> m_width + x2);
                    cs3 = src + (y2 * dataplus -> m_width + x1);
                    cs4 = src + (y2 * dataplus -> m_width + x2);

                    ca = res_data[0] ? color_t(
                        double( *cs1 >> 24        ) * fdx * fdy +
                        double( *cs2 >> 24        ) * dx  * fdy +
                        double( *cs3 >> 24        ) * fdx * dy  +
                        double( *cs4 >> 24        ) * dx  * dy) : 0xff;
                    cr = color_t(
                        double((*cs1 >> 16) & 0xff) * fdx * fdy +
                        double((*cs2 >> 16) & 0xff) * dx  * fdy +
                        double((*cs3 >> 16) & 0xff) * fdx * dy  +
                        double((*cs4 >> 16) & 0xff) * dx  * dy);
                    cg = color_t(
                        double((*cs1 >> 8)  & 0xff) * fdx * fdy +
                        double((*cs2 >> 8)  & 0xff) * dx  * fdy +
                        double((*cs3 >> 8)  & 0xff) * fdx * dy  +
                        double((*cs4 >> 8)  & 0xff) * dx  * dy);
                    cb = color_t(
                        double( *cs1        & 0xff) * fdx * fdy +
                        double( *cs2        & 0xff) * dx  * fdy +
                        double( *cs3        & 0xff) * fdx * dy  +
                        double( *cs4        & 0xff) * dx  * dy);
                    dst[j * size.x + i] = (ca << 24) | (cr << 16) | (cg << 8) | cb;
                }
            }
        } else {
        // bicubic_interpolation
            auto fpBiCubicPoly = [](double x) {
                double const abs_x = x > 0.f ? x : -x;
                double constexpr a = -.5f;
                return (abs_x <= 1.f) ? 
                    (a + 2.f) * (abs_x * abs_x * abs_x) -
                    (a + 3.f) * (abs_x * abs_x) + 1: (
                       (abs_x <  2.f) ?
                    (a) * (abs_x * abs_x * abs_x) - (5.f * a) * (abs_x * abs_x) +
                    (8.f * a) * abs_x - 4.f * a : 0.f);
            };

            color_t *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;
            color_t pa = 0, pr = 0, pg = 0, pb = 0;
            double fpa = 0.f, fpr = 0.f, fpg = 0.f, fpb = 0.f;

            point1d_t i = 0, j = 0;
            point1d_t x0 = 0, x1 = 0, x2 = 0, x3 = 0;
            point1d_t y0 = 0, y1 = 0, y2 = 0, y3 = 0;

            double x = 0, y = 0, dx = 0.f, dy = 0.f, fdx = 0.f, fdy = 0.f;
            double dist_x0 = 0.f, dist_x1 = 0.f, dist_x2 = 0.f, dist_x3 = 0.f;
            double dist_y0 = 0.f, dist_y1 = 0.f, dist_y2 = 0.f, dist_y3 = 0.f;
            double dist_x0y0 = 0.f, dist_x0y1 = 0.f, dist_x0y2 = 0.f, dist_x0y3 = 0.f;
            double dist_x1y0 = 0.f, dist_x1y1 = 0.f, dist_x1y2 = 0.f, dist_x1y3 = 0.f;
            double dist_x2y0 = 0.f, dist_x2y1 = 0.f, dist_x2y2 = 0.f, dist_x2y3 = 0.f;
            double dist_x3y0 = 0.f, dist_x3y1 = 0.f, dist_x3y2 = 0.f, dist_x3y3 = 0.f;
            
            for (; i != size.y; ++ i, dst += size.x) {
                y = double(i) / double(ky);
                
                y0 = point1d_t(y) - 1;
                y1 = point1d_t(y);
                y2 = point1d_t(y) + 1;
                y3 = point1d_t(y) + 2;

                dist_y0 = fpBiCubicPoly (y - double(y0));
                dist_y1 = fpBiCubicPoly (y - double(y1));
                dist_y2 = fpBiCubicPoly (y - double(y2));
                dist_y3 = fpBiCubicPoly (y - double(y3));

                for (j = 0; j != size.x; ++ j) {
                    x = double(j) / double(kx);

                    x0 = point1d_t(x) - 1;
                    x1 = point1d_t(x);
                    x2 = point1d_t(x) + 1;
                    x3 = point1d_t(x) + 2;
                    
                    bool b_border = (x0 >= 0 && x3 < dataplus -> m_width
                        && y0 >= 0 && y3 < dataplus -> m_height);

                    if (b_border) {
                        q0 = src + y0 * dataplus -> m_width;
                        q1 = src + y1 * dataplus -> m_width;
                        q2 = src + y2 * dataplus -> m_width;
                        q3 = src + y3 * dataplus -> m_width;
                        
                        dist_x0 = fpBiCubicPoly (x - double(x0));
                        dist_x1 = fpBiCubicPoly (x - double(x1));
                        dist_x2 = fpBiCubicPoly (x - double(x2));
                        dist_x3 = fpBiCubicPoly (x - double(x3));
                        
                        dist_x0y0 = dist_x0 * dist_y0;
                        dist_x0y1 = dist_x0 * dist_y1;
                        dist_x0y2 = dist_x0 * dist_y2;
                        dist_x0y3 = dist_x0 * dist_y3;
                        dist_x1y0 = dist_x1 * dist_y0;
                        dist_x1y1 = dist_x1 * dist_y1;
                        dist_x1y2 = dist_x1 * dist_y2;
                        dist_x1y3 = dist_x1 * dist_y3;
                        dist_x2y0 = dist_x2 * dist_y0;
                        dist_x2y1 = dist_x2 * dist_y1;
                        dist_x2y2 = dist_x2 * dist_y2;
                        dist_x2y3 = dist_x2 * dist_y3;
                        dist_x3y0 = dist_x3 * dist_y0;
                        dist_x3y1 = dist_x3 * dist_y1;
                        dist_x3y2 = dist_x3 * dist_y2;
                        dist_x3y3 = dist_x3 * dist_y3;

                        fpa = res_data[0] ? 
                            double(q0[x0] >> 24) * dist_x0y0 +
                            double(q1[x0] >> 24) * dist_x0y1 +
                            double(q2[x0] >> 24) * dist_x0y2 +
                            double(q3[x0] >> 24) * dist_x0y3 +
                            double(q0[x1] >> 24) * dist_x1y0 +
                            double(q1[x1] >> 24) * dist_x1y1 +
                            double(q2[x1] >> 24) * dist_x1y2 +
                            double(q3[x1] >> 24) * dist_x1y3 +
                            double(q0[x2] >> 24) * dist_x2y0 +
                            double(q1[x2] >> 24) * dist_x2y1 +
                            double(q2[x2] >> 24) * dist_x2y2 +
                            double(q3[x2] >> 24) * dist_x2y3 +
                            double(q0[x3] >> 24) * dist_x3y0 +
                            double(q1[x3] >> 24) * dist_x3y1 +
                            double(q2[x3] >> 24) * dist_x3y2 +
                            double(q3[x3] >> 24) * dist_x3y3 : 255.f;

                        fpr = 
                            double((q0[x0] >> 16) & 0xff) * dist_x0y0 +
                            double((q1[x0] >> 16) & 0xff) * dist_x0y1 +
                            double((q2[x0] >> 16) & 0xff) * dist_x0y2 +
                            double((q3[x0] >> 16) & 0xff) * dist_x0y3 +
                            double((q0[x1] >> 16) & 0xff) * dist_x1y0 +
                            double((q1[x1] >> 16) & 0xff) * dist_x1y1 +
                            double((q2[x1] >> 16) & 0xff) * dist_x1y2 +
                            double((q3[x1] >> 16) & 0xff) * dist_x1y3 +
                            double((q0[x2] >> 16) & 0xff) * dist_x2y0 +
                            double((q1[x2] >> 16) & 0xff) * dist_x2y1 +
                            double((q2[x2] >> 16) & 0xff) * dist_x2y2 +
                            double((q3[x2] >> 16) & 0xff) * dist_x2y3 +
                            double((q0[x3] >> 16) & 0xff) * dist_x3y0 +
                            double((q1[x3] >> 16) & 0xff) * dist_x3y1 +
                            double((q2[x3] >> 16) & 0xff) * dist_x3y2 +
                            double((q3[x3] >> 16) & 0xff) * dist_x3y3;

                        fpg = 
                            double((q0[x0] >> 8) & 0xff) * dist_x0y0 +
                            double((q1[x0] >> 8) & 0xff) * dist_x0y1 +
                            double((q2[x0] >> 8) & 0xff) * dist_x0y2 +
                            double((q3[x0] >> 8) & 0xff) * dist_x0y3 +
                            double((q0[x1] >> 8) & 0xff) * dist_x1y0 +
                            double((q1[x1] >> 8) & 0xff) * dist_x1y1 +
                            double((q2[x1] >> 8) & 0xff) * dist_x1y2 +
                            double((q3[x1] >> 8) & 0xff) * dist_x1y3 +
                            double((q0[x2] >> 8) & 0xff) * dist_x2y0 +
                            double((q1[x2] >> 8) & 0xff) * dist_x2y1 +
                            double((q2[x2] >> 8) & 0xff) * dist_x2y2 +
                            double((q3[x2] >> 8) & 0xff) * dist_x2y3 +
                            double((q0[x3] >> 8) & 0xff) * dist_x3y0 +
                            double((q1[x3] >> 8) & 0xff) * dist_x3y1 +
                            double((q2[x3] >> 8) & 0xff) * dist_x3y2 +
                            double((q3[x3] >> 8) & 0xff) * dist_x3y3;
                
                        fpb = 
                            double(q0[x0] & 0xff) * dist_x0y0 +
                            double(q1[x0] & 0xff) * dist_x0y1 +
                            double(q2[x0] & 0xff) * dist_x0y2 +
                            double(q3[x0] & 0xff) * dist_x0y3 +
                            double(q0[x1] & 0xff) * dist_x1y0 +
                            double(q1[x1] & 0xff) * dist_x1y1 +
                            double(q2[x1] & 0xff) * dist_x1y2 +
                            double(q3[x1] & 0xff) * dist_x1y3 +
                            double(q0[x2] & 0xff) * dist_x2y0 +
                            double(q1[x2] & 0xff) * dist_x2y1 +
                            double(q2[x2] & 0xff) * dist_x2y2 +
                            double(q3[x2] & 0xff) * dist_x2y3 +
                            double(q0[x3] & 0xff) * dist_x3y0 +
                            double(q1[x3] & 0xff) * dist_x3y1 +
                            double(q2[x3] & 0xff) * dist_x3y2 +
                            double(q3[x3] & 0xff) * dist_x3y3;
                        
                        pa = fpa > 0.f ? color_t(fpa) : 0;
                        pr = fpr > 0.f ? color_t(fpr) : 0;
                        pg = fpg > 0.f ? color_t(fpg) : 0;
                        pb = fpb > 0.f ? color_t(fpb) : 0;
                        
                        if (pa > 0xff) pa = 0xff;
                        if (pr > 0xff) pr = 0xff;
                        if (pg > 0xff) pg = 0xff;
                        if (pb > 0xff) pb = 0xff;

                        dst[j] = (pa << 24) | (pr << 16) | (pg << 8) | pb;
                    } else {
                        if (x2 == dataplus -> m_width)  x2 = dataplus -> m_width  - 1;
                        if (y2 == dataplus -> m_height) y2 = dataplus -> m_height - 1;
                        q0 = src + (y1 * dataplus -> m_width + x1);
                        q1 = src + (y1 * dataplus -> m_width + x2);
                        q2 = src + (y2 * dataplus -> m_width + x1);
                        q3 = src + (y2 * dataplus -> m_width + x2);

                        dx = x - double(x1), dy = y - double(y1);
                        fdx = 1.f - dx,     fdy = 1.f - dy;

                        pa = res_data[0] ? color_t(
                            double( *q0 >> 24        ) * fdx * fdy +
                            double( *q1 >> 24        ) * dx  * fdy +
                            double( *q2 >> 24        ) * fdx * dy  +
                            double( *q3 >> 24        ) * dx  * dy) : 0xff;
                        pr = color_t(
                            double((*q0 >> 16) & 0xff) * fdx * fdy +
                            double((*q1 >> 16) & 0xff) * dx  * fdy +
                            double((*q2 >> 16) & 0xff) * fdx * dy  +
                            double((*q3 >> 16) & 0xff) * dx  * dy);
                        pg = color_t(
                            double((*q0 >> 8)  & 0xff) * fdx * fdy +
                            double((*q1 >> 8)  & 0xff) * dx  * fdy +
                            double((*q2 >> 8)  & 0xff) * fdx * dy  +
                            double((*q3 >> 8)  & 0xff) * dx  * dy);
                        pb = color_t(
                            double( *q0        & 0xff) * fdx * fdy +
                            double( *q1        & 0xff) * dx  * fdy +
                            double( *q2        & 0xff) * fdx * dy  +
                            double( *q3        & 0xff) * dx  * dy);
                        dst[j] = (pa << 24) | (pr << 16) | (pg << 8) | pb;
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
        double constexpr pi = 3.1415926535897932384626433832795f;
        double rad = double(angle) * pi / 180.f;

        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;

        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::rotozoom");
        if (!dataplus -> m_width) return sf_nullptr;

        double fdw = (abs(double(dataplus -> m_width) * cos(rad)) + abs(double(dataplus -> m_height) * sin(rad))) * fscale;
        double fdh = (abs(double(dataplus -> m_width) * sin(rad)) + abs(double(dataplus -> m_height) * cos(rad))) * fscale;
        point1d_t dstw = point1d_t(fdw + .5f);
        point1d_t dsth = point1d_t(fdh + .5f);
        
        surface_t res ({dstw, dsth}, bool(mcl_get_surface_data(&const_cast<surface_t&>(surface))[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char* res_data = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;
        
        color_t *src = dataplus -> m_pbuffer, *dst = res_dataplus -> m_pbuffer;
        color_t *cs1 = 0, *cs2 = 0, *cs3 = 0, *cs4 = 0;
        color_t trans = res_data[0] ? 0 : dataplus -> m_pbuffer[0]; // 0xff00ff00
        color_t ca = 0, cr = 0, cg = 0, cb = 0;

        double fx0 = 0.f, fy0 = 0.f, fx1 = 0.f, fy1 = 0.f;
        double R = 0.f, sita = 0.f;
        double p = 0.f, q = 0.f, fp = 0.f, fq = 0.f;
        point1d_t x1 = 0, y1 = 0;
        point1d_t ix0 = 0, iy0 = 0, ix1 = 0, iy1 = 0;
        
        double vx0 = double(dataplus -> m_width - 1) / 2.f;
        double vy0 = double(dataplus -> m_height - 1) / 2.f;
        double vx1 = double(dstw - 1) / 2.f;
        double vy1 = double(dsth - 1) / 2.f;
        
        if (offset) {
            offset -> x = point1d_t(vx0 - vx1 + .5f);
            offset -> y = point1d_t(vy0 - vy1 + .5f);
        }

        for (; y1 != dsth; ++ y1, dst += dstw) {
            fy1 = double(y1) - vy1;
            fy1 /= fscale;
            
            for (x1 = 0; x1 != dstw; ++ x1)
            {
                fx1 = double(x1) - vx1;
                fx1 /= fscale;

                R = sqrt(fx1 * fx1 + fy1 * fy1);
                sita = atan2(fy1, fx1) + rad;
                fx0 = R * cos(sita); 
                fy0 = R * sin(sita); 
                fx0 += vx0;
                fy0 += vy0;
                
                ix0 = point1d_t(fx0);
                iy0 = point1d_t(fy0);
                ix1 = ix0 + 1;
                iy1 = iy0 + 1;
                
                p = fx0 - double(ix0);
                q = fy0 - double(iy0);
                fp = 1 - p; fq = 1 - q;
                fx0 += .5f; fy0 += .5f;

                bool b_border = (fx0 >= 0.f && fx0 <= double(dataplus -> m_width)
                    && fy0 >= 0.f && fy0 <= double(dataplus -> m_height));
                if (b_border)
                {
                    if (fx0 < .5f)                                 ix0 = ix1;
                    if (fx0 >= double(dataplus -> m_width) - .5f)  ix1 = ix0;
                    if (fy0 < .5f)                                 iy0 = iy1;
                    if (fy0 >= double(dataplus -> m_height) - .5f) iy1 = iy0;

                    cs1 = src + (iy0 * dataplus -> m_width + ix0);
                    cs2 = src + (iy0 * dataplus -> m_width + ix1);
                    cs3 = src + (iy1 * dataplus -> m_width + ix0);
                    cs4 = src + (iy1 * dataplus -> m_width + ix1);
                    
                    ca = res_data[0] ? color_t(
                        double( *cs1 >> 24        ) * fp * fq +
                        double( *cs2 >> 24        ) * p  * fq +
                        double( *cs3 >> 24        ) * fp * q  +
                        double( *cs4 >> 24        ) * p  * q) : 0xff;
                    cr = color_t(
                        double((*cs1 >> 16) & 0xff) * fp * fq +
                        double((*cs2 >> 16) & 0xff) * p  * fq +
                        double((*cs3 >> 16) & 0xff) * fp * q  +
                        double((*cs4 >> 16) & 0xff) * p  * q);
                    cg = color_t(
                        double((*cs1 >> 8)  & 0xff) * fp * fq +
                        double((*cs2 >> 8)  & 0xff) * p  * fq +
                        double((*cs3 >> 8)  & 0xff) * fp * q  +
                        double((*cs4 >> 8)  & 0xff) * p  * q);
                    cb = color_t(
                        double( *cs1        & 0xff) * fp * fq +
                        double( *cs2        & 0xff) * p  * fq +
                        double( *cs3        & 0xff) * fp * q  +
                        double( *cs4        & 0xff) * p  * q);
                    dst[x1] = (ca << 24) | (cr << 16) | (cg << 8) | cb;
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
        if (!dataplus) return sf_nullptr;

        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::scale2x");
        if (!dataplus -> m_width) return sf_nullptr;

        point1d_t dstw = dataplus -> m_width << 1;
        point1d_t dsth = dataplus -> m_height << 1;

        surface_t res ({dstw, dsth}, bool(mcl_get_surface_data(&const_cast<surface_t&>(surface))[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

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

        point1d_t dstw2x = dstw << 1;
        fp2x32 (dst0, src0, src0, src1, dstw);
        fp2x32 (dst1, src1, src0, src0, dstw);
        dst0 += dstw2x;
        dst1 += dstw2x;
        while (src2 != srce) {
            fp2x32 (dst0, src0, src1, src2, dstw);
            fp2x32 (dst1, src2, src1, src0, dstw);
            dst0 += dstw2x;
            dst1 += dstw2x;
            src0 += dataplus -> m_width;
            src1 += dataplus -> m_width;
            src2 += dataplus -> m_width;
        }
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
        if (!rect.w || !rect.h)
            return sf_nullptr;

        point1d_t x = rect.x, y = rect.y, w = rect.w, h = rect.h;
        if (w < 0) x += w + 1, w = -w;
        if (h < 0) y += h + 1, h = -h;
        
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!dataplus) return sf_nullptr;
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::chop");
        if (!dataplus -> m_width) return sf_nullptr;

        point1d_t sx1 = 0, ex1 = x, sx2 = x + w, ex2 = dataplus -> m_width;
        point1d_t sy1 = 0, ey1 = y, sy2 = y + h, ey2 = dataplus -> m_height;
        if (ex1 < sx1) ex1 = sx1; if (ex1 > ex2) ex1 = ex2;
        if (ey1 < sy1) ey1 = sy1; if (ey1 > ey2) ey1 = ey2;
        if (sx2 > ex2) sx2 = ex2; if (sx2 < sx1) sx2 = sx1;
        if (sy2 > ey2) sy2 = ey2; if (sy2 < sy1) sy2 = sy1;

        surface_t res ({ex1 - sx1 + ex2 - sx2, ey1 - sy1 + ey2 - sy2},
            bool(mcl_get_surface_data(&const_cast<surface_t&>(surface))[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;
        
        color_t *dst = res_dataplus -> m_pbuffer, *src = dataplus -> m_pbuffer;
        color_t *srcey1 = src + ey1 * dataplus -> m_width, *srcex1 = 0;
        color_t *srcsy2 = src + sy2 * dataplus -> m_width, *srcsx2 = 0;
        color_t *srcey2 = src + ey2 * dataplus -> m_width, *srcex2 = 0;

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
        if (!rect.w || !rect.h)
            return sf_nullptr;

        if (rect.w < 0) rect.x += rect.w + 1, rect.w = -rect.w;
        if (rect.h < 0) rect.y += rect.h + 1, rect.h = -rect.h;

        surface_t res({ rect.w, rect.h },
            bool(mcl_get_surface_data(&const_cast<surface_t&>(surface))[0]));

        res.bilt (surface, 0, rect);
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
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::laplacian");
        char* data = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus -> m_width) return sf_nullptr;
        
        surface_t res ({dataplus -> m_width, dataplus -> m_height}, bool(data[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        auto h1gray = []
        (color_t* dst, color_t* src, point1d_t count) {
            do {
                *dst = (((*src >> 16) & 0xff) * 299 +
                        ((*src >> 8)  & 0xff) * 587 +
                        ( *src        & 0xff) * 114) * (*src >> 24) / 255000;
                ++ src; ++ dst;
            } while (-- count);
        };
        auto h1laplacian = []
        (color_t* dst, color_t* src0, color_t* src1, color_t* src2, point1d_t count) {
            -- count;
            while (-- count) {
                ++ src0, ++ src1, ++ src2, ++ dst;
                dst[0] = color_t(abs(long(src1[0] << 2) - long(src0[0] + src1[-1] + src1[1] + src2[0])));
            }
        };
        auto h1clear = []
        (color_t* dst, point1d_t count) {
            do {
                *dst = 0xff000000;
                ++ dst;
            } while (-- count);
        };
        auto h1copy = []
        (color_t* dst, color_t* src, point1d_t count) {
            count -= 2;
            dst[0] = 0xff000000;
            do {
                ++ src, ++ dst;
                dst[0] = (src[0] << 16) | (src[0] << 8) | (src[0]) | 0xff000000;
            } while (-- count);
            dst[1] = 0xff000000;
        };

        color_t *src = dataplus -> m_pbuffer;
        color_t *srce = src + (dataplus -> m_height - 1) * dataplus -> m_width;
        color_t *dst0 = res_dataplus -> m_pbuffer;
        color_t *dst1 = dst0 + res_dataplus -> m_width;
        color_t *dst2 = dst0;

        if (dataplus -> m_width <= 2 || dataplus -> m_height <= 2) {
            dst2 += res_dataplus -> m_height * res_dataplus -> m_width;
            while (dst0 != dst2)
                *dst0 = 0xff000000, ++ dst0;
            return res;
        }

        h1gray (dst2, src, dataplus -> m_width); 
        src += dataplus -> m_width;
        dst2 += res_dataplus -> m_width;
        h1gray (dst2, src, dataplus -> m_width);

        do {
            src += dataplus -> m_width;
            dst2 += res_dataplus -> m_width;
            h1gray (dst2, src, dataplus -> m_width);
            h1laplacian (dst0, dst0, dst1, dst2, dataplus -> m_width);
            dst0 += res_dataplus -> m_width;
            dst1 += res_dataplus -> m_width;
        } while (src != srce);

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
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::average_color");
        char* data = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus -> m_width) return 0;

        long long sa = 0, sr = 0, sg = 0, sb = 0, srca = 0;
        point1d_t sum = dataplus -> m_height * dataplus -> m_width;
        color_t* src = dataplus -> m_pbuffer + sum;

        if (data[0]) {
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
                sr += static_cast<long long>((*src >> 16) & 0xff);
                sg += static_cast<long long>((*src >> 8)  & 0xff);
                sb += static_cast<long long>( *src        & 0xff);
            }
        }
        if (sum != 0)
            sr /= sum, sg /= sum, sb /= sum;
        else sr = sg = sb = 0;
        return color_t((sr << 16) | (sg << 8) | (sb) | 0xff000000);
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
        
        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::grayscale");
        char* data = mcl_get_surface_data (&const_cast<surface_t&>(surface));
        if (!dataplus -> m_width) return sf_nullptr;
        
        surface_t res ({dataplus -> m_width, dataplus -> m_height}, bool(data[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;

        color_t *src = dataplus -> m_pbuffer, *dst = res_dataplus -> m_pbuffer;
        color_t *se = src + dataplus -> m_height * dataplus -> m_width;

        while (src != se) {
            *dst = ( ((*src >> 16) & 0xff) * 299 +
                     ((*src >> 8)  & 0xff) * 587 +
                     ( *src        & 0xff) * 114 ) / 1000;
            *dst |= (*dst << 16) | (*dst << 8) | (*src & 0xff000000);
            ++ src, ++ dst;
        }
        return res;
    }

}
