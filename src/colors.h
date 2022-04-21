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
    
   /**
    * @enum mcl_colors_enum <src/colors.h>
    * @brief 145 rgb color values
    * @ingroup mclib
    * @ingroup draw
    */
    enum
    mcl_colors_enum
    : color_t {
        
    //   color name    color value    chinese name
    
        clrLightPink    =  0xffb6c1,  // 浅粉 
        clrGirlPowder   =  0xe8b5b5,  // 少女粉 
        clrPink         =  0xffc0cb,  // 粉红 
        clrCrimson      =  0xdc143c,  // 猩红 
        clrLavenderBlush=  0xfff0f5,  // 淡紫红 
        clrPaleVioletRed=  0xdb7093,  // 弱紫罗兰红 
        clrHotPink      =  0xff69b4,  // 热情的粉 
        clrDeepPink     =  0xff1493,  // 深粉红
        clrLightMagenta =  0xfc54fc,  // 亮品 
        clrMediumVioletRed=0xc71585,  // 中紫罗兰红 
        clrOrchid       =  0xda70d6,  // 暗紫，兰花紫 
        clrThistle      =  0xd8b0d8,  // 蓟色 
        clrPlum         =  0xdda0dd,  // 洋李色，李子紫 
        clrMagenta      =  0xa800a8,  // 洋红，品红，玫瑰红
        clrLightRed     =  0xfc5454,  // 浅红
        clrViolet       =  0xee82ee,  // 紫罗兰 
        
        clrFuchsia      =  0xff00ff,  // 紫红，灯笼海棠
        clrDarkMagenta  =  0x8b008b,  // 深洋红 
        clrPurple       =  0x800080,  // 紫色 
        clrMediumOrchid =  0xba55d3,  // 中兰花紫 
        clrDarkViolet   =  0x8b008b,  // 暗紫罗兰 
        clrDarkOrchid   =  0x9932cc,  // 暗兰花紫
        clrIndigo       =  0x4b0082,  // 靛青，紫兰色
        clrBlueViolet   =  0x8a2be2,  // 蓝紫罗兰
        clrMediumPurple =  0x9370db,  // 中紫色
        clrMediumSlateBlue=0x7b68ee,  // 中暗蓝色，中板岩蓝
        clrSlateBlue    =  0x6a5acd,  // 石蓝色，板岩蓝
        clrDarkSlateBlue=  0x483d8b,  // 暗灰蓝色，暗板岩蓝
        clrLavender     =  0xe6e6fa,  // 淡紫色，薰衣草淡紫
        clrGhostWhite   =  0xf8f8ff,  // 幽灵白
        clrMediumBlue   =  0x0000cd,  // 中蓝色
        clrBlue         =  0x0000a8,  // 蓝色
        
        clrMidnightBlue =  0x191970,  // 午夜蓝
        clrDarkBlue     =  0x00008b,  // 暗蓝色
        clrNavy         =  0x000080,  // 海军蓝
        clrRoyalBlue    =  0x4169e1,  // 宝蓝
        clrCornflowerBlue= 0x6495ed,  // 矢车菊蓝
        clrLightSteelBlue= 0xb0c4de,  // 亮钢蓝
        clrLightSlateBlue= 0x778899,  // 亮蓝灰，亮石板灰
        clrSlateGray    =  0x708090,  // 灰石色，石板灰
        clrDodgerBlue   =  0x1e90ff,  // 闪兰色，道奇蓝
        clrAliceBlue    =  0x0000cd,  // 爱丽丝蓝
        clrSteelBlue    =  0x4682b4,  // 钢蓝，铁青
        clrLightSkyBlue =  0x87cefa,  // 亮天蓝色
        clrSkyBlue      =  0x87ceeb,  // 天蓝色
        clrDeepSkyBlue  =  0x00bfff,  // 深天蓝
        clrLightBlue    =  0xadd8e6,  // 亮蓝
        clrPowderBlue   =  0xb0e0e6,  // 粉蓝色，火药青
         
        clrCadetBlue    =  0x5f9ea0,  // 军兰色，军服蓝
        clrAzure        =  0xf0ffff,  // 蔚蓝色
        clrLightCyan    =  0xe0ffff,  // 淡青色
        clrPaleTurquoise=  0xafeeee,  // 弱绿宝石
        clrCyan         =  0x00ffff,  // 青色
        clrAqua         =  0x00ffff,  // 浅绿色，水色
        clrDarkTurquoise=  0x00ced1,  // 暗绿宝石
        clrDarkSlateGray=  0x2f4f4f,  // 暗瓦灰色，暗石板灰
        clrDarkCyan     =  0x008b8b,  // 暗青色
        clrTeal         =  0x008080,  // 水鸭色
        clrMediumTurquoise=0x48d1cc,  // 中绿宝石
        clrLightSeaGreen=  0x20b2aa,  // 浅海洋绿
        clrTurquoise    =  0x40e0d0,  // 绿宝石
        clrAquamarine   =  0x7fffd4,  // 宝石碧绿
        clrMediumAquamarine=0x66cdaa, // 中宝石碧绿
        clrMediumSpringGreen=0x00fa9a,// 中春绿色
        
        clrMintCream    =  0xf5fffa,  // 薄荷奶油
        clrSpringGreen  =  0x00ff7f,  // 春绿色
        clrMediumSeaGreen= 0x3cb371,  // 中海洋绿
        clrSeaGreen     =  0x2e8b57,  // 海洋绿
        clrHoneydew     =  0xf0fff0,  // 蜜色，蜜瓜色
        clrLightGreen   =  0x90ee90,  // 淡绿色
        clrPaleGreen    =  0x98fb98,  // 弱绿色
        clrDarkSeaGreen =  0x8fbc8f,  // 暗海洋绿
        clrLimeGreen    =  0x32cd32,  // 闪光深绿
        clrLime         =  0x00ff00,  // 闪光绿
        clrForestGreen  =  0x228b22,  // 森林绿
        clrGreen        =  0x008000,  // 纯绿
        clrDarkGreen    =  0x006400,  // 暗绿色
        clrChartreuse   =  0x7fff00,  // 黄绿色，查特酒绿
        clrLawnGreen    =  0x7cfc00,  // 草绿色，草坪绿
        clrGreenYellow  =  0xadff2f,  // 绿黄色
        
        clrDarkOliveGreen= 0x556b2f,  // 暗橄榄绿
        clrYellowGreen  =  0x9acd32,  // 黄绿色
        clrOliveDrab    =  0x6b8e23,  // 橄榄褐色
        clrBeige        =  0xf5f5dc,  // 米色，灰棕色
        clrLightGoldenrodYellow=0xfafad2,// 亮菊黄
        clrIvory        =  0xfffff0,  // 象牙色
        clrLightYellow  =  0xffffe0,  // 浅黄色
        clrYellow       =  0xffff00,  // 纯黄
        clrOlive        =  0x808000,  // 橄榄
        clrDarkKhaki    =  0xbdb76b,  // 暗黄褐色，深卡叽布
        clrLemonChiffon =  0xfffacd,  // 柠檬绸
        clrPaleGoldenrod=  0xeee8aa,  // 灰菊黄，苍麒麟色
        clrKhaki        =  0xf0e68c,  // 黄褐色，卡叽布
        clrGold         =  0xffd700,  // 金色
        clrCornsilk     =  0xfff8dc,  // 玉米丝色
        clrGoldenrod    =  0xdaa520,  // 金菊黄
        
        clrDarkGoldenrod=  0xb8860b,  // 暗金菊黄
        clrfloralWhite  =  0xfffaf0,  // 花的白色
        clrOldLace      =  0xfdf5e6,  // 老花色，旧蕾丝
        clrWheat        =  0xf5deb3,  // 浅黄色，小麦色
        clrMoccasin     =  0xffe4b5,  // 鹿皮色，鹿皮靴
        clrOrange       =  0xffa500,  // 橙色
        clrPapayaWhip   =  0xffefd5,  // 番木色，番木瓜
        clrBlanchedAlmond= 0xffebcd,  // 白杏色
        clrNavajoWhite  =  0xffdead,  // 纳瓦白，土著白
        clrAntiqueWhite =  0xfaebd7,  // 古董白
        clrTan          =  0xd2b48c,  // 茶色
        clrBurlyWood    =  0xdeb887,  // 硬木色
        clrBisque       =  0xffe4c4,  // 陶坯黄
        clrDarkOrange   =  0xff8c00,  // 深橙色
        clrLinen        =  0xfaf0e6,  // 亚麻布
        clrPeru         =  0xcd853f,  // 秘鲁色
        
        clrPeachPuff    =  0xffdab9,  // 桃肉色
        clrSandyBrown   =  0xf4a460,  // 沙棕色
        clrChocolate    =  0xd2691e,  // 巧克力色
        clrSaddleBrown  =  0x8b4513,  // 重褐色，马鞍棕色
        clrSeashell     =  0xfff5ee,  // 海贝壳
        clrSienna       =  0xa0522d,  // 黄土赭色
        clrLightSalmon  =  0xffa07a,  // 浅鲑鱼肉色
        clrCoral        =  0xff7f50,  // 珊瑚
        clrOrangeRed    =  0xff4500,  // 橙红色
        clrDarkSalmon   =  0xe9967a,  // 深鲜肉，鲑鱼色
        clrTomato       =  0xff6347,  // 番茄红
        clrMistyRose    =  0xffe4e1,  // 浅玫瑰色，薄雾玫瑰
        clrSalmon       =  0xfa8072,  // 鲜肉，鲑鱼色
        clrSnow         =  0xfffafa,  // 雪白色
        clrLightCoral   =  0xf08080,  // 淡珊瑚色
        clrRosyBrown    =  0xbc8f8f,  // 玫瑰棕色
        
        clrIndianRed    =  0xcd5c5c,  // 印度红
        clrRed          =  0xff0000,  // 纯红
        clrBrown        =  0xa52a2a,  // 棕色
        clrFireBrick    =  0xb22222,  // 火砖色，耐火砖
        clrDarkRed      =  0x8b0000,  // 深红色
        clrMaroon       =  0x800000,  // 栗色
        clrWhite        =  0xffffff,  // 纯白
        clrWhiteSmoke   =  0xf5f5f5,  // 白烟
        clrGainsboro    =  0xdcdcdc,  // 淡灰色
        clrLightGray    =  0xa8a8a8,  // 浅灰
        clrLightGrey    =  0xd3d3d3,  // 浅灰色
        clrSilver       =  0xc0c0c0,  // 银灰色
        clrDarkGray     =  0xa9a9a9,  // 深灰色
        clrGray         =  0x808080,  // 灰色
        clrDimGray      =  0x696969,  // 暗淡灰
        clrBlack        =  0x000000,  // 纯黑
        
        clrTransparent  =  0xff000000 // 无色

    };
    
    // Synthesize colors with red, green, and blue components.
    constexpr color_t
    makrgb (colorcv_t byRed, colorcv_t byGreen,
           colorcv_t byBlue, colorcv_t byAlpha = 0) {
        return ( static_cast<color_t>(byAlpha) << 24)
            |  ( static_cast<color_t>(byRed)   << 16)
            |  ( static_cast<color_t>(byGreen) << 8 )
            |  ( static_cast<color_t>(byBlue)       );
    }

    // Synthesize colors with hue, saturation, lightness, alpha components.
    constexpr color_t
    makhsl (colorcv_t byHue,   colorcv_t bySaturation,
        colorcv_t byLightness, colorcv_t byAlpha = 0) {
        return makrgb (byHue, bySaturation, byLightness, byAlpha);
    }

    // Synthesize colors with hue, saturation, value, alpha components.
    constexpr color_t
    makhsv (colorcv_t byHue,   colorcv_t bySaturation,
          colorcv_t byValue,   colorcv_t byAlpha = 0) {
        return makrgb (byHue, bySaturation, byValue,     byAlpha);
    }

    // Synthesize colors with lightness components.
    constexpr color_t
    makgray (colorcv_t byLight, colorcv_t byAlpha = 0) {
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

#endif  // MCL_COLORS
