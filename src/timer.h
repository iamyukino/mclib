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

namespace
mcl {

   /**
    * @unimplemented
    *     none
    */

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

    //      get_ticks() -> time_milliseconds
    //      get_ticks_us() -> time_microseconds
    //      Return the number of milliseconds / microseconds since
    //      mcl::display.init() was called. Before display is initialized
    //      this will always be 0. 

        // Sleep for an amount of millseconds
        long long wait (long millseconds) noexcept;

    //      wait(millseconds) -> time_milliseconds
    //      Put the program into sleep and make it inactive for a given
    //      number of milliseconds.

        // Pause the program for an amount of time.
        long long delay (long millseconds, long microsecond = 0) noexcept;

    //      delay(milliseconds, microseconds) -> time_microseconds
    //      Pause for a sum of given numbers of milliseconds and microseconds.
    //      This function will use the processor (rather than sleeping) in order
    //      to make the delay more accurate than mcl::timer.wait()

        // Repeatedly create an event on the event queue.
        bool set_timer (event_t type, long millseconds, int loops = 0) noexcept;
        // Repeatedly create an event on the event queue.
        bool set_timer (eventtype_t type, long millseconds, int loops = 0) noexcept;

    //      set_timer(type, code, millseconds, int)
    //      Set an event to appear on the event queue every given number
    //      of milliseconds. The first event will not appear until the amount
    //      of time has passed.
    
    };
    extern mcl_time_t timer; // Module for monitoring time.

   /**
    * @class tclock_t <src/timer.h>
    * @brief object to help track time
    * 
    * @ingroup time
    * @ingroup mclib
    * @ingroup display
    */
    class
    tclock_t {
    public:
        enum : char { mcl_averaging_max = 10 };

        explicit    tclock_t  () noexcept;
                   ~tclock_t  () noexcept;
                    tclock_t  (tclock_t const& rhs) = delete;
        tclock_t&   operator= (tclock_t const& rhs) = delete;
        operator    void*     () const noexcept;
        bool        operator! () const noexcept; 

        // Update the clock.
        long long   tick      (float framerate = 0.f) noexcept;
        // Update the clock.
        long long   tick_busy_loop (float framerate = 0.f) noexcept;
        // Time used in the previous tick.  Accurate to milliseconds.
        long long   get_time  () noexcept;
        // Time used in the previous tick.  Accurate to mircoseconds.
        long long   get_time_us () noexcept;
        // Actual time used in the previous tick.  Accurate to milliseconds.
        long long   get_rawtime () noexcept;
        // Actual time used in the previous tick.  Accurate to mircoseconds.
        long long   get_rawtime_us () noexcept;
        // Compute the clock framerate.
        float       get_fps   () noexcept;

    private:
        void* m_dataplus_;
        char m_data_[1];

        char : 8; char : 8; char : 8; char : 8;
        char : 8; char : 8; char : 8;
    };

} // namespace

#endif // MCL_TIME
