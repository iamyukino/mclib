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
    
    @file src/timer.h
    This is a C++11 header for monitoring time.
*/


#ifndef MCL_TIME
# define MCL_TIME

# include "mclfwd.h"

// no define

namespace
mcl {

   /**
    * @class mcl_time_t <src/timer.h>
    * @brief module for monitoring time
    * 
    * @ingroup time
    * @ingroup mclib
    * @ingroup display
    * @{
    */
    class
    mcl_time_t { public:
        
        explicit       mcl_time_t ()                      = default;
                      ~mcl_time_t ()                      = default;
                       mcl_time_t (mcl_time_t const& rhs) = delete;
        mcl_time_t&    operator=  (mcl_time_t const& rhs) = delete;
        operator       void*      () const noexcept;
        bool           operator!  () const noexcept; 
        
        // Get the time in milliseconds
        long long get_ticks    () noexcept; 
        // Get the time in microseconds
        long long get_ticks_us () noexcept; 

    //    mcl::timer.get_ticks()
    //    mcl::timer.get_ticks_us()
    //      get the time in milliseconds / microseconds
    //      get_ticks() -> time_milliseconds
    //      get_ticks_us() -> time_microseconds
    //
    //      Return the number of milliseconds / microseconds since
    //      mcl::display.init() was called. Before display is initialized
    //      this will always be 0. 

        // Sleep for an amount of millseconds
        long long wait (long millseconds) noexcept;

    //    mcl::timer.wait()
    //      pause the program for an amount of time
    //      wait(millseconds) -> time_milliseconds
    //
    //      Put the program into sleep and make it inactive for a given number
    //      of milliseconds.
    //
    //      This returns the actual number of milliseconds used.

        // Pause the program for an amount of time.
        long long delay (long millseconds, long microsecond = 0) noexcept;

    //    mcl::timer.delay()
    //      pause the program for an amount of time
    //      delay(milliseconds) -> time_microseconds
    //      delay(milliseconds, microseconds) -> time_microseconds
    // 
    //      Will pause for a sum of given numbers of milliseconds and microseconds.
    //      This function will use the processor (rather than sleeping) in order
    //      to make the delay more accurate than mcl::timer.wait()
    //
    //      This returns the actual number of microseconds used.
    
    };
    extern mcl_time_t timer; // Module for monitoring time.

} // namespace

#endif // MCL_TIME
