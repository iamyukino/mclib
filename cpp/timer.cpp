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

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/timer.h"
#include "mcl_control.h"

#include <map>

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {
    
    // Module for monitoring time.
    mcl_time_t timer;
    
    mcl_time_t::operator void* () const noexcept{
        return mcl_control_obj.timer ?
            const_cast<mcl_time_t*>(this) : 0;
    }
    
    bool mcl_time_t::operator! () const
        noexcept{ return !mcl_control_obj.timer; }

   /**
    * @function mcl_time_t::get_ticks <src/timer.h> 
    * @brief Get the time in milliseconds / microseconds.
    * @return int64_t:
    *     the number of milliseconds / microseconds
    *     since display.init() was called.
    */
    long long mcl_time_t::
    get_ticks () noexcept{
        return (get_ticks_us () + 500ll) / 1000ll;
    }
    long long mcl_time_t::
    get_ticks_us () noexcept{
        if (!mcl_control_obj.timer) return 0ll;
        LARGE_INTEGER stopCount;
        ::QueryPerformanceCounter (&stopCount);
        stopCount.QuadPart =
            (stopCount.QuadPart - mcl_control_obj.timer) * 1000000ll / mcl_base_obj.frequency;
        return mcl_control_obj.timer ? stopCount.QuadPart : 0ll;
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
        return ((stopCount.QuadPart - startCount.QuadPart) * 10000ll / mcl_base_obj.frequency + 5ll) / 10ll;
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

   /**
    * @function mcl_time_t::set_timer <src/timer.h> 
    * @brief Repeatedly create an event on the event queue.
    * @param {event_t, eventtype_t} event: denotes event
    * @param {long} millseconds: 0 if need to disable the timer for such an event
    * @param {int} loops: the number of events posted. keep getting posted if set to 0
    * @return bool: true if succeeded
    */
    bool mcl_time_t::
    set_timer (eventtype_t type, long millseconds, int loops) noexcept{
        return set_timer(event_t{ type, {{0, 0}} }, millseconds, loops);
    }
    bool mcl_time_t::
    set_timer (event_t type, long millseconds, int loops) noexcept{
        if (!type.type || millseconds < 0 || loops < 0)
            return false;
        
        UINT_PTR idevent = static_cast<UINT_PTR>(type.type);

        { // spinlock
            mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.nrtlock);
            if (!mcl_control_obj.bIsReady)
                return false;

            struct mcl_timermape_t { event_t event;
                int loops; char : 8; char : 8; char : 8; char : 8; };
            using mcl_timermap_t = std::map<UINT_PTR, mcl_timermape_t>;

            mcl_timermap_t*& ptmap = *reinterpret_cast
                <mcl_timermap_t**>(&mcl_control_obj.timermap);

            if (!millseconds) {
                if (!ptmap)
                    return false;

                mcl_timermap_t::iterator it = ptmap -> find (idevent);
                if (it == ptmap -> end ())
                    return false;
            
                if (!::KillTimer (mcl_control_obj.hwnd, idevent))
                    return false;
                ptmap -> erase (it);
                return true;
            }

            if (!ptmap) {
                ptmap = new (std::nothrow) mcl_timermap_t;
                if (!ptmap)
                    return false;
            }
            (*ptmap)[idevent] = { type, loops };
        }
        return static_cast<bool>(::SetTimer (mcl_control_obj.hwnd,
            idevent, static_cast<UINT>(millseconds), 0));
    }


    tclock_t::
    tclock_t () noexcept
      : m_dataplus_ (new (std::nothrow) LARGE_INTEGER[mcl_averaging_max + 1]),
        m_data_ {0} {
        if (m_dataplus_) {
            reinterpret_cast<LARGE_INTEGER*>(m_dataplus_)[mcl_averaging_max - 1].QuadPart = 0;
            reinterpret_cast<LARGE_INTEGER*>(m_dataplus_)[mcl_averaging_max].QuadPart = 0;
        }
    }

    tclock_t::
    ~tclock_t () noexcept{
        if (m_dataplus_) {
            delete[] reinterpret_cast<LARGE_INTEGER*>(m_dataplus_);
            m_dataplus_ = nullptr;
        }
    }

    tclock_t::
    operator void* () const noexcept{
        return m_dataplus_ && (m_data_[0] || reinterpret_cast<LARGE_INTEGER*>
            (m_dataplus_)[mcl_averaging_max - 1].QuadPart) ?
            const_cast<tclock_t*>(this) : nullptr;
    }
    
    bool tclock_t::
    operator! () const noexcept{
        return !(m_dataplus_ && (m_data_[0] || reinterpret_cast<LARGE_INTEGER*>
            (m_dataplus_)[mcl_averaging_max - 1].QuadPart));
    }
       
   /**
    * @function tclock_t::tick <src/timer.h> 
    * @brief update the clock
    * @param {float} framerate: wait() to keep running slower than framerate
    * @return int64_t: the actual number of milliseconds used.
    */
    long long tclock_t::
    tick (float framerate) noexcept{
        if (!m_dataplus_ || framerate < 0.f) return -1ll;
        
        LONGLONG       elapsed = 0ll, lastus = 0ll;
        LARGE_INTEGER* pqs = reinterpret_cast<LARGE_INTEGER*>(m_dataplus_);
        LARGE_INTEGER* prear = pqs + m_data_[0];
        LARGE_INTEGER* pprev = pqs;
        if (m_data_[0]) pprev += m_data_[0] - 1;
        else            pprev += mcl_averaging_max - 1;
        
        if (++ m_data_[0] == mcl_averaging_max)
            m_data_[0] = 0;
        ::QueryPerformanceCounter (prear);

        if (!pprev -> QuadPart)
            return 0ll;
        elapsed = (prear -> QuadPart - pprev -> QuadPart) * 1000000ll / mcl_base_obj.frequency;
        pqs[mcl_averaging_max].QuadPart = elapsed;
        if (!framerate)
            return (elapsed + 500ll) / 1000ll;

        lastus = static_cast<LONGLONG>(1000000.f / framerate + .5f);
        lastus -= elapsed;
        if (lastus < 0ll) lastus = 0ll;
        
        ::Sleep (static_cast<DWORD>(lastus / 1000ll));
        ::QueryPerformanceCounter (prear);
        return (elapsed + 500ll) / 1000ll;
    }
    
   /**
    * @function tclock_t::tick_busy_loop <src/timer.h> 
    * @brief update the clock
    * @param {float} framerate: delay() to keep running slower than framerate
    * @return int64_t: the actual number of microseconds used.
    */
    long long tclock_t::
    tick_busy_loop (float framerate) noexcept{
        if (!m_dataplus_ || framerate < 0.f) return -1ll;
        
        LONGLONG       elapsed = 0ll, lastus = 0ll;
        LARGE_INTEGER* pqs = reinterpret_cast<LARGE_INTEGER*>(m_dataplus_);
        LARGE_INTEGER* prear = pqs + m_data_[0];
        LARGE_INTEGER* pprev = pqs;
        if (m_data_[0]) pprev += m_data_[0] - 1;
        else            pprev += mcl_averaging_max - 1;
        
        if (++ m_data_[0] == mcl_averaging_max)
            m_data_[0] = 0;
        ::QueryPerformanceCounter (prear);

        if (!pprev -> QuadPart)
            return 0ll;
        elapsed = (prear -> QuadPart - pprev -> QuadPart) * 1000000ll / mcl_base_obj.frequency;
        pqs[mcl_averaging_max].QuadPart = elapsed;
        if (!framerate)
            return elapsed;

        lastus = static_cast<LONGLONG>(1000000.f / framerate + .5f);
        if (lastus <= elapsed) lastus = 0ll;
        if (!lastus) return elapsed;

        do {
            ::QueryPerformanceCounter (prear);
            elapsed = (prear -> QuadPart - pprev -> QuadPart) * 1000000ll / mcl_base_obj.frequency;
        } while (elapsed < lastus);
        return pqs[mcl_averaging_max].QuadPart;
    }

    /**
     * @function tclock_t::get_time <src/timer.h>
     * @brief time used in the previous tick
     * @return int64_t: the actual number of milliseconds / microseconds used.
     */
    long long tclock_t::
    get_time () noexcept{
        return (get_time_us () + 500ll) / 1000ll;
    }
    long long tclock_t::
    get_time_us () noexcept{
        if (!m_dataplus_) return -1ll;

        LARGE_INTEGER* pqs = reinterpret_cast<LARGE_INTEGER*>(m_dataplus_);
        if (!pqs[mcl_averaging_max - 1].QuadPart && m_data_[0] <= 1)
            return -1ll;

        LARGE_INTEGER* pprev = pqs + (m_data_[0] - 1), *pprev2 = pqs + (m_data_[0] - 2);
        if (!m_data_[0])     pprev  += mcl_averaging_max;
        if (m_data_[0] <= 1) pprev2 += mcl_averaging_max;
        return (pprev -> QuadPart - pprev2 -> QuadPart) * 1000000ll / mcl_base_obj.frequency;
    }

    /**
     * @function tclock_t::get_rawtime <src/timer.h>
     * @brief actual time used in the previous tick
     * @return int64_t: similar to get_time(), but does not include any time
     *     used while tick() was delaying to limit the framerate.
     */
    long long tclock_t::
    get_rawtime () noexcept{
        return (get_rawtime_us () + 500ll) / 1000ll;
    }
    long long tclock_t::
    get_rawtime_us () noexcept{
        if (!m_dataplus_) return -1ll;

        LARGE_INTEGER* pqs = reinterpret_cast<LARGE_INTEGER*>(m_dataplus_);
        if (!pqs[mcl_averaging_max - 1].QuadPart && m_data_[0] <= 1)
            return -1ll;

        return pqs[mcl_averaging_max].QuadPart;
    }

    /**
     * @function tclock_t::get_fps <src/timer.h>
     * @brief compute the clock framerate
     * @return float: your game's framerate (in frames per second)
     */
    float tclock_t::
    get_fps () noexcept{
        if (!m_dataplus_) return 0.f;
        
        LARGE_INTEGER* pqs = reinterpret_cast<LARGE_INTEGER*>(m_dataplus_);
        LONGLONG tpass = 0ll, tnum = static_cast<LONGLONG>(mcl_averaging_max) - 1ll;
        if (!pqs[tnum].QuadPart) {
            if (m_data_[0] <= 1)
                return 0.f;
            tnum = static_cast<LONGLONG>(m_data_[0]) - 1ll;
            tpass = pqs[tnum].QuadPart - pqs[0].QuadPart;
        } else {
            tpass = -pqs[static_cast<int>(m_data_[0])].QuadPart;
            if (m_data_[0]) tpass += pqs[m_data_[0] - 1].QuadPart;
            else            tpass += pqs[mcl_averaging_max - 1].QuadPart;
        }
        return static_cast<float>(static_cast<double>(tnum) *
            static_cast<double>(mcl_base_obj.frequency) / static_cast<double>(tpass));
    }

} // namespace    
