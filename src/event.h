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
    
    @file src/event.h
    This is a C++11 header for interacting with events and queues.
*/


#ifndef MCL_EVENT
# define MCL_EVENT

# include "mclfwd.h"

// no define

namespace
mcl {

   /**
    * @class mcl_event_t
    * @brief Module for interacting with events and queues
    * 
    * @ingroup event
    * @ingroup display
    * @ingroup mclib
    * @{
    */
    class
    mcl_event_t { public:
        explicit      mcl_event_t ()                       = default;
                     ~mcl_event_t ()                       = default;
                      mcl_event_t (mcl_event_t const& rhs) = delete;
        mcl_event_t&  operator=   (mcl_event_t const& rhs) = delete;
        operator      void*       () const noexcept;
        bool          operator!   () const noexcept;
        
    };
   /** @}  */


  /**
   * @name Mclib Events Dealing Objects
   * @{
   */
    extern mcl_event_t event;
    
/** @}  */ 


} // namespace

#endif // MCL_EVENT
