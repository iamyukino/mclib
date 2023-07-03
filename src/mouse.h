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
    
    @file src/mouse.h
    This is a C++11 header for working with the mouse.
*/


#ifndef MCL_MOUSE
# define MCL_MOUSE

# include "mclfwd.h"
# include "pyobj.h"
# include <vector>

namespace
mcl {
    
   /**
    * @class mcl_mouse_t <src/mouse.h>
    * @brief module for interacting with the mouse
    * 
    * @ingroup mouse
    * @ingroup event
    * @ingroup mclib
    */
    class
    mcl_mouse_t { public:
        explicit       mcl_mouse_t ()                       = default;
                      ~mcl_mouse_t ()                       = default;
                       mcl_mouse_t (mcl_mouse_t const& rhs) = delete;
        mcl_mouse_t&   operator=   (mcl_mouse_t const& rhs) = delete;

    public:
        using btn_type = char;
 
        static btn_type constexpr BtnNone     =  0x0;
        static btn_type constexpr BtnLButton  =  0x1;
        static btn_type constexpr BtnRButton  =  0x2;
        static btn_type constexpr BtnShift    =  0x4;
        static btn_type constexpr BtnCtrl     =  0x8;
        static btn_type constexpr BtnMButton  = 0x10;
        static btn_type constexpr BtnXButton1 = 0x20;
        static btn_type constexpr BtnXButton2 = 0x40;

    public:
        operator       void*       () const noexcept;
        bool           operator!   () const noexcept;
        
        // get the state of the mouse buttons.
        std::vector<bool> get_pressed (int num_buttons = 3) noexcept;
        // get the state of the mouse buttons and modifier keys.
        btn_type          get_buttons () noexcept;

        // get the mouse cursor position
        point2d_t         get_pos     (bool b_global = false) noexcept;
        // set the mouse cursor position
        bool              set_pos     (point2d_t pos, bool b_global = false) noexcept;
        
        // get the visibility state of the mouse cursor
        bool              get_visible () noexcept;
        // set the visibility state of the mouse cursor
        bool              set_visible (bool b_visible) noexcept;

        // check if the display is receiving mouse input
        bool              get_focused () noexcept;

        // set the mouse cursor to a new cursor.  see cursors.h
        bool              set_cursor  () noexcept;
        // set the mouse cursor to a new cursor.  see cursors.h
        bool              set_cursor  (cursor_t cur) noexcept;
        // set the mouse cursor to a new cursor.  see cursors.h
        bool              set_cursor  (sys_cursor_t constant) noexcept;
        // set the mouse cursor to a new cursor.  see cursors.h
        bool              set_cursor  (point2d_t hotspot, surface_t surface) noexcept;
        // set the mouse cursor to a new cursor.  see cursors.h
        bool              set_cursor  (point2d_t size, point2d_t hotspot,
            pytuple< std::vector<unsigned char>, std::vector<unsigned char> >&&
            xor_and_masks) noexcept;
        // set the mouse cursor to a new cursor.  see cursors.h
        bool              set_cursor  (point2d_t size, point2d_t hotspot,
            unsigned char const* xormasks, unsigned char const* andmasks) noexcept;

        // get the current mouse cursor
        cursor_t          get_cursor  () noexcept;

    };
    extern mcl_mouse_t mouse; // Module for interacting with the mouse.

} // namespace

#endif // MCL_MOUSE
