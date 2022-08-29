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

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/mclib.h"
#include "../src/clog4m.h"
#include "mcl_control.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#include <exception>

namespace mcl
{
    
    mcl_base_t mcl_base_obj;
    
    [[noreturn]] static void
    mcl_term_func () noexcept{
        clog4m[cll4m.Fatal].println("::abort() was called.");
        clog4m.uninit (-1);
        ::abort ();
    }
    void
    mcl_quick_exit_func () noexcept{
        clog4m[cll4m.Info].println("::quick_exit() was called.");
        clog4m.uninit (-2);
    }

    struct mcl_do_is_atquickexit_exist {
        template <typename T, typename = decltype(at_quick_exit(*static_cast<T*>(0)))>
            static std::integral_constant<bool, true> test(int); // only declaration
        template <typename>
            static std::integral_constant<bool, false> test(...);
    };
    template<typename T>
    struct mcl_is_atquickexit_exist
        : public mcl_do_is_atquickexit_exist {
        typedef decltype(test<T>(0)) type;
    };
    template <typename Ret>
    inline typename std::enable_if<mcl_is_atquickexit_exist<Ret()>::type::value, void>::type
        mcl_atquickexit(Ret(*fun)() noexcept) { at_quick_exit(fun); }
    template <typename Ret>
    inline typename std::enable_if<!mcl_is_atquickexit_exist<Ret()>::type::value, void>::type
        mcl_atquickexit(Ret(*)() noexcept) { }

   /**
    * @function mcl_base_t::mcl_base_t <cpp/mcl_base.h>
    * @brief Constructor.
    * @return nothing
    */
    mcl_base_t::mcl_base_t () noexcept {
        std::set_terminate (mcl_term_func);
        ::atexit (quit);
        mcl_atquickexit (mcl_quick_exit_func);
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
                            "{\"own\":\"0x%04lx\", \"cur\":\"0x%04lx\"}", lk_, threadid);
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
        
        void mcl_lock (mcl_spinlock_t::lock_t& lk_, unsigned& m_nrt_count) noexcept {
            unsigned long threadid = ::GetCurrentThreadId ();
            if (!::InterlockedCompareExchange (&lk_, threadid, 0))
                return ;
            if (lk_ == threadid) {
                clog4m[cll4m.Debug]. putws (
                   L"\n  mcl::spinlock  | "
                    "Same thread acquires the same spinlock twice. ");
                if (m_nrt_count == 0xffffffff) {
                    clog4m[cll4m.Warn].putws(
                        L"\n  mcl::spinlock  | "
                        "Repeat lock exceed the maximum depth. ");
                    return ;
                }
                ++ m_nrt_count;
                return ; 
            }
            while (::InterlockedCompareExchange (&lk_, threadid, 0)); 
        }
        void mcl_unlock (mcl_spinlock_t::lock_t& lk_, unsigned& m_nrt_count) noexcept{
            unsigned long threadid = ::GetCurrentThreadId();
            if (lk_ == threadid) {
                if (m_nrt_count) -- m_nrt_count;
                else ::InterlockedExchange (&lk_, 0);
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
