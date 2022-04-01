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

// no define

namespace
mcl {

   /**
    * @class mcl_mouse_t
    * @brief Module to work with the mouse
    * 
    * @ingroup event
    * @ingroup mouse
    * @ingroup mclib
    * @{
    */
    class
    mcl_mouse_t { public:
        explicit      mcl_mouse_t ()                       = default;
                     ~mcl_mouse_t ()                       = default;
                      mcl_mouse_t (mcl_mouse_t const& rhs) = delete;
        mcl_mouse_t&  operator=   (mcl_mouse_t const& rhs) = delete;
        operator      void*       () const noexcept;
        bool          operator!   () const noexcept;
        
    };
   /** @}  */


  /**
   * @name Mouse State Objects
   * @{
   */
    extern mcl_mouse_t mouse;
    
/** @}  */ 


} // namespace

#endif // MCL_MOUSE
