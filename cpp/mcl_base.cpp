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
    
    @file cpp/mcl_base.cpp
    This is a C++11 implementation file for useful tools.
*/


#include "../src/mclib.h"
#include "../src/clog4m.h"
#include "mcl_control.h"

namespace mcl
{
    
    mcl_base_t mcl_base_obj;
    
   /**
    * @function mcl_base_t::mcl_base_t <cpp/mcl_base.h>
    * @brief Constructor.
    * @return nothing
    */
    mcl_base_t::mcl_base_t () noexcept {
        atexit (quit);
        // Returns the number of performance counter ticks per second
        LARGE_INTEGER tickCount;
        ::QueryPerformanceFrequency (&tickCount);
        frequency = tickCount.QuadPart;
    }
    
    
    namespace mcl_simpletls_ns
    {
    
       /**
        * @class mcl_spinlock_t <cpp/mcl_base.h>
        * @brief spinlock
        */
        mcl_spinlock_t::
        mcl_spinlock_t (lock_t& lk, unsigned long uWaitMs) noexcept
          : lk_ (lk), islock_ (true) {
            unsigned long threadid = ::GetCurrentThreadId ();
            if (!::InterlockedCompareExchange (&lk_, threadid, 0))
                return ;
            if (lk_ == threadid) {
                clog4m[cll4m.Debug]. putws (
                   L"\n  mcl::spinlock  | "
                    "Same thread acquires the same spinlock twice. ");
                return ; 
            }
            if (uWaitMs != 0ul) {
                ULONGLONG oldt, nowt;
                oldt = nowt = MCL_GETTICKCOUNT();
                while (::InterlockedCompareExchange (&lk_, threadid, 0)) {
                    if (nowt < oldt) nowt = oldt;
                    if (nowt - oldt > uWaitMs) {
                        oldt = nowt;
                        clog4m[cll4m.Warn]. wprintln (
                           L"\n  mcl::spinlock  | TimeOut "
                            "{\"own\":\"%lux\", \"cur\":\"%lux\"}", lk_, threadid);
                        ::InterlockedExchange (&lk_, 0);
                    }  nowt = MCL_GETTICKCOUNT ();
                }
            } else
                while (::InterlockedCompareExchange (&lk_, threadid, 0)); 
        }
        mcl_spinlock_t::~mcl_spinlock_t () noexcept{
            if (islock_) {
                ::InterlockedExchange (&lk_, 0);
                islock_ = false;
            }
        }
        
        
       /**
        * @class mcl_m2w_str_t <cpp/mcl_base.h>
        * @brief cast from string to wstring
        */
        mcl_m2w_str_t::mcl_m2w_str_t (char const* begin, char const* end) noexcept
        : len_ (static_cast<size_t>(::MultiByteToWideChar(
            CP_ACP, 0, begin,
            end ? static_cast<int>(end - begin) : -1,
            nullptr, 0
        ))), auto_ptr_ (len_ + 1u) {
            if (!auto_ptr_) { len_ = 0; return ; }
            ::MultiByteToWideChar ( CP_ACP, 0, begin,
                end ? static_cast<int>(end - begin) : -1, auto_ptr_,
                static_cast<int>(len_));
        }
        
        
    }
}
