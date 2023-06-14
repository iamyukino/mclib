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
    * @class mcl_cursors_t <src/cursors.h>
    * @brief module for cursor resources
    * 
    * @ingroup cursors
    * @ingroup mouse
    * @ingroup mclib
    * @{
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
        
        pytuple< std::vector<unsigned char>, std::vector<unsigned char> >
            compile (char const* strings, char cblack = 'X', char cwhite = '.', char cxor = 'o') noexcept;

    };
    extern mcl_cursors_t cursors; // Module for cursor resources.

    class
    cursor_t {
    public:
        explicit cursor_t  () noexcept;
        operator void*     () const noexcept;
        bool     operator! () const noexcept;

        wchar_t const* type () noexcept;
        char const* type_a () noexcept;
        
        // TODO: 
        // 1. implement reference counting to ensure that
        // each set_cursor will call DestroyCursor. 
        // 2. cursor_t objects can be created directly
        // by providing parameters as set_cursor. 
        // 3. load_xbm 

    private:
        void* m_dataplus_; // HCURSOR
        char m_data_[1];   // 1: "system", 2: "bitmap", 3: "color"
        
        friend class mcl_mouse_t;
        char : 8; char : 8; char : 8; char : 8;
        char : 8; char : 8; char : 8;
    };

} // namespace

#endif // MCL_CURSORS
