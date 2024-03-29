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

    @file src/surface.h
    This is a C++11 header for representing any image.
*/


#ifndef MCL_SURFACE
# define MCL_SURFACE

# include "mclfwd.h"
# include "bufferproxy.h"

namespace
mcl {

   /**
    * @unimplemented
    *     pygame.Surface.scroll()
    *     pygame.Surface.subsurface()
    *     pygame.Surface.get_parent()
    *     pygame.Surface.get_abs_parent()
    *     pygame.Surface.get_offset()
    *     pygame.Surface.get_abs_offset()
    *     pygame.Surface.set_clip()
    *     pygame.Surface.get_clip()
    *     
    *     pygame.Surface.convert()
    *       # premult_alpha
    *     pygame.Surface.get_at_mapped()
    *     pygame.Surface.get_palette()
    *     pygame.Surface.get_palette_at()
    *     pygame.Surface.set_palette()
    *     pygame.Surface.set_palette_at()
    *     pygame.Surface.map_rgb()
    *     pygame.Surface.unmap_rgb()
    *     pygame.Surface.get_bitsize()
    *     pygame.Surface.get_bytesize()
    *     
    *     pygame.Surface.mustlock()
    *     pygame.Surface.get_locks()
    *     pygame.Surface.get_view()
    *     pygame.Surface.blits()
    * 
    * @unfinished
    *     pygame.Surface()
    */

   /**
    * @enum class mcl_dflags_t <src/display.h>
    * @brief Flags that controls which type of display you want.
    * 
    * @ingroup images
    * @ingroup mclib
    */
    class
    mcl_blend_t {
    
    public:
        using type = unsigned long;

        static type constexpr Copy  = 0x0; // normal copy.
        static type constexpr Add   = 0x1; // color dodge.
        static type constexpr Sub   = 0x2; // color burn.
        static type constexpr Mult  = 0x3; // multiply.
        static type constexpr Min   = 0x4; // darken.
        static type constexpr Max   = 0x5; // lighten.
        static type constexpr Xor   = 0x6; // normal xor.
        static type constexpr Alpha = 0x7; // overlay.

        static type constexpr PreMultiplied = 0x10; // src has premultiplied Alpha

        static type constexpr Copy_rgb  = 0x100; // normal copy. ignore alpha.
        static type constexpr Add_rgb   = 0x101; // color dodge. ignore alpha.
        static type constexpr Sub_rgb   = 0x102; // color burn. ignore alpha.
        static type constexpr Mult_rgb  = 0x103; // multiply. ignore alpha.
        static type constexpr Min_rgb   = 0x104; // darken. ignore alpha.
        static type constexpr Max_rgb   = 0x105; // lighten. ignore alpha.
        static type constexpr Xor_rgb   = 0x106; // normal xor. ignore alpha.
        static type constexpr Alpha_rgb = 0x107; // overlay. ignore ignore alpha.

        static type constexpr Copy_rgba  = 0x200; // normal copy. alpha involved.
        static type constexpr Add_rgba   = 0x201; // color dodge. alpha involved.
        static type constexpr Sub_rgba   = 0x202; // color burn. alpha involved.
        static type constexpr Mult_rgba  = 0x203; // multiply. alpha involved.
        static type constexpr Min_rgba   = 0x204; // darken. alpha involved.
        static type constexpr Max_rgba   = 0x205; // lighten. alpha involved.
        static type constexpr Xor_rgba   = 0x206; // normal xor. alpha involved.
        static type constexpr Alpha_rgba = 0x207; // overlay. alpha involved.

    };
    using blend_t = mcl_blend_t::type;
    extern mcl_blend_t blend;
    
    /**
     * @class surface_t
     * @brief Class to represent image.
     *
     * @ingroup surface
     * @ingroup images
     * @ingroup mclib
     */
    class
    surface_t {
    public:
        using type = char;
        static type constexpr SwSurface   = 0x0;
        static type constexpr SrcAlpha    = 0x1;
        static type constexpr SrcColorKey = 0x2;

    public:
        explicit   surface_t (void* = 0, type special_flags = 0) noexcept;
        explicit   surface_t (point2d_t size, type special_flags = 0) noexcept;
                  ~surface_t () noexcept;

                   surface_t (surface_t const& rhs) noexcept;
                   surface_t (surface_t const& rhs, type special_flags) noexcept;
                   surface_t (surface_t&& rhs) noexcept;

        surface_t& operator= (surface_t const& rhs) noexcept;
        surface_t& operator= (surface_t&& rhs) noexcept;

        operator   void*     () const noexcept;
        bool       operator! () const noexcept;
        
        // draw one image onto another
        rect_t     blit      (surface_t const& source, void* = 0, void* = 0, blend_t special_flags = 0) noexcept;
        // draw one image onto another
        rect_t     blit      (surface_t const& source, point2d_t dest, void* = 0, blend_t special_flags = 0) noexcept;
        // draw one image onto another
        rect_t     blit      (surface_t const& source, void*, rect_t area, blend_t special_flags = 0) noexcept;
        // draw one image onto another
        rect_t     blit      (surface_t const& source, point2d_t dest, rect_t area, blend_t special_flags = 0) noexcept;
        // change the pixel format of an image including per pixel alphas
        surface_t  convert_alpha () const noexcept;
        // change the pixel format of an image including per pixel alphas
        surface_t  convert_alpha (bool b_srcalpha) const noexcept;
        // change the pixel format of an image including per pixel alphas
        surface_t  convert_alpha (surface_t const& source) const noexcept;
        // create a new copy of a Surface
        surface_t  copy      () const noexcept;
        // resize the surface. see also transform.clip
        point2d_t  resize    (point2d_t size, bool b_fast = false) noexcept;
        // fill surface_t with a solid color
        rect_t     fill      (color_t color, void* = 0, blend_t special_flags = 0) noexcept;
        // fill surface_t with a solid color
        rect_t     fill      (color_t color, rect_t recta, blend_t special_flags = 0) noexcept;
        // Set the transparent colorkey
        void       set_colorkey () noexcept;
        // Set the transparent colorkey
        void       set_colorkey (color_t colorkey) noexcept;
        // Get the current transparent colorkey
        bool       get_colorkey (color_t* pcolorkey) const noexcept;
        // Set the alpha value for the full surface image
        void       set_alpha () noexcept;
        // Set the alpha value for the full surface image
        void       set_alpha (color_t alpha) noexcept;
        // Get the current surface transparency value
        color_t    get_alpha () const noexcept;
        // lock the Surface memory for pixel access
        surface_t& lock      () noexcept;
        // unlock the Surface memory for pixel access
        surface_t& unlock    () noexcept;
        // test if the Surface is current locked
        bool       get_locked() const noexcept;
        // get the color value at a single pixel
        color_t    get_at    (point2d_t pos) const noexcept;
        // set the color value at a single pixel
        color_t    set_at    (point2d_t pos, color_t color) noexcept;
        // get the dimensions of the Surface
        point2d_t  get_size  () const noexcept;
        // get the width of the Surface
        point1d_t  get_width () const noexcept;
        // get the height of the Surface
        point1d_t  get_height() const noexcept;
        // get the rectangular area of the Surface
        rect_t     get_rect  (void* = 0) const noexcept;
        // get the rectangular area of the Surface
        rect_t     get_rect  (point2d_t center) const noexcept;
        // get the additional flags used for the Surface
        type       get_flags () const noexcept;
        // find the smallest rect containing data
        rect_t     get_bounding_rect (color_t min_alpha = 1) const noexcept;
        // acquires a buffer object for the pixels of the surface_t.
        inline bufferproxy_t get_buffer () noexcept{ return bufferproxy_t (this); }
        // pixel buffer address
        color_t*   _pixels_address () noexcept;
        // returns a copy of the surface with the RGB channels pre-multiplied by the alpha channel
        surface_t  premul_alpha () const noexcept;
        
    private:
        void* m_dataplus_;
        char m_data_[1];

        char : 8; char : 8; char : 8; char : 8;
        char : 8; char : 8; char : 8;
    };

    extern surface_t sf_nullptr;

} // namespace

#endif // MCL_SURFACE
