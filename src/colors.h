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
    This is a C++11 header for synthesizing colors.
*/

#ifndef MCL_COLORS
# define MCL_COLORS

# include "mclfwd.h"

 /**
  * @brief
  *  'colors.h' uses 24bit true color and does not
  *  support palette mode.
  *  
  *  Color Representation Rule is:
  *  a = alpha, r = red, g = green, b = brue,
  *  h = hue, s = saturation, l = lightness, v = value
  *  -rgb: 0xaarrggbb  (0 <= a,r,g,b <= 255)
  *  -hsl: 0xaahhssll  (0 <= a <= 255, 0 <= h,s,l <= 240)
  *  -hsv: 0xaahhssvv  (0 <= a <= 255, 0 <= h,s,v <= 240)
  * 
  * @function
  *    x = a, r, g, b, h, s, l, v, g
  *    y = rgb, hsl, hsv, gray
  * > function  mcl::makyyy ()
  * > function  mcl::getx4yyy ()
  * > function  mcl::setx4yyy ()
  * > function  mcl::cvt...2... ()
  *         (... = rgb-bgr, rgb-gray, rgb-hsl, rgb-hsv,
  *             hsl-rgb, hsl-hsv, hsv-rgb, hsv-hsl)
  */

namespace
mcl {
    
    enum
    mcl_colors_enum
    : color_t {
            
        clrLightPink    =  0xffffb6c1,  
        clrGirlPowder   =  0xffe8b5b5,  
        clrPink         =  0xffffc0cb,  
        clrCrimson      =  0xffdc143c,  
        clrLavenderBlush=  0xfffff0f5,  
        clrPaleVioletRed=  0xffdb7093,  
        clrHotPink      =  0xffff69b4,  
        clrDeepPink     =  0xffff1493,  
        clrLightMagenta =  0xfffc54fc,  
        clrMediumVioletRed=0xffc71585,  
        clrOrchid       =  0xffda70d6,  
        clrThistle      =  0xffd8b0d8,  
        clrPlum         =  0xffdda0dd,  
        clrMagenta      =  0xffa800a8,  
        clrLightRed     =  0xfffc5454,  
        clrViolet       =  0xffee82ee,  
        
        clrFuchsia      =  0xffff00ff,  
        clrDarkMagenta  =  0xff8b008b,  
        clrPurple       =  0xff800080,  
        clrMediumOrchid =  0xffba55d3,  
        clrDarkViolet   =  0xff8b008b,  
        clrDarkOrchid   =  0xff9932cc,  
        clrIndigo       =  0xff4b0082,  
        clrBlueViolet   =  0xff8a2be2,  
        clrMediumPurple =  0xff9370db,  
        clrMediumSlateBlue=0xff7b68ee,  
        clrSlateBlue    =  0xff6a5acd,  
        clrDarkSlateBlue=  0xff483d8b,  
        clrLavender     =  0xffe6e6fa,  
        clrGhostWhite   =  0xfff8f8ff,  
        clrMediumBlue   =  0xff0000cd,  
        clrBlue         =  0xff0000a8,  
        
        clrMidnightBlue =  0xff191970,  
        clrDarkBlue     =  0xff00008b,  
        clrNavy         =  0xff000080,  
        clrRoyalBlue    =  0xff4169e1,  
        clrCornflowerBlue= 0xff6495ed,  
        clrLightSteelBlue= 0xffb0c4de,  
        clrLightSlateBlue= 0xff778899,  
        clrSlateGray    =  0xff708090,  
        clrDodgerBlue   =  0xff1e90ff,  
        clrAliceBlue    =  0xff0000cd,  
        clrSteelBlue    =  0xff4682b4,  
        clrLightSkyBlue =  0xff87cefa,  
        clrSkyBlue      =  0xff87ceeb,  
        clrDeepSkyBlue  =  0xff00bfff,  
        clrLightBlue    =  0xffadd8e6,  
        clrPowderBlue   =  0xffb0e0e6,  
         
        clrCadetBlue    =  0xff5f9ea0,  
        clrAzure        =  0xfff0ffff,  
        clrLightCyan    =  0xffe0ffff,  
        clrPaleTurquoise=  0xffafeeee,  
        clrCyan         =  0xff00ffff,  
        clrAqua         =  0xff00ffff,  
        clrDarkTurquoise=  0xff00ced1,  
        clrDarkSlateGray=  0xff2f4f4f,  
        clrDarkCyan     =  0xff008b8b,  
        clrTeal         =  0xff008080,  
        clrMediumTurquoise=0xff48d1cc,  
        clrLightSeaGreen=  0xff20b2aa,  
        clrTurquoise    =  0xff40e0d0,  
        clrAquamarine   =  0xff7fffd4,  
        clrMediumAquamarine=0xff66cdaa, 
        clrMediumSpringGreen=0xff00fa9a,
        
        clrMintCream    =  0xfff5fffa,  
        clrSpringGreen  =  0xff00ff7f,  
        clrMediumSeaGreen= 0xff3cb371,  
        clrSeaGreen     =  0xff2e8b57,  
        clrHoneydew     =  0xfff0fff0,  
        clrLightGreen   =  0xff90ee90,  
        clrPaleGreen    =  0xff98fb98,  
        clrDarkSeaGreen =  0xff8fbc8f,  
        clrLimeGreen    =  0xff32cd32,  
        clrLime         =  0xff00ff00,  
        clrForestGreen  =  0xff228b22,  
        clrGreen        =  0xff008000,  
        clrDarkGreen    =  0xff006400,  
        clrChartreuse   =  0xff7fff00,  
        clrLawnGreen    =  0xff7cfc00,  
        clrGreenYellow  =  0xffadff2f,  
        
        clrDarkOliveGreen= 0xff556b2f,  
        clrYellowGreen  =  0xff9acd32,  
        clrOliveDrab    =  0xff6b8e23,  
        clrBeige        =  0xfff5f5dc,  
        clrLightGoldenrodYellow=0xfffafad2,
        clrIvory        =  0xfffffff0,  
        clrLightYellow  =  0xffffffe0,  
        clrYellow       =  0xffffff00,  
        clrOlive        =  0xff808000,  
        clrDarkKhaki    =  0xffbdb76b,  
        clrLemonChiffon =  0xfffffacd,  
        clrPaleGoldenrod=  0xffeee8aa,  
        clrKhaki        =  0xfff0e68c,  
        clrGold         =  0xffffd700,  
        clrCornsilk     =  0xfffff8dc,  
        clrGoldenrod    =  0xffdaa520,  
        
        clrDarkGoldenrod=  0xffb8860b,  
        clrfloralWhite  =  0xfffffaf0,  
        clrOldLace      =  0xfffdf5e6,  
        clrWheat        =  0xfff5deb3,  
        clrMoccasin     =  0xffffe4b5,  
        clrOrange       =  0xffffa500,  
        clrPapayaWhip   =  0xffffefd5,  
        clrBlanchedAlmond= 0xffffebcd,  
        clrNavajoWhite  =  0xffffdead,  
        clrAntiqueWhite =  0xfffaebd7,  
        clrTan          =  0xffd2b48c,  
        clrBurlyWood    =  0xffdeb887,  
        clrBisque       =  0xffffe4c4,  
        clrDarkOrange   =  0xffff8c00,  
        clrLinen        =  0xfffaf0e6,  
        clrPeru         =  0xffcd853f,  
        
        clrPeachPuff    =  0xffffdab9,  
        clrSandyBrown   =  0xfff4a460,  
        clrChocolate    =  0xffd2691e,  
        clrSaddleBrown  =  0xff8b4513,  
        clrSeashell     =  0xfffff5ee,  
        clrSienna       =  0xffa0522d,  
        clrLightSalmon  =  0xffffa07a,  
        clrCoral        =  0xffff7f50,  
        clrOrangeRed    =  0xffff4500,  
        clrDarkSalmon   =  0xffe9967a,  
        clrTomato       =  0xffff6347,  
        clrMistyRose    =  0xffffe4e1,  
        clrSalmon       =  0xfffa8072,  
        clrSnow         =  0xfffffafa,  
        clrLightCoral   =  0xfff08080,  
        clrRosyBrown    =  0xffbc8f8f,  
        
        clrIndianRed    =  0xffcd5c5c,  
        clrRed          =  0xffff0000,  
        clrBrown        =  0xffa52a2a,  
        clrFireBrick    =  0xffb22222,  
        clrDarkRed      =  0xff8b0000,  
        clrMaroon       =  0xff800000,  
        clrWhite        =  0xffffffff,  
        clrWhiteSmoke   =  0xfff5f5f5,  
        clrGainsboro    =  0xffdcdcdc,  
        clrLightGray    =  0xffa8a8a8,  
        clrLightGrey    =  0xffd3d3d3,  
        clrSilver       =  0xffc0c0c0,  
        clrDarkGray     =  0xffa9a9a9,  
        clrDimGray      =  0xff696969,  
        clrBlack        =  0xff000000, 
        clrGray         =  0xff808080,   
        clrGrey         =  0xff808080,   

        clrTransparent  =  0x00000000,  
        clrCurrentcolor =  0x00ffffff,
    };
    
    // Synthesize colors with red, green, and blue components.
    constexpr color_t
    makrgb (colorcv_t byRed, colorcv_t byGreen,
           colorcv_t byBlue, colorcv_t byAlpha = 255) {
        return ( static_cast<color_t>(byAlpha) << 24)
            |  ( static_cast<color_t>(byRed)   << 16)
            |  ( static_cast<color_t>(byGreen) << 8 )
            |  ( static_cast<color_t>(byBlue)       );
    }

    // Synthesize colors with hue, saturation, lightness, alpha components.
    constexpr color_t
    makhsl (colorcv_t byHue,   colorcv_t bySaturation,
        colorcv_t byLightness, colorcv_t byAlpha = 255) {
        return makrgb (byHue, bySaturation, byLightness, byAlpha);
    }

    // Synthesize colors with hue, saturation, value, alpha components.
    constexpr color_t
    makhsv (colorcv_t byHue,   colorcv_t bySaturation,
          colorcv_t byValue,   colorcv_t byAlpha = 255) {
        return makrgb (byHue, bySaturation, byValue, byAlpha);
    }

    // Synthesize colors with lightness components.
    constexpr color_t
    makgray (colorcv_t byLight, colorcv_t byAlpha = 255) {
        return ( static_cast<color_t>(byAlpha) << 24 )
            |  ( static_cast<color_t>(byLight) << 16 )
            |  ( static_cast<color_t>(byLight) << 8  )
            |  ( static_cast<color_t>(byLight)       );
    }

    // Return the alpha value in the specified rgbcolor.
    constexpr colorcv_t
    geta4rgb (color_t rgbcolor) {
        return static_cast<colorcv_t>(   rgbcolor >> 24          );
    }

    // Return the red value in the specified rgbcolor.
    constexpr colorcv_t
    getr4rgb (color_t rgbcolor) {
        return static_cast<colorcv_t>( ( rgbcolor >> 16 ) & 0xff );
    }

    // Return the green value in the specified rgbcolor.
    constexpr colorcv_t
    getg4rgb (color_t rgbcolor) {
        return static_cast<colorcv_t>( ( rgbcolor >>  8 ) & 0xff );
    }

    // Return the blue value in the specified rgbcolor.
    constexpr colorcv_t
    getb4rgb (color_t rgbcolor) {
        return static_cast<colorcv_t>(   rgbcolor         & 0xff );
    }

    // Return the alpha value in the specified hslcolor.
    constexpr colorcv_t
    geta4hsl (color_t hslcolor) {
        return geta4rgb (hslcolor);
    }

    // Return the hue value in the specified hslcolor.
    constexpr colorcv_t
    geth4hsl (color_t hslcolor) {
        return getr4rgb (hslcolor);
    }

    // Return the saturation value in the specified hslcolor.
    constexpr colorcv_t
    gets4hsl (color_t hslcolor) {
        return getg4rgb (hslcolor);
    }

    // Return the lightness value in the specified hslcolor.
    constexpr colorcv_t
    getl4hsl (color_t hslcolor) {
        return getb4rgb (hslcolor);
    }

    // Return the alpha value in the specified hsvcolor.
    constexpr colorcv_t
    geta4hsv (color_t hsvcolor) {
        return geta4rgb (hsvcolor);
    }

    // Return the hue value in the specified hsvcolor.
    constexpr colorcv_t
    geth4hsv (color_t hsvcolor) {
        return getr4rgb (hsvcolor);
    }

    // Return the saturation value in the specified hsvcolor.
    constexpr colorcv_t
    gets4hsv (color_t hsvcolor) {
        return getg4rgb (hsvcolor);
    }

    // Return the brightness value in the specified hslcolor.
    constexpr colorcv_t
    getv4hsv (color_t hsvcolor) {
        return getb4rgb (hsvcolor);
    }

    // Return the gray value in the specified graycolor.
    constexpr colorcv_t
    getg4gray (color_t graycolor) {
        return getb4rgb (graycolor);
    }

    // Change the alpha value in the specified rgbcolor.
    constexpr color_t
    seta4rgb (colorcv_t alpha, color_t rgbcolor) {
        return ( static_cast<color_t>(alpha) << 24 ) | ( rgbcolor & 0x00ffffff );
    }

    // Change the red value in the specified rgbcolor.
    constexpr color_t
    setr4rgb (colorcv_t red  , color_t rgbcolor) {
        return ( static_cast<color_t>(red)   << 16 ) | ( rgbcolor & 0xff00ffff );
    }

    // Change the green value in the specified rgbcolor.
    constexpr color_t
    setg4rgb (colorcv_t green, color_t rgbcolor) {
        return ( static_cast<color_t>(green) << 8  ) | ( rgbcolor & 0xffff00ff );
    }

    // Change the blue value in the specified rgbcolor.
    constexpr color_t
    setb4rgb (colorcv_t blue, color_t rgbcolor) {
        return ( static_cast<color_t>(blue)        ) | ( rgbcolor & 0xffffff00 );
    }

    // Change the alpha value in the specified hslcolor.
    constexpr color_t
    seta4hsl (colorcv_t alpha, color_t hslcolor) {
        return seta4rgb (alpha, hslcolor);
    }

    // Change the hue value in the specified hslcolor.
    constexpr color_t
    seth4hsl (colorcv_t hue, color_t hslcolor) {
        return setr4rgb (hue, hslcolor);
    }

    // Change the saturation value in the specified hslcolor.
    constexpr color_t
    sets4hsl (colorcv_t saturation, color_t hslcolor) {
        return setg4rgb (saturation, hslcolor);
    }

    // Change the lightness value in the specified hslcolor.
    constexpr color_t
    setl4hsl (colorcv_t lightness, color_t hslcolor) {
        return setb4rgb (lightness, hslcolor);
    }

    // Change the alpha value in the specified hsvcolor.
    constexpr color_t
    seta4hsv (colorcv_t alpha, color_t hsvcolor) {
        return seta4rgb (alpha, hsvcolor);
    }

    // Change the hue value in the specified hsvcolor.
    constexpr color_t
    seth4hsv (colorcv_t hue, color_t hsvcolor) {
        return setr4rgb (hue, hsvcolor);
    }

    // Change the saturation value in the specified hsvcolor.
    constexpr color_t
    sets4hsv (colorcv_t saturation, color_t hsvcolor) {
        return setg4rgb (saturation, hsvcolor);
    }

    // Change the brightness value in the specified hsvcolor.
    constexpr color_t
    setv4hsv (colorcv_t value, color_t hsvcolor) {
        return setb4rgb (value, hsvcolor);
    }

    // Swap colors in red and blue.
    constexpr color_t
    cvtrgb2bgr (color_t rgbcolor) {
        return ( (rgbcolor & 0xff    ) << 16 )
            |  ( (rgbcolor & 0xff0000) >> 16 )
            |  (  rgbcolor & 0xff00ff00);
    }

    // Convert the rgbcolor to grayscale.
    constexpr color_t
    cvtrgb2gray (color_t rgbcolor) {
        return makgray (static_cast<colorcv_t>(
            getr4rgb (rgbcolor) * 0.299f + getg4rgb (rgbcolor) * 0.587f
            + getb4rgb (rgbcolor) * 0.114f + .5f ), geta4rgb (rgbcolor) );
    }

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

}  // namespace


/**
 * To make it easier to use colors, mclib also introduces
 * the colour functions in CSS.
 */

namespace
mcl {

    constexpr color_t black   = 0xff000000;
    constexpr color_t silver  = 0xffc0c0c0;
    constexpr color_t gray    = 0xff808080;
    constexpr color_t white   = 0xffffffff;
    constexpr color_t maroon  = 0xff800000;

    constexpr color_t red     = 0xffff0000;
    constexpr color_t purple  = 0xff800080;
    constexpr color_t fuchsia = 0xffff00ff;
    constexpr color_t green   = 0xff008000;
    constexpr color_t lime    = 0xff00ff00;

    constexpr color_t olive   = 0xff808000;
    constexpr color_t yellow  = 0xffffff00;
    constexpr color_t navy    = 0xff000080;
    constexpr color_t blue    = 0xff0000ff;
    constexpr color_t teal    = 0xff008080;

    constexpr color_t aqua    = 0xff00ffff;
    constexpr color_t orange  = 0xffffa500;
    constexpr color_t transparent  = 0x00000000;
    constexpr color_t currentcolor = 0x00ffffff;

    constexpr color_t grey    = gray;
    constexpr color_t cyan    = aqua;
    constexpr color_t magenta = fuchsia;


   /**
    * @function rgb <src/colors.h>
    * @brief Synthesize colors with red, green and blue components.
    * @param[in] {color_t} byRed: [0, 255]
    * @param[in] {color_t} byGreen: [0, 255]
    * @param[in] {color_t} byBlue: [0, 255]
    */
    constexpr color_t
    rgb (color_t byRed, color_t byGreen, color_t byBlue)
    {
        return (byRed << 16) | (byGreen << 8) | byBlue | 0xff000000;
    }

    /**
     * @function rgba <src/colors.h>
     * @brief Synthesize colors with red, green, blue and alpha components.
     * @param[in] {color_t} byRed: [0, 255]
     * @param[in] {color_t} byGreen: [0, 255]
     * @param[in] {color_t} byBlue: [0, 255]
     * @param[in] {float} byAlpha: [0, 1]
     */
    constexpr color_t
    rgba (color_t byRed, color_t byGreen, color_t byBlue, float byAlpha = 1.0f)
    {
        return byAlpha < .001960785f ? transparent :
            (static_cast<color_t>(byAlpha * 255.f + .5f) << 24)
            | (byRed << 16) | (byGreen << 8) | byBlue;
    }

    /**
     * @function hsl <src/colors.h>
     * @brief Synthesize colors with hue, saturation, and lightness components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 1]
     * @param[in] {float} byLightness: [0, 1]
     */
    inline color_t
    hsl (color_t byHue, float bySaturation, float byLightness)
    {
        return cvthsl2rgb (rgb (
            static_cast<color_t>(static_cast<float>(byHue % 360 << 1) / 3.f + .5f),
            static_cast<color_t>(bySaturation * 240.f + .5f),
            static_cast<color_t>(byLightness  * 240.f + .5f)
        ));
    }

    /**
     * @function hsla <src/colors.h>
     * @brief Synthesize colors with hue, saturation, lightness and alpha components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 1]
     * @param[in] {float} byLightness: [0, 1]
     * @param[in] {float} byAlpha: [0, 1]
     */
    inline color_t
    hsla (color_t byHue, float bySaturation, float byLightness, float byAlpha = 1.0f)
    {
        return byAlpha < .001960785f ? transparent :
            cvthsl2rgb (makrgb (
                static_cast<colorcv_t>(byAlpha      * 255.f + .5f),
                static_cast<colorcv_t>(static_cast<float>(
                                    byHue % 360 << 1) / 3.f + .5f),
                static_cast<colorcv_t>(bySaturation * 240.f + .5f),
                static_cast<colorcv_t>(byLightness  * 240.f + .5f)
            ));
    }

    /**
     * @function hsv <src/colors.h>
     * @brief Synthesize colors with hue, saturation, and value components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 1]
     * @param[in] {float} byValue: [0, 1]
     */
    inline color_t
    hsv (color_t byHue, float bySaturation, float byValue)
    {
        return cvthsv2rgb (rgb (
            static_cast<color_t>(static_cast<float>(byHue % 360 << 1) / 3.f + .5f),
            static_cast<color_t>(bySaturation * 240.f + .5f),
            static_cast<color_t>(byValue      * 240.f + .5f)
        ));
    }

    /**
     * @function hsva <src/colors.h>
     * @brief Synthesize colors with hue, saturation, value and alpha components.
     * @param[in] {color_t} byHue: [0, 360]
     * @param[in] {float} bySaturation: [0, 1]
     * @param[in] {float} byValue: [0, 1]
     * @param[in] {float} byAlpha: [0, 1]
     */
    inline color_t
    hsva (color_t byHue, float bySaturation, float byValue, float byAlpha = 1.0f)
    {
        return byAlpha < .001960785f ? transparent :
            cvthsv2rgb (makrgb (
                static_cast<colorcv_t>(byAlpha      * 255.f + .5f),
                static_cast<colorcv_t>(static_cast<float>(
                                    byHue % 360 << 1) / 3.f + .5f),
                static_cast<colorcv_t>(bySaturation * 240.f + .5f),
                static_cast<colorcv_t>(byValue      * 240.f + .5f)
            ));
    }
    
}  // namespace

#endif  // MCL_COLORS
