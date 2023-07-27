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
    
    @file src/cursors.h
    This is a C++11 header for cursor resources.
*/


#ifndef MCL_CURSORS
# define MCL_CURSORS

# include "mclfwd.h"
# include "pyobj.h"

# include <vector>

namespace
mcl {
    
   /**
    * @unimplemented
    *     pygame.cursors.load_xbm()
    *     pygame.cursors.Cursor.data()
    */

   /**
    * @class mcl_cursors_t <src/cursors.h>
    * @brief module for cursor resources
    * 
    * @ingroup cursors
    * @ingroup mouse
    * @ingroup mclib
    */
    class
    mcl_cursors_t { public:
        explicit       mcl_cursors_t ()                         = default;
                      ~mcl_cursors_t ()                         = default;
                       mcl_cursors_t (mcl_cursors_t const& rhs) = delete;
        mcl_cursors_t& operator=   (mcl_cursors_t const& rhs) = delete;

    public:
        using type = sys_cursor_t;
        
        static type constexpr Arrow     = 0;
        static type constexpr Ibeam     = 1;
        static type constexpr Wait      = 2;
        static type constexpr CrossHair = 3;
        static type constexpr WaitArrow = 4;

        static type constexpr SizeNWSE  = 5;
        static type constexpr SizeNESW  = 6;
        static type constexpr SizeWE    = 7;
        static type constexpr SizeNS    = 8;
        static type constexpr SizeAll   = 9;

        static type constexpr No        = 10;
        static type constexpr Hand      = 11;
        static type constexpr UpArrow   = 12;
        static type constexpr Help      = 13;
        static type constexpr Pin       = 14;
        static type constexpr Person    = 15;
    
    public:
        operator       void*       () const noexcept;
        bool           operator!   () const noexcept;
        
        // create binary cursor data from simple strings
        pytuple< std::vector<unsigned char>, std::vector<unsigned char> >
            compile (char const* strings, char cblack = 'X', char cwhite = '.', char cxor = 'o') noexcept;

    };
    extern mcl_cursors_t cursors; // Module for cursor resources.

   /**
    * @class cursor_t <src/cursors.h>
    * @brief type for representing a cursor
    * 
    * @ingroup cursors
    * @ingroup mouse
    * @ingroup mclib
    */
    class
    cursor_t {
    public:
                       cursor_t (cursor_t const& rhs) noexcept;
                       cursor_t (cursor_t&& rhs) noexcept;
                      ~cursor_t () noexcept;
        operator       void*    () const noexcept;
        bool           operator!() const noexcept;
        cursor_t&      operator=(cursor_t const& rhs) noexcept;
        cursor_t&      operator=(cursor_t&& rhs) noexcept;
        cursor_t&      operator=(decltype(nullptr)) noexcept;

        // Gets the cursor type
        wchar_t const* type     () noexcept;
        // Gets the cursor type
        char const*    type_a   () noexcept;

        // set_cursor example:
        //   auto cursor = mcl::cursors.compile(thickarrow_strings);
        //   mouse.set_cursor({24, 24}, {0, 0}, *cursor);

        // Default constructor
        explicit       cursor_t () noexcept;
        // Use system cursor.  See cursors.h .
        explicit       cursor_t (sys_cursor_t constant) noexcept;
        // Use color cursor.  All sizes and rgba colors are supported.
        explicit       cursor_t (point2d_t hotspot, surface_t surface) noexcept;
        // Use Windows binary cursor. (returned by mcl::cursors.compile)
        // Width and height must be equal and be a multiple of 8
        explicit       cursor_t (point2d_t size, point2d_t hotspot,
            pytuple< std::vector<unsigned char>, std::vector<unsigned char> >&&
            xor_and_masks) noexcept;

        // Use Windows binary cursor.
        // Width and height must be equal and be a multiple of 8.
        // |  and xor  resultc   |
        // |   0   0    black    |
        // |   0   1    white    |
        // |   1   0  background |
        // |   1   1     xor     |
        explicit       cursor_t (point2d_t size, point2d_t hotspot,
            unsigned char const* xormasks, unsigned char const* andmasks) noexcept;
        
    private:
        void* m_dataplus_;
        char m_data_[1];
        
        char : 8; char : 8; char : 8; char : 8;
        char : 8; char : 8; char : 8;
    };

} // namespace

#endif // MCL_CURSORS
