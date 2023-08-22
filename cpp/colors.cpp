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
    
    @file cpp/colors.cpp
    This is a C++11 implementation file for synthesizing colors.
*/


#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/colors.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

#define MCL_FRGB(tor, tog, tob, fc) \
    ((static_cast<color_t>(tor) << 16) | (static_cast<color_t>(tog) << 8) | \
      static_cast<color_t>(tob) | ((fc) & 0xff000000))

   /**
    * @function cvtrgb2hsl <src/colors.h>
    * @brief Convert the RGB color to the HSL color
    * @param {color_t} rgbcolor: {a,r,g,b} = [0, 255]
    * @return {color_t} hslcolor: {a} = [0, 255], {h,s,l} = [0, 240]
    */
    color_t
    cvtrgb2hsl (color_t rgbcolor) {
    
        float toh, tos, tol, cMax, cMin, del_Max,
            frr = static_cast<float>(getr4rgb (rgbcolor)) / 255.f,
            frg = static_cast<float>(getg4rgb (rgbcolor)) / 255.f,
            frb = static_cast<float>(getb4rgb (rgbcolor)) / 255.f;
        cMax = frr > frg ? frr : frg;  cMax = cMax > frb ? cMax : frb;
        cMin = frr < frg ? frr : frg;  cMin = cMin < frb ? cMin : frb;
        del_Max = cMax - cMin;
        
        tol = (cMax + cMin) * 120.f;
        if (del_Max == 0.f) {
            toh = 0.f;   tos = 0.f;   tol += .5f;
        } else {
            // final hue
            if (cMax == frr)        toh = 40.f * (frg - frb) / del_Max +    .5f;
            else if (cMax == frg)   toh = 40.f * (frb - frr) / del_Max +  80.5f;
            else                    toh = 40.f * (frr - frg) / del_Max + 160.5f; // cMax == frb
            if (toh < .5f) toh += 240.f;
            
            // final saturation
            if (tol < 120.f)  tos = del_Max * 240.f / (      cMax + cMin) + .5f;
            else              tos = del_Max * 240.f / (2.f - cMax - cMin) + .5f;
            
            // final lightness
            tol += .5f;
        }
        return MCL_FRGB (toh, tos, tol, rgbcolor);

    }
    
   /**
    * @function cvthsl2hsv <src/colors.h>
    * @brief Convert the HSL color to the HSV color
    * @param {color_t} hslcolor: {a} = [0, 255], {h,s,l} = [0, 240]
    * @return {color_t} hsvcolor: {a} = [0, 255], {h,s,v} = [0, 240]
    */
    color_t
    cvthsl2hsv (color_t hslcolor) {
        
        float frs = static_cast<float>(getg4rgb (hslcolor)),
              frl = static_cast<float>(getb4rgb (hslcolor));
        float tos, tov;
        if (frl == 0.f)
            tos = tov = 0;
        else if (frl > 0.f && frl <= 120.f) {
            tos = 480.f * frs / (240.f + frs) + .5f;  // final saturation
            tov = frl * (1.f + frs / 240.f)   + .5f;  // final lightness
        } else {
            tov = frs +  frl - frs * frl / 240.f  + .5f;
            tos = frs * (480.f - 2.f * frl) / tov + .5f;
        }
        return MCL_FRGB ((hslcolor >> 16) & 0xff, tos, tov, hslcolor);

    }
    
   /**
    * @function cvthsv2rgb <src/colors.h>
    * @brief Convert the HSV color to the RGB color.
    * @param {color_t} hsvcolor: {a} = [0, 255], {h,s,v} = [0, 240]
    * @return {color_t} rgbcolor: {a,r,g,b} = [0, 255]
    */
    color_t
    cvthsv2rgb (color_t hsvcolor) {
    
        color_t h     = getr4rgb(hsvcolor);
        color_t tempH = h / 40ul;
        float tor, tog, tob;
        float frh = static_cast<float>(h) / 40.f - static_cast<float>(tempH),
              frs = static_cast<float>(getg4rgb(hsvcolor)) / 240.f,
              frv = static_cast<float>(getb4rgb(hsvcolor)) / 240.f * 255.f;
        float cv1 = frv + .5f,
              cv2 = frv * (1.f - frs) + .5f,
              cv3 = frv * (1.f - frh  * frs) + .5f,
              cv4 = frv * (1.f - (1.f - frh) * frs) + .5f;
        switch (tempH) {
            case 0:  tor = cv1; tog = cv4; tob = cv2; break;
            case 1:  tor = cv3; tog = cv1; tob = cv2; break;
            case 2:  tor = cv2; tog = cv1; tob = cv4; break;
            case 3:  tor = cv2; tog = cv3; tob = cv1; break;
            case 4:  tor = cv4; tog = cv2; tob = cv1; break;
            case 5:  tor = cv1; tog = cv2; tob = cv3; break;
            default: tor =  0 ; tog =  0 ; tob =  0 ; break;
        }
        return MCL_FRGB (tor, tog, tob, hsvcolor);

    }
    
   /**
    * @function cvtrgb2hsv <src/colors.h>
    * @brief Convert the RGB color to the HSV color.
    * @param {color_t} rgbcolor: {a,r,g,b} = [0, 255]
    * @return {color_t} hsvcolor: {a} = [0, 255], {h,s,v} = [0, 240]
    */
    color_t
    cvtrgb2hsv (color_t rgbcolor) {
        
        float toh, tos, tov, cMax, cMin, del_Max,
            frr = static_cast<float>(getr4rgb(rgbcolor)) / 255.f,
            frg = static_cast<float>(getg4rgb(rgbcolor)) / 255.f,
            frb = static_cast<float>(getb4rgb(rgbcolor)) / 255.f;
        cMin = frr < frg ? frr : frg;  cMin = cMin < frb ? cMin : frb;
        cMax = frr > frg ? frr : frg;  cMax = cMax > frb ? cMax : frb;
        del_Max = cMax - cMin;
        
        // final value
        tov = cMax * 240.f + .5f;
        if (del_Max == 0.f) {
            toh = 0.f;   tos = 0.f;
        } else {
            // final hue
            if (cMax == frr)        toh = 40.f * (frg - frb) / del_Max +    .5f;
            else if (cMax == frg)   toh = 40.f * (frb - frr) / del_Max +  80.5f;
            else                    toh = 40.f * (frr - frg) / del_Max + 160.5f; // cMax == frb
            if (toh < .5f) toh += 240.f;
            
            // final saturation
            tos = del_Max * 240.f / cMax + .5f;
        }
        return MCL_FRGB (toh, tos, tov, rgbcolor);

    }
    
   /**
    * @function cvthsv2hsl <src/colors.h>
    * @brief Convert the HSV color to the HSL color.
    * @param {color_t} hsvcolor: {a} = [0, 255], {h,s,v} = [0, 240]
    * @return {color_t} hslcolor: {a} = [0, 255], {h,s,l} = [0, 240]
    */
    color_t cvthsv2hsl (color_t hsvcolor) {
        
        float frs = static_cast<float>(getg4rgb(hsvcolor)),
              frv = static_cast<float>(getb4rgb(hsvcolor));
        float tos, tol;
        if (frv == 0.f)
            tos = tol = 0;
        else if (frv <= 240.f / (2.f - frs / 240.f)) {
            tos = 240.f * frs / (480.f - frs) + .5f;  // final saturation
            tol = frv * (1.f - frs / 480.f)   + .5f;  // final lightness
        } else {
            tos = frv * frs / (480.f + frv * (frs - 480.f) / 240.f) + .5f;
            tol = frv * (1.f - frs / 480.f)                         + .5f;
        }
        return MCL_FRGB ((hsvcolor >> 16) & 0xff, tos, tol, hsvcolor);

    }
    
   /**
    * @function cvthsl2rgb <src/colors.h>
    * @brief Convert the HSL color to the RGB color.
    * @param {color_t} hslcolor: {a} = [0, 255], {h,s,l} = [0, 240]
    * @return {color_t} rgbcolor: {a,r,g,b} = [0, 255]
    */
    color_t
    cvthsl2rgb (color_t hslcolor) {
        
        float q, p, tor, tog, tob,
              frh = static_cast<float>(getr4rgb(hslcolor)) / 240.f,
              frs = static_cast<float>(getg4rgb(hslcolor)) / 240.f,
              frl = static_cast<float>(getb4rgb(hslcolor)) / 240.f;
        if (frs == 0.f) {
            frl = frl * 255.f + .5f;
            return MCL_FRGB (frl, frl, frl, hslcolor);
        }
        if (frl < .5f) q = frl * (1.f + frs);
        else           q = frl + frs - frl * frs;
        p = 2.f * frl - q;
        float color[3]{ frh + 1.f/3.f, frh, frh - 1.f/3.f };
        for (int i = 0; i < 3; ++ i) {
            if (color[i] < 0.f) color[i] += 1.f;
            if (color[i] > 1.f) color[i] -= 1.f;
            if (color[i] * 6.f < 1.f)      color[i] = p + (q - p) * 6.f * color[i];
            else if (color[i] * 2.f < 1.f) color[i] = q;
            else if (color[i] * 3.f < 2.f) color[i] = p + (q - p) * (2.f/3.f - color[i]) * 6.f;
            else                           color[i] = p;
        }
        tor = (color[0] > 1.f ? 1.f : (color[0] < 0.f ? 0.f : color[0])) * 255.f + .5f;
        tog = (color[1] > 1.f ? 1.f : (color[1] < 0.f ? 0.f : color[1])) * 255.f + .5f;
        tob = (color[2] > 1.f ? 1.f : (color[2] < 0.f ? 0.f : color[2])) * 255.f + .5f;
        return MCL_FRGB (tor, tog, tob, hslcolor);

    }

#undef MCL_FRGB

}
