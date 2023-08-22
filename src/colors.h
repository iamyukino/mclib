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
    
    @file src/colors.h
    This is a C++11 header for synthesizing colors. mclib uses 24bit
    true color and does not support palette mode.
*/

#ifndef MCL_COLORS
# define MCL_COLORS

# include "mclfwd.h"

namespace
mcl {

   /**
    * @unimplemented
    *     pygame.Color.correct_gamma()
    */

    enum: color_t{
        
        aliceblue    =  0xff0000cd,  
        antiquewhite =  0xfffaebd7,  
        aqua         =  0xff00ffff,  
        aquamarine   =  0xff7fffd4,  
        azure        =  0xfff0ffff,  
        
        beige        =  0xfff5f5dc,  
        bisque       =  0xffffe4c4,  
        black        =  0xff000000,  
        blanchedalmond= 0xffffebcd,  
        blue         =  0xff0000a8,  
        blueviolet   =  0xff8a2be2,  
        brown        =  0xffa52a2a,  
        burlywood    =  0xffdeb887,  
        
        cadetblue    =  0xff5f9ea0,  
        chartreuse   =  0xff7fff00,  
        chocolate    =  0xffd2691e,  
        coral        =  0xffff7f50,  
        cornflowerblue= 0xff6495ed,  
        cornsilk     =  0xfffff8dc,  
        crimson      =  0xffdc143c,  
        cyan         =  0xff00ffff,  
        
        darkblue     =  0xff00008b,  
        darkcyan     =  0xff008b8b,  
        darkgoldenrod=  0xffb8860b,  
        darkgray     =  0xffa9a9a9,  
        darkgreen    =  0xff006400,  
        darkgrey     =  0xffa9a9a9,  
        darkkhaki    =  0xffbdb76b,  
        darkmagenta  =  0xff8b008b,  
        darkolivegreen= 0xff556b2f,  
        darkorange   =  0xffff8c00,  
        darkorchid   =  0xff9932cc,  
        darkred      =  0xff8b0000,  
        darksalmon   =  0xffe9967a,  
        darkseagreen =  0xff8fbc8f,  
        darkslateblue=  0xff483d8b,  
        darkslategray=  0xff2f4f4f,  
        darkturquoise=  0xff00ced1,  
        darkviolet   =  0xff8b008b,  
        deeppink     =  0xffff1493,  
        deepskyblue  =  0xff00bfff,  
        dimgray      =  0xff696969,  
        dimgrey      =  0xff696969,  
        dodgerblue   =  0xff1e90ff,  
        
        firebrick    =  0xffb22222,  
        floralwhite  =  0xfffffaf0,  
        forestgreen  =  0xff228b22,  
        fuchsia      =  0xffff00ff,  
        
        gainsboro    =  0xffdcdcdc,  
        ghostwhite   =  0xfff8f8ff,  
        girlpowder   =  0xffe8b5b5,  
        gold         =  0xffffd700,  
        goldenrod    =  0xffdaa520,  
        gray         =  0xff808080,  
        green        =  0xff008000,  
        greenyellow  =  0xffadff2f,  
        grey         =  0xff808080,  
        
        honeydew     =  0xfff0fff0,  
        hotpink      =  0xffff69b4,  
        
        indianred    =  0xffcd5c5c,  
        indigo       =  0xff4b0082,  
        ivory        =  0xfffffff0,  
        
        khaki        =  0xfff0e68c,  
        
        lavender     =  0xffe6e6fa,  
        lavenderblush=  0xfffff0f5,  
        lawngreen    =  0xff7cfc00,  
        lemonchiffon =  0xfffffacd,  
        lightblue    =  0xffadd8e6,  
        lightcoral   =  0xfff08080,  
        lightcyan    =  0xffe0ffff,  
        lightgoldenrodyellow=0xfffafad2,
        lightgray    =  0xffa8a8a8,  
        lightgreen   =  0xff90ee90,  
        lightgrey    =  0xffd3d3d3,  
        lightmagenta =  0xfffc54fc,  
        lightpink    =  0xffffb6c1,  
        lightred     =  0xfffc5454,  
        lightsalmon  =  0xffffa07a,  
        lightseagreen=  0xff20b2aa,  
        lightskyblue =  0xff87cefa,  
        lightslateblue= 0xff778899,  
        lightsteelblue= 0xffb0c4de,  
        lightyellow  =  0xffffffe0,  
        lime         =  0xff00ff00,  
        limegreen    =  0xff32cd32,  
        linen        =  0xfffaf0e6,  
        
        magenta      =  0xffa800a8,  
        maroon       =  0xff800000,  
        mediumaquamarine=0xff66cdaa, 
        mediumblue   =  0xff0000cd,  
        mediumorchid =  0xffba55d3,  
        mediumpurple =  0xff9370db,  
        mediumseagreen= 0xff3cb371,  
        mediumslateblue=0xff7b68ee,  
        mediumspringgreen=0xff00fa9a,
        mediumturquoise=0xff48d1cc,  
        mediumvioletred=0xffc71585,  
        midnightblue =  0xff191970,  
        mintcream    =  0xfff5fffa,  
        mistyrose    =  0xffffe4e1,  
        moccasin     =  0xffffe4b5,  
        
        navajowhite  =  0xffffdead,  
        navy         =  0xff000080,  
        
        oldlace      =  0xfffdf5e6,  
        olive        =  0xff808000,  
        olivedrab    =  0xff6b8e23,  
        orange       =  0xffffa500,  
        orangered    =  0xffff4500,  
        orchid       =  0xffda70d6,  
        
        palegoldenrod=  0xffeee8aa,  
        palegreen    =  0xff98fb98,  
        paleturquoise=  0xffafeeee,  
        palevioletred=  0xffdb7093,  
        papayawhip   =  0xffffefd5,  
        peachpuff    =  0xffffdab9,  
        peru         =  0xffcd853f,  
        pink         =  0xffffc0cb,  
        plum         =  0xffdda0dd,  
        powderblue   =  0xffb0e0e6,  
        purple       =  0xff800080,  
        
        red          =  0xffff0000,  
        rosybrown    =  0xffbc8f8f,  
        royalblue    =  0xff4169e1,  
        
        saddlebrown  =  0xff8b4513,  
        salmon       =  0xfffa8072,  
        sandybrown   =  0xfff4a460,  
        seagreen     =  0xff2e8b57,  
        seashell     =  0xfffff5ee,  
        sienna       =  0xffa0522d,  
        silver       =  0xffc0c0c0,  
        skyblue      =  0xff87ceeb,  
        slateblue    =  0xff6a5acd,  
        slategray    =  0xff708090,  
        snow         =  0xfffffafa,  
        springgreen  =  0xff00ff7f,  
        steelblue    =  0xff4682b4,  
        
        tan          =  0xffd2b48c,  
        teal         =  0xff008080,  
        thistle      =  0xffd8b0d8,  
        tomato       =  0xffff6347,  
        turquoise    =  0xff40e0d0,  
        
        violet       =  0xffee82ee,  
        
        wheat        =  0xfff5deb3,  
        white        =  0xffffffff,  
        whitesmoke   =  0xfff5f5f5,  
        
        yellow       =  0xffffff00,  
        yellowgreen  =  0xff9acd32,  
        
        transparent  =  0x00000000,  
        opaque       =  0xff000000,
    };

    // Convert the rgbcolor to the hslcolor.
    color_t cvtrgb2hsl (color_t rgbcolor);
    // Convert the hslcolor to the hsvcolor.
    color_t cvthsl2hsv (color_t hslcolor);
    // Convert the hsvcolor to the rgbcolor.
    color_t cvthsv2rgb (color_t hsvcolor);
    // Convert the rgbcolor to the hsvcolor.
    color_t cvtrgb2hsv (color_t rgbcolor);
    // Convert the hsvcolor to the hslcolor.
    color_t cvthsv2hsl (color_t hsvcolor);
    // Convert the hslcolor to the rgbcolor.
    color_t cvthsl2rgb (color_t hslcolor);

    // Return the alpha value in the specified rgbcolor.
    constexpr color_t geta4rgb (color_t rgbcolor)
    { return rgbcolor >> 24;        }
    // Return the red value in the specified rgbcolor.
    constexpr color_t getr4rgb (color_t rgbcolor)
    { return rgbcolor >> 16 & 0xff; }
    // Return the green value in the specified rgbcolor.
    constexpr color_t getg4rgb (color_t rgbcolor)
    { return rgbcolor >> 8  & 0xff; }
    // Return the blue value in the specified rgbcolor.
    constexpr color_t getb4rgb (color_t rgbcolor)
    { return rgbcolor       & 0xff; }
    // Change the alpha value in the specified rgbcolor.
    constexpr color_t seta4rgb (color_t byAlpha, color_t rgbcolor)
    { return byAlpha << 24 | rgbcolor & 0x00ffffff; }
    // Change the red value in the specified rgbcolor.
    constexpr color_t setr4rgb (color_t byRed, color_t rgbcolor)
    { return byRed   << 16 | rgbcolor & 0xff00ffff; }
    // Change the green value in the specified rgbcolor.
    constexpr color_t setg4rgb (color_t byGreen, color_t rgbcolor)
    { return byGreen << 8  | rgbcolor & 0xffff00ff; }
    // Change the blue value in the specified rgbcolor.
    constexpr color_t setb4rgb (color_t byBlue, color_t rgbcolor)
    { return byBlue        | rgbcolor & 0xffffff00; }


   /**
    * @function rgb <src/colors.h>
    * @brief Synthesize colors with rgb components
    * @param[in] {color_t} byRed: [0, 255]
    * @param[in] {color_t} byGreen: [0, 255]
    * @param[in] {color_t} byBlue: [0, 255]
    */
    constexpr color_t
    rgb (color_t byRed, color_t byGreen, color_t byBlue) {
        return 0xff000000 | byRed << 16 | byGreen << 8 | byBlue;
    }

    /**
     * @function rgba <src/colors.h>
     * @brief Synthesize colors with rgba components
     * @param[in] {color_t} byRed: [0, 255]
     * @param[in] {color_t} byGreen: [0, 255]
     * @param[in] {color_t} byBlue: [0, 255]
     * @param[in] {color_t} byAlpha: [0, 255]
     */
    constexpr color_t
    rgba (color_t byRed, color_t byGreen, color_t byBlue, color_t byAlpha = 255) {
        return byAlpha << 24 | byRed << 16 | byGreen << 8 | byBlue;
    }
    
   /**
    * @function cmy <src/colors.h>
    * @brief Synthesize colors with cmy components
    * @param[in] {float} byCyan: [0, 1]
    * @param[in] {float} byMagenta: [0, 1]
    * @param[in] {float} byYellow: [0, 1]
    * @param[in] {float} byAlpha: [0, 1]
    */
    constexpr color_t
    cmy (float byCyan, float byMagenta, float byYellow, float byAlpha = 1.f) {
        return    static_cast<color_t>(byAlpha   * 255.f + .5f)  << 24 |
           (255 - static_cast<color_t>(byCyan    * 255.f + .5f)) << 16 | 
           (255 - static_cast<color_t>(byMagenta * 255.f + .5f)) << 8  | 
           (255 - static_cast<color_t>(byYellow  * 255.f + .5f))      ;
    }

    /**
     * @function hsv <src/colors.h>
     * @brief Synthesize colors with hue, saturation, and value components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 100]
     * @param[in] {float} byValue: [0, 100]
     */
    inline color_t
    hsv (color_t byHue, float bySaturation, float byValue) {
        return cvthsv2rgb (0xff000000 | 
            static_cast<color_t>(float(byHue % 360 << 1) / 3.f + .5f) << 16 | 
            static_cast<color_t>(bySaturation * 2.4f           + .5f) << 8  | 
            static_cast<color_t>(byValue      * 2.4f           + .5f)     );
    }

    /**
     * @function hsva <src/colors.h>
     * @brief Synthesize colors with hue, saturation, value and alpha components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 100]
     * @param[in] {float} byValue: [0, 100]
     * @param[in] {float} byAlpha: [0, 100]
     */
    inline color_t
    hsva (color_t byHue, float bySaturation, float byValue, float byAlpha = 100.f) {
        return cvthsv2rgb (
            static_cast<color_t>(byAlpha      * 2.55f          + .5f) << 24 | 
            static_cast<color_t>(float(byHue % 360 << 1) / 3.f + .5f) << 16 | 
            static_cast<color_t>(bySaturation * 2.4f           + .5f) << 8  |
            static_cast<color_t>(byValue      * 2.4f           + .5f)     );
    }

    /**
     * @function hsl <src/colors.h>
     * @brief Synthesize colors with hue, saturation, and lightness components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 100]
     * @param[in] {float} byLightness: [0, 100]
     */
    inline color_t
    hsl (color_t byHue, float bySaturation, float byLightness) {
        return cvthsl2rgb (0xff000000 | 
            static_cast<color_t>(float(byHue % 360 << 1) / 3.f + .5f) << 16 | 
            static_cast<color_t>(bySaturation * 2.4f           + .5f) << 8  |
            static_cast<color_t>(byLightness  * 2.4f           + .5f)     );
    }

    /**
     * @function hsla <src/colors.h>
     * @brief Synthesize colors with hue, saturation, lightness and alpha components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 100]
     * @param[in] {float} byLightness: [0, 100]
     * @param[in] {float} byAlpha: [0, 100]
     */
    inline color_t
    hsla (color_t byHue, float bySaturation, float byLightness, float byAlpha = 100.f) {
        return cvthsl2rgb (
            static_cast<color_t>(byAlpha      * 2.55f          + .5f) << 24 | 
            static_cast<color_t>(float(byHue % 360 << 1) / 3.f + .5f) << 16 | 
            static_cast<color_t>(bySaturation * 2.4f + .5f)           << 8  |
            static_cast<color_t>(byLightness  * 2.4f + .5f)               );
    }

   /**
    * @function i1i2i3 <src/colors.h>
    * @brief Synthesize colors with i1i2i3 components.
    * @param[in] {float} byI1: [0, 1]
    * @param[in] {float} byI2: [-0.5, 0.5]
    * @param[in] {float} byI3: [-0.5, 0.5]
    * @param[in] {float} byAlpha: [0, 1]
    */
    inline color_t
    i1i2i3 (float byI1, float byI2, float byI3, float byAlpha = 1.f) {
        return byI1 *= 255.f, byI2 *= 255.f, byI3 *= 170.f,
            static_cast<color_t>(byAlpha * 255.f    + .5f) << 24 |
            static_cast<color_t>(byI1 + byI2 - byI3 + .5f) << 16 |
            static_cast<color_t>(byI1 + byI3 + byI3 + .5f) << 8  |
            static_cast<color_t>(byI1 - byI2 - byI3 + .5f)      ;
    }

   /**
    * @function grayscale <src/colors.h>
    * @brief returns the grayscale of a color.
    * @param[in] {color_t} rgbcolor
    */
    constexpr color_t
    grayscale (color_t rgbcolor) {
        return ( (rgbcolor >> 16 & 0xff) * 299 +
                 (rgbcolor >> 8  & 0xff) * 587 +
                 (rgbcolor       & 0xff) * 114  ) / 1000;
    }
    
   /**
    * @function lerp <src/colors.h>
    * @brief returns a linear interpolation to the given color
    * @param[in] {color_t} self
    * @param[in] {color_t} other
    * @param[in] {float} distance: [0, 1]
    */
    constexpr color_t
    lerp (color_t self, color_t other, float distance) {
        return
            color_t(float(self >> 24       ) * (1.f - distance) + float(other >> 24       ) * distance) << 24 | 
            color_t(float(self >> 16 & 0xff) * (1.f - distance) + float(other >> 16 & 0xff) * distance) << 16 |
            color_t(float(self >> 8  & 0xff) * (1.f - distance) + float(other >> 8  & 0xff) * distance) << 8  |
            color_t(float(self       & 0xff) * (1.f - distance) + float(other       & 0xff) * distance)      ;
    }

   /**
    * @function premul_alpha <src/colors.h>
    * @brief returns a color where the r,g,b components
    *     have been multiplied by the alpha.
    * @param[in] {color_t} rgbcolor
    */
    inline color_t
    premul_alpha (color_t rgbcolor) {
        float alpha = static_cast<float>(rgbcolor >> 24) / 255.f;
        return rgbcolor & 0xff000000 | 
            color_t(float(rgbcolor >> 16 & 0xff) * alpha + .5f) << 16 |
            color_t(float(rgbcolor >> 8  & 0xff) * alpha + .5f) << 8  |
            color_t(float(rgbcolor       & 0xff) * alpha + .5f)      ;
    }

}  // namespace

#endif  // MCL_COLORS
