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
    
    @file cpp/timer.cpp
    This is a C++11 implementation file for monitoring time.
*/


#include "../src/timer.h"
#include "mcl_control.h"

namespace
mcl {
    
    // Module for monitoring time.
    mcl_time_t time;
    
    mcl_time_t::operator void* () const
        noexcept{ return const_cast<mcl_time_t*>(this); } 
    
    bool mcl_time_t::operator! () const
        noexcept{ return false; } 

   /**
    * @function mcl_time_t::get_ticks <src/timer.h> 
    * @brief Get the time in milliseconds / microseconds.
    * @return int64_t:
    *     the number of milliseconds / microseconds
    *     since display.init() was called.
    */
    long long mcl_time_t::
    get_ticks () noexcept{
        if (!mcl_control_obj.time) return 0ll;
        LARGE_INTEGER stopCount;
        ::QueryPerformanceCounter (&stopCount);
        stopCount.QuadPart =
            (stopCount.QuadPart - mcl_control_obj.time) * 1000ll / mcl_base_obj.frequency;
        return mcl_control_obj.time ? stopCount.QuadPart : 0ll;
    }
    long long mcl_time_t::
    get_ticks_us () noexcept{
        if (!mcl_control_obj.time) return 0ll;
        LARGE_INTEGER stopCount;
        ::QueryPerformanceCounter (&stopCount);
        stopCount.QuadPart =
            (stopCount.QuadPart - mcl_control_obj.time) * 1000000ll / mcl_base_obj.frequency;
        return mcl_control_obj.time ? stopCount.QuadPart : 0ll;
    }


   /**
    * @function mcl_time_t::wait <src/timer.h> 
    * @brief Put the program into sleep and make it
    *     inactive for a period of time.
    * @param {int32_t} millseconds: hang time
    * @return int32_t: the actual number of milliseconds used.
    */
    long long mcl_time_t::
    wait (long millseconds) noexcept{
        if (millseconds < 0) return 0;
        LARGE_INTEGER startCount, stopCount;
        ::QueryPerformanceCounter (&startCount);
        ::Sleep (static_cast<DWORD>(millseconds));
        ::QueryPerformanceCounter (&stopCount);
        return (stopCount.QuadPart - startCount.QuadPart) * 1000ll / mcl_base_obj.frequency;
    }
    
   /**
    * @function mcl_time_t::delay <src/timer.h> 
    * @brief Wait exactly for a period of time
    *     may consume high CPU
    * @param {int32_t} millseconds, microseconds: wait time
    * @return int64_t: the actual number of microseconds used.
    */
    long long mcl_time_t::
    delay (long millseconds, long microseconds) noexcept{
        LONGLONG const lastus = millseconds * 1000ll + microseconds;
        if (lastus <= 0ll) return 0;
        LARGE_INTEGER startCount, stopCount;
        ::QueryPerformanceCounter (&startCount);
        LONGLONG elapsed = 0ll;
        if (lastus >= 262144ll) {
            elapsed = lastus / 1000ll - 256ll;
            ::Sleep (static_cast<DWORD>(elapsed));
        }
        do {
            ::QueryPerformanceCounter (&stopCount);
            elapsed = (stopCount.QuadPart - startCount.QuadPart) * 1000000ll / mcl_base_obj.frequency;
        } while (elapsed < lastus);
        return elapsed;
    }

} // namespace    
