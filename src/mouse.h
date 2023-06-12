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

namespace
mcl {
    
   /**
    * @class mcl_mouse_t <src/mouse.h>
    * @brief module for interacting with the mouse
    * 
    * @ingroup mouse
    * @ingroup event
    * @ingroup mclib
    * @{
    */
    class
    mcl_mouse_t { public:
        explicit       mcl_mouse_t ()                       = default;
                      ~mcl_mouse_t ()                       = default;
                       mcl_mouse_t (mcl_mouse_t const& rhs) = delete;
        mcl_mouse_t&   operator=   (mcl_mouse_t const& rhs) = delete;

        pytuple<bool, bool, bool> mcl_get_pressed_3 () const noexcept;
        pytuple<bool, bool, bool, bool, bool> mcl_get_pressed_5 () const noexcept;

    public:
        operator       void*       () const noexcept;
        bool           operator!   () const noexcept;
        
        // get the state of the mouse buttons.  see event_t::mcl_mouse_event_t
        // example:  mcl::mouse.get_pressed<3>();
        template<int num_buttons = 3>
        constexpr typename std::enable_if< num_buttons == 3,
            pytuple<bool, bool, bool> >::type get_pressed ()
        noexcept{ return mcl_get_pressed_3 (); }
        
        template<int num_buttons = 3>
        constexpr typename std::enable_if< num_buttons == 5,
            pytuple<bool, bool, bool, bool, bool> >::type get_pressed()
        noexcept{ return mcl_get_pressed_5 (); }
        
        // get & set the mouse cursor position
        point2d_t      get_pos     (bool b_global = false) noexcept;
        bool           set_pos     (point2d_t pos, bool b_global = false) noexcept;
        
        // get & set the visibility state of the mouse cursor
        bool           get_visible () noexcept;
        bool           set_visible (bool b_visible) noexcept;

        // check if the display is receiving mouse input
        bool           get_focused () noexcept;

    };
    extern mcl_mouse_t mouse; // Module for interacting with the mouse.

} // namespace

#endif // MCL_MOUSE
