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
    
    @file cpp/music.cpp
    This is a C++11 implementation file for controling streamed
    audio.
*/


#include "../src/mixer.h"
#include "mcl_base.h"

#include <mmsystem.h>
#include <limits>
#include <string>

/* 
// Code within this comment is part of header digitalv.h.
// This header is used by Windows Multimedia. However, most
// mci devices do not support this instruction.

extern "C" {
    typedef struct tagMCI_DGV_SETAUDIO_PARMSW {
        DWORD_PTR dwCallback;
        DWORD     dwItem;
        DWORD     dwValue;
        DWORD     dwOver;
        LPWSTR    lpstrAlgorithm;
        LPWSTR    lpstrQuality;
    } MCI_DGV_SETAUDIO_PARMSW, MCI_DGV_SETAUDIO_PARMS;

# ifndef MCI_DGV_SETAUDIO_VOLUME
#   define MCI_DGV_SETAUDIO_ITEM               0x00800000L
#   define MCI_DGV_SETAUDIO_VALUE              0x01000000L
#   define MCI_DGV_SETAUDIO_VOLUME             0x00004002L
#   define MCI_SETAUDIO                        0x0873
# endif
} // digitalv.h
*/

namespace
mcl {
    
    class mcl_music_base_t {
    public:  // library guard
        mcl_music_base_t () noexcept{ }
        ~mcl_music_base_t () noexcept { free_lib (); }
        
    public: 
        typename mcl_simpletls_ns::mcl_spinlock_t::
        lock_t         ldlib_lock = 0u;
        HINSTANCE      hInsWinmm  = nullptr;
        MCIERROR       load_lib (MCIDEVICEID mciId,
                            UINT uMsg, DWORD_PTR dwParam1,
                            DWORD_PTR dwParam2) noexcept;
        void           free_lib () noexcept;
        HANDLE         handle     = nullptr; 
        
    public:  // mci info
        MCIDEVICEID    nDeviceID  = ~1u+1u;
        int            nLoopCnt   = 0;
        float          fLoopSetT  = .0f;
        DWORD          dwStatus   = ~1ul+1ul;
        DWORD          dwLastSetT = ~1ul+1ul;
        std::wstring   sQueueName;
        int            iQueueLoop = -2;
        
    public:  // post & get
        UINT           thread_mci (void*);
        bool           load_thread ();
        DWORD          post_      = mNotReady;
        float          f_ret_     = .0f;
        bool           b_ret_     = false;
        wchar_t const* sparam_    = nullptr;
        float          fparam_    = .0f;
        float          fparam2_   = .0f;
        int            iparam_    = 0;
    
    public:  // fade
        enum: DWORD{ fadeNull = ~2ul+1ul,
            fadeIn = ~1ul+1ul, fadeOutMax = ~3ul+1ul };
        typename mcl_simpletls_ns::mcl_spinlock_t::
        lock_t         fade_setvol_lock=0u;
        DWORD          fadeFrmVol = 0ul;
        DWORD          fadeTarVol = 0ul;
        DWORD          fade_      = fadeNull; 
        LONGLONG       fadeTickSt = 0ll;
        DWORD          fadeMsLast = 0ul;
        
    public:
        enum: DWORD{ mNoMsg, mNotReady, mExit, mLoad, mUnload,
                     mPlay, mRewind, mStop, mPause, mUnpause,
                     mGetPos, mSetPos, mGetLength, mQueue,
                     mFadeout };
        
        bool         load       (wchar_t const* filename) noexcept;
        bool         queue      (wchar_t const* filename,
                                            int loops) noexcept; 
        bool         unload     ()                     noexcept; 
        bool         play       (int loops,
                        float start_s, float fadein_s) noexcept; 
        bool         rewind     ()                     noexcept; 
        bool         stop       ()                     noexcept; 
        bool         fadeout    (float fadeout_s)      noexcept; 
        bool         pause      ()                     noexcept; 
        bool         unpause    ()                     noexcept; 
        float        get_pos    ()                     noexcept; 
        bool         set_pos    (float pos_s)          noexcept; 
        float        get_length ()                     noexcept; 
        
        void         check_end  ()                     noexcept; 
        void         check_fade ()                     noexcept;
        void         end_fade   ()                     noexcept;
    };
    
    static mcl_music_base_t mcl_music_base;
    mcl_mixer_t::mcl_music_t mcl_mixer_t::music;
        // mcl_mixer_t::music should define after mcl_music_base.
        
    static bool  mcl_set_volume (DWORD dw_volume) noexcept;
    static DWORD mcl_get_volume () noexcept;
    
    
   /**
    * @function mcl_mixer_t::mcl_music_t::operator void* <src/music.h>
    * @brief Test if the music stream is playing.
    * @return void*
    */
    mcl_mixer_t::mcl_music_t::
    operator void* () const noexcept{
        return music.get_busy () ?
            const_cast<mcl_mixer_t::mcl_music_t*>(this) : nullptr; 
    } 
    bool mcl_mixer_t::mcl_music_t::
    operator! () const noexcept{
        return !music.get_busy();
    } 
    
/**
 * Thread for controling streamed audio.
 *  @param {void} 
 *  @return {UINT}: 0u
 */
    UINT mcl_music_base_t::
    thread_mci (void*) {
        while (1) {
            switch (post_) {
            case mNoMsg:     check_fade (); check_end (); ::Sleep (1); continue;
            case mExit:      return 0u;
            case mLoad:      b_ret_ = load (sparam_);     post_ = mNoMsg; break;
            case mQueue:     b_ret_ = queue (sparam_,
                                            iparam_);     post_ = mNoMsg; break;
            case mUnload:    b_ret_ = unload ();          post_ = mNoMsg; break;
            case mPlay:      b_ret_ = play (iparam_,
                                  fparam_, fparam2_);     post_ = mNoMsg; break;
            case mRewind:    b_ret_ = rewind ();          post_ = mNoMsg; break;
            case mStop:      b_ret_ = stop ();            post_ = mNoMsg; break;
            case mPause:     b_ret_ = pause ();           post_ = mNoMsg; break;
            case mUnpause:   b_ret_ = unpause ();         post_ = mNoMsg; break;
            case mFadeout:   b_ret_ = fadeout (fparam_);  post_ = mNoMsg; break;
            case mGetPos:    f_ret_ = get_pos ();         post_ = mNoMsg; break;
            case mSetPos:    b_ret_ = set_pos (fparam_);  post_ = mNoMsg; break;
            case mGetLength: f_ret_ = get_length ();      post_ = mNoMsg; break;
            default:                                      post_ = mNoMsg; break;
            } check_end (); ::Sleep (0);
        }
    }
    bool mcl_music_base_t::
    load_thread () {
        if (post_ != mNotReady) return false;
        mcl_simpletls_ns::mcl_spinlock_t lock (ldlib_lock);
        if (post_ != mNotReady) return false;
        post_ = mNoMsg;
        auto fun = mcl_simpletls_ns::bind_mf (&mcl_music_base_t::thread_mci, this);
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 5039)
#       endif // never throw an exception passed to extern C function
        handle = HANDLE(::_beginthreadex (nullptr, 0, fun, nullptr, 0, nullptr));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
        return !handle;
    }
    
/**
 * Load/Free function mciSendCommandA in winmm.lib
 *  @param {void}
 *  @return {ULONG} error_code
 */
    MCIERROR mcl_music_base_t::
    load_lib ( MCIDEVICEID mciId, UINT uMsg,
              DWORD_PTR dwParam1, DWORD_PTR dwParam2 ) noexcept {
        using             mci_pfun_t       = decltype (&mciSendCommandW);
        static mci_pfun_t mci_sendstring_f = nullptr;
        if (!mci_sendstring_f) {
            if (!hInsWinmm) hInsWinmm = ::LoadLibraryW (L"winmm.dll");
            if (hInsWinmm) {
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 4191)
#       endif
                mci_sendstring_f = reinterpret_cast<mci_pfun_t>(
                    ::GetProcAddress (hInsWinmm, "mciSendCommandW"));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
                if (!mci_sendstring_f) return ~1ul+1ul;
                return mci_sendstring_f (mciId, uMsg, dwParam1, dwParam2);
            } return ~1ul+1ul;
        } return mci_sendstring_f (mciId, uMsg, dwParam1, dwParam2);
    }
    void mcl_music_base_t::
    free_lib () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (ldlib_lock);
        if (hInsWinmm) {
            post_ = mExit;
            ::FreeLibrary (hInsWinmm);
            hInsWinmm = nullptr;
        }
    }  
 
/**
 * Destructor.
 *  @param {void}
 *  @return {void} 
 */
    mcl_mixer_t::mcl_music_t::
    ~mcl_music_t () noexcept {
        unload ();
    }
    
/**
 * Load a music file for playback.
 *  @param {char const*} filename 
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    load (wchar_t const* filename) noexcept {
        if (!filename) return false;
        if (fade_ != fadeNull) end_fade ();
        
        MCIERROR ret = 0u;
        if (~1u+1u != nDeviceID) {
            MCI_GENERIC_PARMS mciGP{0};
            ret = load_lib (nDeviceID, MCI_CLOSE, MCI_WAIT,
                      DWORD_PTR(static_cast<LPVOID>(&mciGP)) );
            if (ret) return false;
        }
        iQueueLoop = -2;
        
        MCI_OPEN_PARMSW mciOP{0, 0, 0, 0, 0};
        mciOP.lpstrElementName = filename;
        ret       = load_lib (0u, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_WAIT,
                                  DWORD_PTR(static_cast<LPVOID>(&mciOP)));
        nDeviceID = (ret ? ~1u+1u : mciOP.wDeviceID);
        dwStatus  = MCI_SEEK;
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    load (wchar_t const* filename) noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.mExit == mcl_music_base.post_) return false;
        
        mcl_music_base.sparam_ = filename;
        mcl_music_base.post_   = mcl_music_base.mLoad;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }
    bool mcl_mixer_t::mcl_music_t::
    load (char const* filename) noexcept {
        if (!filename) return false;
        mcl_simpletls_ns::mcl_m2w_str_t wstr (filename);
        return load (wstr); 
    }
    
/**
 * Unload the currently loaded music to free up resources.
 *  @param {void}
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    unload () noexcept {
        if (nDeviceID == ~1u+1u) return false;
        if (fade_ != fadeNull) end_fade ();
        
        MCI_GENERIC_PARMS mciGP{0};
        MCIERROR ret = load_lib (nDeviceID, MCI_CLOSE, MCI_WAIT,
                                    DWORD_PTR(static_cast<LPVOID>(&mciGP)) );
        dwLastSetT   = ~1ul+1ul;
        dwStatus     = ~1ul+1ul;
        nDeviceID    = (ret ? nDeviceID : ~1u+1u);
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    unload () noexcept {
        if (mcl_music_base.post_ == mcl_music_base.mNotReady) return false;
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mNotReady
        || mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.post_ = mcl_music_base.mUnload;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    } 
    
/**
 * Start the playback of the music stream.
 *  @param {int,float,float} loops, start_s, fadein_s
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    play (int loops, float start_s, float fadein_s) noexcept { 
        if (nDeviceID == ~1u+1u) return false;
        if (!(dwStatus == MCI_SEEK || dwStatus == MCI_PLAY)) return false;
        if (loops < -1) return false;
        
        if (start_s < .0f) return false;
        start_s = start_s * 1000.f + .5f;
        if (static_cast<ULONGLONG>(start_s) >= (std::numeric_limits<DWORD>::max) ())
            start_s = static_cast<float>((std::numeric_limits<DWORD>::max) () - 1ul);
        
        if (fadein_s > .0f) {
            fadein_s = fadein_s * 1000.f + .5f;
            if (static_cast<ULONGLONG>(fadein_s) >= (std::numeric_limits<DWORD>::max) ())
                fadein_s = static_cast<float>((std::numeric_limits<DWORD>::max) () - 1ul);
            
            if (fade_ == fadeNull) {
                DWORD nwVol = mcl_get_volume ();
                mcl_set_volume (0ul);
                fadeFrmVol = 0ul;
                fadeTarVol = nwVol;
            } else if (fade_ == fadeIn) {
                fadeFrmVol = mcl_get_volume ();
            //  fadeTarVol = fadeTarVol;
            } else {
                fadeFrmVol = mcl_get_volume ();
                fadeTarVol = fade_;
            }
            LARGE_INTEGER tickCount;
            ::QueryPerformanceCounter (&tickCount);
            fadeTickSt = tickCount.QuadPart;
            fadeMsLast = static_cast<DWORD>(fadein_s);
            fade_      = fadeIn;
            
        } else if (fade_ != fadeNull) end_fade ();

        MCI_PLAY_PARMS mciPP{0, 0, 0};
        mciPP.dwFrom = static_cast<DWORD>(fLoopSetT = start_s);
        nLoopCnt     = loops;
        MCIERROR ret = load_lib (nDeviceID, MCI_PLAY, MCI_FROM,
                                    DWORD_PTR(static_cast<LPVOID>(&mciPP)) );
        dwStatus     = MCI_PLAY;
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    play (int loops, float start_s, float fadein_s) noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.iparam_  = loops;
        mcl_music_base.fparam_  = start_s;
        mcl_music_base.fparam2_ = fadein_s;
        mcl_music_base.post_    = mcl_music_base.mPlay;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }    
    
/**
 * Restart music
 *  @param {void}
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    rewind () noexcept {
        if (nDeviceID == ~1u+1u) return false;
        if (dwStatus == MCI_PAUSE) {
            dwLastSetT = 0ul;
            return true;
        } else if (dwStatus != MCI_PLAY)
            return false;
        
        MCI_PLAY_PARMS mciPP{0, 0, 0};
        mciPP.dwFrom = 0ul;
        MCIERROR ret = load_lib (nDeviceID, MCI_PLAY, MCI_FROM,
                                   DWORD_PTR(static_cast<LPVOID>(&mciPP)) );
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    rewind () noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.post_ = mcl_music_base.mRewind;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }  
    
/**
 * Test if the music is ended and then repeat it.
 *  @param {void}
 *  @return {void}
 */
    void mcl_music_base_t::
    check_end () noexcept {
        if (nDeviceID == ~1u+1u || MCI_PLAY != dwStatus) return ;
        
        MCI_STATUS_PARMS mciMSP{0, 0, 0, 0};
        mciMSP.dwItem = MCI_STATUS_MODE;
        if (load_lib (nDeviceID, MCI_STATUS, 
            MCI_STATUS_ITEM | MCI_WAIT, DWORD_PTR(static_cast<LPVOID>(&mciMSP)) ) )
                return ;
        
        if (mciMSP.dwReturn != MCI_MODE_PLAY) {
            if (!nLoopCnt) {
                if (iQueueLoop == -2) { dwStatus = MCI_SEEK; return ; }
                int loops = iQueueLoop;  iQueueLoop = -2; 
                if (!load (sQueueName.c_str ()))  dwStatus = MCI_SEEK;
                else if (!play (loops, .0f, .0f)) dwStatus = MCI_SEEK;
                return ;
            }
            if (nLoopCnt != -1) -- nLoopCnt;
        
            MCI_SEEK_PARMS mciSP{0, 0};
            MCIERROR ret = load_lib (nDeviceID,
                MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_START,
                DWORD_PTR(static_cast<LPVOID>(&mciSP)) );
            if (ret) return ;
        
            MCI_PLAY_PARMS mciPP{0, 0, 0};
            mciPP.dwFrom = static_cast<DWORD>(fLoopSetT);
            load_lib (nDeviceID, MCI_PLAY, MCI_FROM,
                DWORD_PTR(static_cast<LPVOID>(&mciPP)) );
        }
    }   
    
/**
 * fading the music if needed.
 *  @param {void}
 *  @return {void}
 */
    void mcl_music_base_t::
    end_fade () noexcept {
        DWORD fade = fade_;
        fade_ = fadeNull; 
        mcl_set_volume ((fade == fadeIn) ? fadeTarVol : fade);
    }
    
    void mcl_music_base_t::
    check_fade () noexcept {
        if (fade_ == fadeNull) return ;
        
        // lock_used_in_set_volume_and_end_fade
        mcl_simpletls_ns::mcl_spinlock_t lock2 (fade_setvol_lock);
        if (fade_ == fadeNull) return ;
        
        // Judge whether the current time exceeds
        // the set fade out time.
        LARGE_INTEGER stopCount;
        ::QueryPerformanceCounter (&stopCount);
        DWORD elapsed = static_cast<DWORD>((stopCount.QuadPart - fadeTickSt)
                    * 1000ll / mcl_base_obj.frequency);
        
        if (fade_ == fadeIn) {
            // If time exceeds, set it directly as the target volume
            // and end fading.
            if (elapsed >= fadeMsLast) { end_fade (); return ; }
            if (fadeTarVol == 0ul) return ;
            
            double result = static_cast<double>(elapsed) / fadeMsLast;
            result *= static_cast<double>(fadeTarVol - fadeFrmVol);
            mcl_set_volume (static_cast<DWORD>(fadeFrmVol + result + .5f));
        } else { 
            if (elapsed >= fadeMsLast) { stop (); return ; }
            if (fadeFrmVol == 0ul) return ;  // volume before fade is 0.
            
            // Otherwise, elapsed/fadeMsLast = (fade-resultVal)/fade
            double result = static_cast<double>(elapsed)
                            / static_cast<double>(fadeMsLast);
            result *= static_cast<double>(fadeFrmVol);
            result += .5f;
            mcl_set_volume (fadeFrmVol - static_cast<DWORD>(result));
        }
    }   

/**
 * Stop the music playback.
 *  @param {void}
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    stop () noexcept {
        if (nDeviceID == ~1u+1u) return false;
        if (fade_ != fadeNull) end_fade ();
        
        MCI_SEEK_PARMS mciSP{0, 0};
        MCIERROR ret = load_lib (nDeviceID,
            MCI_SEEK, MCI_WAIT | MCI_SEEK_TO_START,
            DWORD_PTR(static_cast<LPVOID>(&mciSP)) );
        dwLastSetT   = ~1ul+1ul;
        dwStatus     = MCI_SEEK;
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    stop () noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.post_ = mcl_music_base.mStop;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    } 
    
/**
 * Stop music playback after fading out.
 *  @param {float} fadeout_s
 *  @return {bool}: true if successful
 */ 
    bool mcl_music_base_t::
    fadeout (float fadeout_s) noexcept {
        if (fadeout_s <= .0f) return stop ();
        if (nDeviceID == ~1u+1u) return false;
        
        fadeFrmVol = mcl_get_volume ();
        if (fade_ == fadeNull)    fade_ = fadeFrmVol;
        else if (fade_ == fadeIn) fade_ = fadeTarVol;
        fadeTarVol = 0ul;
        
        fadeout_s = fadeout_s * 1000.f + .5f;
        if (static_cast<ULONGLONG>(fadeout_s)
          >= (std::numeric_limits<DWORD>::max) ())
            fadeout_s = static_cast<float>((std::numeric_limits<DWORD>::max) () - 1ul);
        fadeMsLast = static_cast<DWORD>(fadeout_s);
        
        LARGE_INTEGER tickCount;
        ::QueryPerformanceCounter (&tickCount);
        fadeTickSt = tickCount.QuadPart;
        return true;
    }
    bool mcl_mixer_t::mcl_music_t::
    fadeout (float fadeout_s) noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.fparam_ = fadeout_s;
        mcl_music_base.post_   = mcl_music_base.mFadeout;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }   
    
/**
 * Queue a sound file to follow the current.
 *  @param {char const*, int} filename, loops 
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    queue (wchar_t const* filename, int loops) noexcept { 
        if (~1u+1u == nDeviceID) {
            if (!load (filename))
                return false;
            return play (loops, .0f, .0f);
        }
        
        if (!filename) return false;
        if (loops < -1) return false;
        
        sQueueName = filename;
        iQueueLoop = loops;
        return true;
    }
    bool mcl_mixer_t::mcl_music_t::
    queue (wchar_t const* filename, int loops) noexcept { 
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.mExit == mcl_music_base.post_) return false;
        
        mcl_music_base.sparam_ = filename;
        mcl_music_base.iparam_ = loops;
        mcl_music_base.post_   = mcl_music_base.mQueue;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }
    bool mcl_mixer_t::mcl_music_t::
    queue (char const* filename, int loops) noexcept { 
        if (!filename) return false;
        mcl_simpletls_ns::mcl_m2w_str_t wstr (filename);
        return queue (wstr, loops); 
    }
    
/**
 * Temporarily stop music playback
 *  @param {void}
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    pause () noexcept {
        if (nDeviceID == ~1u+1u) return false;
        
        MCI_GENERIC_PARMS mciGP{0};
        MCIERROR ret = load_lib (nDeviceID, MCI_PAUSE, 0ul,
                                   DWORD_PTR(static_cast<LPVOID>(&mciGP)) );
        dwStatus     = MCI_PAUSE;
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    pause () noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.post_ = mcl_music_base.mPause;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }  

/**
 * Resume paused music
 *  @param {void}
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    unpause () noexcept {
        if (nDeviceID == ~1u+1u) return false;
        if (dwStatus != MCI_PAUSE) return false;
        
        if (dwLastSetT == ~1ul+1ul) {
            MCI_GENERIC_PARMS mciGP{0};
            MCIERROR ret = load_lib (nDeviceID, MCI_RESUME, 0ul,
                                   DWORD_PTR(static_cast<LPVOID>(&mciGP)) );
            dwStatus     = MCI_PLAY;
            return !ret;
        } 
        
        MCI_PLAY_PARMS mciPP{0, 0, 0};
        mciPP.dwFrom = dwLastSetT, dwLastSetT = ~1ul+1ul;
        MCIERROR ret = load_lib (nDeviceID, MCI_PLAY, MCI_FROM,
                                   DWORD_PTR(static_cast<LPVOID>(&mciPP)) );
        dwStatus     = MCI_PLAY;
        return !ret;  
    }
    bool mcl_mixer_t::mcl_music_t::
    unpause () noexcept {
        if (mcl_music_base.load_thread ()) return false;
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        mcl_music_base.post_ = mcl_music_base.mUnpause;
        
        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }    
    
/**
 * Get the music volume.
 *  @param {void}
 *  @return {float} volume: [0,1] 
 */
    static DWORD mcl_get_volume () noexcept {
        using             wav_pfun_t      = decltype (&waveOutGetVolume);
        static wav_pfun_t wav_getvolume_f = nullptr;
        DWORD             volume          = 0ul;
        if (!wav_getvolume_f) {
            if (!mcl_music_base.hInsWinmm)
                mcl_music_base.hInsWinmm = ::LoadLibraryW (L"winmm.dll");
            if (mcl_music_base.hInsWinmm)
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 4191)
#       endif
                wav_getvolume_f = reinterpret_cast<wav_pfun_t> (
                    ::GetProcAddress (mcl_music_base.hInsWinmm, "waveOutGetVolume"));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
        }
        if (wav_getvolume_f) wav_getvolume_f (0, &volume);
        return volume & 0xffff;
    }
    float mcl_mixer_t::mcl_music_t::
    get_volume () noexcept {
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        // lock_used_in_set_volume_and_end_fade
        mcl_simpletls_ns::mcl_spinlock_t lock2 (mcl_music_base.fade_setvol_lock);
        if (mcl_music_base.fade_ == mcl_music_base.fadeNull) 
               return static_cast<float>(mcl_get_volume ()) / 0xffff;
        else if (mcl_music_base.fade_ == mcl_music_base.fadeIn) 
               return static_cast<float>(mcl_music_base.fadeTarVol);
        return static_cast<float>(mcl_music_base.fade_);
    }

/**
 * Set the music volume.
 *  @param {float} volume: [0, 1]
 *  @return {bool}: true if successful
 */
    static bool
    mcl_set_volume (DWORD dw_volume) noexcept {
        using             wav_pfun_t      = decltype (&waveOutSetVolume);
        static wav_pfun_t wav_setvolume_f = nullptr;
        MCIERROR          ret             = ~1ul+1ul;
        
        class mcl_volume_guard_t {  DWORD volume;  public:
            explicit mcl_volume_guard_t () noexcept
                : volume (mcl_get_volume ()) { }
            ~mcl_volume_guard_t () noexcept
            { mixer.music.set_volume (static_cast<float>(volume / 0xffff) + .5f); }
        };
        static mcl_volume_guard_t mcl_volume_guard_obj;
# ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4191)
# endif
        if (!wav_setvolume_f) wav_setvolume_f = reinterpret_cast<wav_pfun_t> (
            ::GetProcAddress (mcl_music_base.hInsWinmm, "waveOutSetVolume"));
# ifdef _MSC_VER
#   pragma warning(pop)
# endif
        if (wav_setvolume_f)
            ret = wav_setvolume_f (0, dw_volume | (dw_volume << 16));
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    set_volume (float volume) noexcept {
        if (volume < 0.f) return false;
        if (volume > 1.f) volume = 1.f;
        DWORD vol = static_cast<DWORD>(volume * 0xffff + .5f);
        
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;
        
        // lock_used_in_set_volume_and_end_fade
        mcl_simpletls_ns::mcl_spinlock_t lock2 (mcl_music_base.fade_setvol_lock);
        if (mcl_music_base.fade_ == mcl_music_base.fadeNull)
            return mcl_set_volume (vol);
        if (mcl_music_base.fade_ == mcl_music_base.fadeIn) {
            mcl_music_base.fade_ = mcl_music_base.fadeNull;
            return mcl_set_volume (vol);
        }
        mcl_music_base.fade_ = vol;
        return true;
    }
    
/**
 * Get the music play time.
 *  @param {void}
 *  @return {float}: the number of milliseconds that the music has been playing for.
 */
    float mcl_music_base_t::
    get_pos () noexcept {
        if (nDeviceID == ~1u+1u) return false;

        MCI_STATUS_PARMS mciMSP{0, 0, 0, 0};
        mciMSP.dwItem = MCI_STATUS_POSITION;
        load_lib (nDeviceID, MCI_STATUS, 
            MCI_STATUS_ITEM | MCI_WAIT, DWORD_PTR(static_cast<LPVOID>(&mciMSP)) );
        return static_cast<float>(mciMSP.dwReturn) / 1000.f;
    }
    float mcl_mixer_t::mcl_music_t::
    get_pos () noexcept {
        if (mcl_music_base.load_thread ()) return .0f;

        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return .0f;

        mcl_music_base.post_ = mcl_music_base.mGetPos;

        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.f_ret_;
    }
    
/**
 * Set the music play time.
 *  @param {void}
 *  @return {bool}: true if successful
 */
    bool mcl_music_base_t::
    set_pos (float pos_s) noexcept {
        if (nDeviceID == ~1u+1u) return false;
        if (pos_s < .0f) return false;
        
        pos_s = pos_s * 1000.f + .5f;
        if (static_cast<ULONGLONG>(pos_s) >= (std::numeric_limits<DWORD>::max) ())
            pos_s = static_cast<float>((std::numeric_limits<DWORD>::max) () - 1ul);
        
        MCIERROR       ret = ~1ul+1ul;
        MCI_PLAY_PARMS mciPP{0, 0, 0};
        mciPP.dwFrom       = static_cast<DWORD>(pos_s);
        if (dwStatus == MCI_PLAY)
            ret = load_lib (nDeviceID, MCI_PLAY, MCI_FROM,
                                       DWORD_PTR(static_cast<LPVOID>(&mciPP)) );
        else if (dwStatus == MCI_PAUSE)
            dwLastSetT = mciPP.dwFrom, ret = 0u;
        return !ret;
    }
    bool mcl_mixer_t::mcl_music_t::
    set_pos (float pos_s) noexcept {
        if (mcl_music_base.load_thread ()) return false;

        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return false;

        mcl_music_base.fparam_ = pos_s;
        mcl_music_base.post_   = mcl_music_base.mSetPos;

        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.b_ret_;
    }
    
/**
 * Get the music length
 *  @param {void}
 *  @return {float}: the number of milliseconds that the music has been playing for.
 */
    float mcl_music_base_t::
    get_length () noexcept {
        if (nDeviceID == ~1u+1u) return false;

        MCI_STATUS_PARMS mciMSP{0, 0, 0, 0};
        mciMSP.dwItem = MCI_STATUS_LENGTH;
        load_lib (nDeviceID, MCI_STATUS, MCI_STATUS_ITEM | MCI_WAIT,
                               DWORD_PTR(static_cast<LPVOID>(&mciMSP)) );
        return static_cast<float>(mciMSP.dwReturn) / 1000.f;
    }
    float mcl_mixer_t::mcl_music_t::
    get_length () noexcept {
        if (mcl_music_base.load_thread ()) return .0f;

        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_music_base.ldlib_lock);
        if (mcl_music_base.post_ == mcl_music_base.mExit) return .0f;

        mcl_music_base.post_ = mcl_music_base.mGetLength;

        while (mcl_music_base.post_ != mcl_music_base.mNoMsg) ::Sleep (0);
        return mcl_music_base.f_ret_;
    }
    
/**
 * Check if the music stream is playing
 *  @param {void}
 *  @return {bool}: true if the music stream is playing
 */
    bool mcl_mixer_t::mcl_music_t::
    get_busy() noexcept {
        return MCI_PLAY == mcl_music_base.dwStatus;
    }
    
}
