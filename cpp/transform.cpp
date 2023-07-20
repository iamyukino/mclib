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
    scale (surface_t const& surface, point2d_t size, short smooth_ipt) noexcept{
        mcl_imagebuf_t* dataplus = mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!(size.x > 0 && size.y > 0 && dataplus)) return sf_nullptr;

        mcl_simpletls_ns::mcl_spinlock_t lk(dataplus -> m_nrtlock, L"mcl_transform_t::scale");
        if (!dataplus -> m_width) return sf_nullptr;

        surface_t res (size, bool(mcl_get_surface_data (&const_cast<surface_t&>(surface))[0]));
        mcl_imagebuf_t* res_dataplus = mcl_get_surface_dataplus (&res);
        char* res_data = mcl_get_surface_data (&res);
        if (!(res_dataplus && res_dataplus -> m_width)) return sf_nullptr;
        
        color_t *src = dataplus -> m_pbuffer, *dst = res_dataplus -> m_pbuffer;
        double kx = static_cast<double>(size.x) / static_cast<double>(dataplus -> m_width);
        double ky = static_cast<double>(size.y) / static_cast<double>(dataplus -> m_height);

        if (smooth_ipt >= 2 && (size.x < 3 || size.y < 3))
            smooth_ipt = 1;

        if (smooth_ipt <= 0) {
        // nearest_neighbor
            point1d_t i = 0, j = 0, x = 0, y = 0;
            for (; i != size.x; ++ i) {
                x = static_cast<int>(static_cast<double>(i) / kx + .5f);
                for (j = 0; j != size.y; ++ j) {
                    y = static_cast<int>(static_cast<double>(j) / ky + .5f);
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
                double const abs_x = (x >= 0.f ? x : -x);
                double constexpr a = -.5f;
                return (abs_x <= 1.f) ? 
                    (a + 2.f) * (abs_x * abs_x * abs_x) -
                    (a + 3.f) * (abs_x * abs_x) + 1: (
                       (abs_x <  2.f) ?
                    (a) * (abs_x * abs_x * abs_x) - (5.f * a) * (abs_x * abs_x) +
                    (8.f * a) * abs_x - 4.f * a : 0.f);
            };

            color_t *p = 0, *q0 = 0, *q1 = 0, *q2 = 0, *q3 = 0;
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
            
            for (; i != size.y; ++ i) {
                p = dst + i * size.x; 
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

                        p[j] = (pa << 24) | (pr << 16) | (pg << 8) | pb;
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
                        p[j] = (pa << 24) | (pr << 16) | (pg << 8) | pb;
                    }
                }
            }
        }
        return res;
    }
    surface_t mcl_transform_t::
    scale_by (surface_t const& surface, float factor_x, float factor_y, short smooth_ipt) noexcept{
        factor_x *= static_cast<float>(surface.get_width ());
        factor_y *= static_cast<float>(surface.get_height ());
        return scale (surface, { point1d_t(factor_x), point1d_t(factor_y) }, smooth_ipt);
    }
}
