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
    
    @file cpp/clog4m.cpp
    This is a C++11 implementation file for logging.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/clog4m.h"
#include "mcl_base.h" // for spinlock

#ifdef _MSC_VER
# pragma warning(pop)
#endif

#include <streambuf>  // for basic_streambuf
#include <cstdio>     // for vsnprintf
#include <cstdarg>
#include <io.h>

namespace
mcl {
    
    // Enum class of log level for clog4m.
    mcl_cll4m_t cll4m; 
    // Module for loging.
    mcl_clog4m_t clog4m; 

#if __cplusplus < 201703L
    cll4m_t constexpr mcl_cll4m_t::All;
    cll4m_t constexpr mcl_cll4m_t::Int;
    cll4m_t constexpr mcl_cll4m_t::Trace;
    cll4m_t constexpr mcl_cll4m_t::Debug;
    cll4m_t constexpr mcl_cll4m_t::Info;
    cll4m_t constexpr mcl_cll4m_t::Warn;
    cll4m_t constexpr mcl_cll4m_t::Error;
    cll4m_t constexpr mcl_cll4m_t::Fatal;
    cll4m_t constexpr mcl_cll4m_t::Off;
#endif
    
   /**
    * @class mcl_logbuf_t <src/clog4m.cpp>
    * @brief The buffer for clog4m_t logger object.
    */    
    class mcl_logbuf_t
      : public std::basic_streambuf<wchar_t, std::char_traits<wchar_t> > {
    
    protected:
        // For EOF detection
        typedef std::char_traits<wchar_t> traits_t;
        // This function is called when stream is flushed,
        // for example when std::endl is put to stream.
        virtual wint_t overflow (wint_t c = traits_t::eof ()) override;
        virtual int sync () override;
        
    private:
        // The flag that prepares for the next character to be printed or
        // displayed on the next line
        // Output stream
        cll4m_t log_level_;
        
        // Work in buffer mode. It is also possible to work without buffer.
        enum { buf_size = 64 };
        char_type Cbuf_[buf_size];
        bool bf_fail_ = false;
        char : 8; char : 8; char : 8;

        friend class clog4m_t;
        friend class mcl_clog4m_t;
        
    public:
        mcl_logbuf_t () noexcept;
        ~mcl_logbuf_t () noexcept;
        MCL_NODISCARD_CXX17 static mcl_logbuf_t* get_tl_obj () noexcept;
        
    private:
        // In this function, the characters are parsed.
        void pt_chars (const wchar_t* begin, const wchar_t* end) noexcept;
    
    };
    
   /**
    * @function mcl_logbuf_t::overflow <src/clog4m.cpp>
    * @brief Clear buffer when stack is full.
    * @param {wint_t} c: the one character that didn't fit to buffer
    * @return {wint_t} c
    */
    inline wint_t mcl_logbuf_t::
    overflow (wint_t c) {
    // This is called when buffer becomes full. If
    // buffer is not used, then this is called every
    // time when characters are put to stream.
        // Handle output
        pt_chars (this -> pbase (), this -> pptr ());
        if (c != traits_t::eof ()) {
            char_type c2 = c;
            // Handle the one character that didn't fit to buffer
            pt_chars (&c2, &c2 + 1);
        }
        // This tells that buffer is empty again.
        this -> setp (Cbuf_, Cbuf_ + buf_size);
        return c;
    }
    
   /**
    * @function mcl_logbuf_t::sync <src/clog4m.cpp>
    * @brief Synchronize the stream with its corresponding data source.
    * @return int
    */
    inline int mcl_logbuf_t::
    sync () {
    // This function is called when stream is flushed,
    // for example when std::endl is put to stream.
        // Handle output
        pt_chars (this -> pbase (), this -> pptr ());
        // This tells that buffer is empty again.
        this -> setp (Cbuf_, Cbuf_ + buf_size);
        return 0;
    }    
    
    
   /**
    * @class mcl_logf_t <src/clog4m.cpp>
    * @brief Information of logger. 
    */ 
    struct mcl_logf_t {
        FILE* f_u8log = nullptr;
            // ptr to log file 
        typename mcl_simpletls_ns::mcl_spinlock_t::
        lock_t nrtlock_obj = 0u;
            // non-reentrant spinlock ( to ensure orderly writing
            //  of log files)
        DWORD dw_lastthread_id = 0ul;
        unsigned long dw_cnt = 0;
            // object counter
        unsigned u_level = cll4m.All.value;
            // log level
        size_t dw_indent = 0ul;
            // number of indents on the beginning of each line
        bool b_console = false;
            // whether console is used
        bool b_newl = true;
            // line feed identifier
            // (to ensure that the current time is output at the
            //  beginning of the line)
        char : 8; char : 8; char : 8; char : 8; char : 8;
        char : 8;
    };
    static mcl_logf_t mcl_logf_obj;
    
   /**
    * @function mcl_ptnewline <src/clog4m.cpp>
    * @brief Print header to the beginning of each line of the file.
    * @param {cll4m_t} level: logLevel
    * @return void
    */
    static void
    mcl_ptnewline (cll4m_t level) noexcept{
    // This function is called when time string needed.
        static wchar_t mcl_time_s[25] =
        { '[', 0, 0, 0, 0, '-', 0, 0, '-', 0, 0, ' ',
               0, 0, ':', 0, 0, ':', 0, 0, '.', 0, 0, 0, ']' };
        static ULONGLONG mcl_tickcnt_dw = 0;

        // timer
        ULONGLONG new_tick = MCL_GETTICKCOUNT ();
            // 'GetTickCount' runs faster than 'GetLocalTime' so much.
        if (new_tick != mcl_tickcnt_dw) {
            mcl_tickcnt_dw = new_tick;
            SYSTEMTIME lt;
            ::GetLocalTime (&lt);
            mcl_time_s[1]  = static_cast<wchar_t>(lt.wYear / 1000     + '0');
            mcl_time_s[2]  = static_cast<wchar_t>(lt.wYear / 100 % 10 + '0');
            mcl_time_s[3]  = static_cast<wchar_t>(lt.wYear / 10  % 10 + '0');
            mcl_time_s[4]  = static_cast<wchar_t>(lt.wYear       % 10 + '0');
            mcl_time_s[6]  = static_cast<wchar_t>(lt.wMonth  / 10 + '0');
            mcl_time_s[7]  = static_cast<wchar_t>(lt.wMonth  % 10 + '0');
            mcl_time_s[9]  = static_cast<wchar_t>(lt.wDay    / 10 + '0');
            mcl_time_s[10] = static_cast<wchar_t>(lt.wDay    % 10 + '0');
            mcl_time_s[12] = static_cast<wchar_t>(lt.wHour   / 10 + '0');
            mcl_time_s[13] = static_cast<wchar_t>(lt.wHour   % 10 + '0');
            mcl_time_s[15] = static_cast<wchar_t>(lt.wMinute / 10 + '0');
            mcl_time_s[16] = static_cast<wchar_t>(lt.wMinute % 10 + '0');
            mcl_time_s[18] = static_cast<wchar_t>(lt.wSecond / 10 + '0');
            mcl_time_s[19] = static_cast<wchar_t>(lt.wSecond % 10 + '0');
            mcl_time_s[21] = static_cast<wchar_t>(lt.wMilliseconds / 100     + '0');
            mcl_time_s[22] = static_cast<wchar_t>(lt.wMilliseconds / 10 % 10 + '0');
            mcl_time_s[23] = static_cast<wchar_t>(lt.wMilliseconds      % 10 + '0');
        }
        ::fwrite (mcl_time_s, sizeof (wchar_t), 25, mcl_logf_obj.f_u8log);
        
        // log level
        switch (level.value) {
            case cll4m.Int.value:    ::fwrite (L"[INT]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.Trace.value:  ::fwrite (L"[TRC]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.Debug.value:  ::fwrite (L"[DBG]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.Info.value:   ::fwrite (L"[INF]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.Warn.value:   ::fwrite (L"[WRN]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.Error.value:  ::fwrite (L"[ERR]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.Fatal.value:  ::fwrite (L"[FAT]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
            case cll4m.All.value:    MCL_FALLTHROUGH_CXX17
            case cll4m.Off.value:    MCL_FALLTHROUGH_CXX17
            default:                 ::fwrite (L"[ ? ]", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log); break;
        }
        
        // thread id
        ::fwprintf (mcl_logf_obj.f_u8log, L"[Tx%04lx]", ::GetCurrentThreadId ());
        
        // indent
        ::fputwc (' ', mcl_logf_obj.f_u8log);
        for (size_t n = mcl_logf_obj.dw_indent; n > 0; -- n)
            ::fwrite (L"    ", sizeof (wchar_t), 5, mcl_logf_obj.f_u8log);
    }
    
   /**
    * @function mcl_logbuf_t::mcl_ptchars <src/clog4m.cpp>
    * @brief Output log to file when buffer flushed.
    * @param {wchar_t*} begin: the starting position of the string
    * @param {wchar_t*} end: the next position at which the string ends
    * @return void
    */
    void mcl_logbuf_t::
    pt_chars (const wchar_t* begin, const wchar_t* end) noexcept{
    // In this function, the characters are parsed.
        if (begin == end) return ; 
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_logf_obj.nrtlock_obj);
        if (!mcl_logf_obj.f_u8log) return ;
        if (mcl_logf_obj.u_level > log_level_.value) return ;
        
        wchar_t const* c = begin;
        DWORD thread_id = ::GetCurrentThreadId ();
        if (thread_id != mcl_logf_obj.dw_lastthread_id) {
            mcl_logf_obj.dw_lastthread_id = thread_id;
            if (!mcl_logf_obj.b_newl && *c != '\n' && *c != '\r') {
                ::fputwc ('\n', mcl_logf_obj.f_u8log);
                mcl_ptnewline (log_level_);
            } 
        }
        for (; c != end; ++ c) {
            if (mcl_logf_obj.b_newl) {
                ::fwrite (begin, sizeof (wchar_t),
                    static_cast<size_t>(c - begin), mcl_logf_obj.f_u8log);
                begin = c;
                mcl_ptnewline (log_level_);
            } mcl_logf_obj.b_newl = (*c == '\n' || *c == '\r');
        } ::fwrite (begin, sizeof (wchar_t),
                static_cast<size_t>(c - begin), mcl_logf_obj.f_u8log);
    }
    
    
/**
 * @class mcl_clog4m_t <src/clog4m.h>
 * @brief An encapsulation class for clog4m_t
 */
    
   /**
    * @function mcl_clog4m_t::operator void* <src/clog4m.h>
    * @brief Returns true if log file is open.
    * @return {void*}: nullptr if not open
    */
    mcl_clog4m_t::
    operator void* () const noexcept{
        return mcl_logf_obj.f_u8log ?
            const_cast<mcl_clog4m_t*>(this) : 0;
    }

    /**
     * @function mcl_clog4m_t::operator! <src/clog4m.h>
     * @brief Returns false if log file is open.
     * @return {bool}
     */
    bool mcl_clog4m_t::
    operator! () const noexcept{
        return !mcl_logf_obj.f_u8log;
    }
    
    void* mcl_new_mcl_logbuf_t () {
        return new(std::nothrow) mcl_logbuf_t();
    }
    void mcl_del_mcl_logbuf_t (void** pbuf) {
        delete static_cast<mcl_logbuf_t*>(*pbuf);
        *pbuf = nullptr;
    }

    /**
     * @function mcl_logbuf_t::get_tl_obj <src/clog4m.cpp>
     * @brief Get pointer of current thread's logger
     * @return {mcl_logbuf_t*}
     */
    MCL_NODISCARD_CXX17 mcl_logbuf_t* mcl_logbuf_t::
    get_tl_obj () noexcept {
        thread_local mcl_logbuf_t mcl_tl_logbuf_obj;
        return mcl_base_obj.mcl_clog4m_after_exit
            && mcl_base_obj.mcl_threadid_after_exit == ::GetCurrentThreadId ()
            ? static_cast<mcl_logbuf_t*>(mcl_base_obj.mcl_clog4m_after_exit)
            : &mcl_tl_logbuf_obj;
    }

    /**
     * @function mcl_clog4m_t::operator[] <src/clog4m.h>
     * @brief Get a copy of the thread local logger
     * @param[in] {cll4m_t} logLevel
     * @return {clog4m_t}
     */
    clog4m_t mcl_clog4m_t::
    operator[] (cll4m_t logLevel) noexcept{
        mcl_logbuf_t::get_tl_obj ()
            -> log_level_ = logLevel;
        return clog4m_t{};
    }
    
   /**
    * @function mcl_get_sys_version <src/clog4m.cpp>
    * @brief Get Windows Information
    * @param[out] {ULONG} dwMajorVer
    * @param[out] {ULONG} dwMinorVer
    * @param[out] {ULONG} dwBuildNumber
    * @return {ULONG} code
    */
    MCL_NODISCARD_CXX17 static unsigned long
    mcl_get_sys_version (unsigned long& dwMajorVer,
        unsigned long& dwMinorVer, unsigned long& dwBuildNumber)
    noexcept {
    // Get windows system version
        HMODULE hModNtdll = ::LoadLibraryW(L"ntdll.dll");
        bool    ret = false;
        if (hModNtdll) {
            using pfRTLGETNTVERSIONNUMBERS = 
                VOID (WINAPI*) (DWORD*, DWORD*, DWORD*);
            pfRTLGETNTVERSIONNUMBERS pfRtlGetNtVersionNumbers;
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 4191)
#       endif // C4191: 'Function cast' : unsafe conversion.
            pfRtlGetNtVersionNumbers =
                reinterpret_cast<pfRTLGETNTVERSIONNUMBERS>(
                    ::GetProcAddress (hModNtdll, "RtlGetNtVersionNumbers"));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
            if (pfRtlGetNtVersionNumbers) {
                pfRtlGetNtVersionNumbers
                    (&dwMajorVer, &dwMinorVer, &dwBuildNumber);
                dwBuildNumber &= 0x0ffff;
                ret = true;
            }
            ::FreeLibrary (hModNtdll);
        }
        return ret ? 0ul : ::GetLastError ();
    } 
    
    /**
     * @function mcl_get_sys_localinfo <src/clog4m.cpp>
     * @brief Get System Bits & Current Time Zone
     * @param[out] {int} bRet: system bits
     * @param[out] {int} iTimeZone: current time zone
     * @return {ULONG} code
     */
    MCL_NODISCARD_CXX17 static unsigned long
    mcl_get_sys_localinfo (int& bRet, int& iTimeZone)
    noexcept{
        HMODULE hModKernel32 = ::GetModuleHandleW(L"kernel32");
        bool    ret          = false;
        if (hModKernel32) {
            // Get the number of system bits
            using LPFN_GetNativeSystemInfo =
                void (WINAPI*) (LPSYSTEM_INFO);
            LPFN_GetNativeSystemInfo fnGetNativeSystemInfo;
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 4191)
#       endif // C4191: 'Function cast' : unsafe conversion.
            fnGetNativeSystemInfo =
                reinterpret_cast<LPFN_GetNativeSystemInfo>(
                    ::GetProcAddress (hModKernel32 , "GetNativeSystemInfo"));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
            SYSTEM_INFO si;
            if (fnGetNativeSystemInfo) fnGetNativeSystemInfo (&si);
            else ::GetSystemInfo (&si);
            if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64
                || si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
                bRet = 64;
            else bRet = 32;
            
            // Get time zone
            using LPFN_GetDynamicTimeZoneInformation =
                DWORD (*) (PDYNAMIC_TIME_ZONE_INFORMATION);
            LPFN_GetDynamicTimeZoneInformation fnGetDynamicTimeZoneInformation;
#       ifdef _MSC_VER
#           pragma warning(push)
#           pragma warning(disable: 4191)
#       endif // C4191: 'Function cast' : unsafe conversion.
            fnGetDynamicTimeZoneInformation =
                reinterpret_cast<LPFN_GetDynamicTimeZoneInformation>(
                    ::GetProcAddress (hModKernel32 , "GetDynamicTimeZoneInformation"));
#       ifdef _MSC_VER
#           pragma warning(pop)
#       endif
            if (fnGetDynamicTimeZoneInformation) {
                DYNAMIC_TIME_ZONE_INFORMATION dtzi;
                fnGetDynamicTimeZoneInformation (&dtzi);
                iTimeZone = -(dtzi.Bias * 100l) / 60l;
                ret = true;
            } 
            ::FreeLibrary (hModKernel32);
        }
        return ret ? 0ul : ::GetLastError ();
    }
    
   /**
    * @function mcl_ptheader <src/clog4m.cpp>
    * @brief Print information at the begining of the log file.
    * @return void
    */
    static void
    mcl_ptheader () noexcept{
    // This is called to output operating environment.
        clog4m_t bf;
        
        // Get windows version number
        unsigned long dwMajorVer, dwMinorVer, dwBuildNumber, ret;
        ret = mcl_get_sys_version (dwMajorVer, dwMinorVer, dwBuildNumber);
        if (ret)
            bf[cll4m.Warn].wprintln
            (L"Unable to load 'ntdll.dll' module for function: "
                "'RtlGetNtVersionNumbers', error code %lu.", ret);
        
        // Get system bits & time zone
        int nbits, iTimeZone;
        ret = mcl_get_sys_localinfo (nbits, iTimeZone);
        if (ret)
            bf[cll4m.Warn].wprintln
            (L"Unable to load 'kernel32' module for function: "
                "'GetNativeSystemInfo', error code %lu.", ret);
        
        // Print log header
        bf[cll4m.Off].wprintln (
        L"{\"WinVer\":\"%lu.%lu Build %lu (%d-bit)\", "
          "\"TimeZone\":\"UTC%+05d\", "
          "\"MclVer\":\"%ls\"}\n", 
                dwMajorVer, dwMinorVer, dwBuildNumber, nbits,
                iTimeZone, mcl_simpletls_ns::mcl_name ()
        );
    }
    
   /**
    * @function mcl_clog4m_t::init <src/clog4m.cpp>
    * @brief Create a log file or a console.
    * @param directory_path: The path to the folder where the log file
    *     will be created. If nullptr, use console.
    * @return *this
    */
    mcl_clog4m_t& mcl_clog4m_t::
    init (wchar_t const* directory_path) noexcept{
    // This is called to log where threads open.
        {
            mcl_simpletls_ns::mcl_spinlock_t lock(mcl_logf_obj.nrtlock_obj);
            if (mcl_logf_obj.f_u8log) return *this;

            if (directory_path) { // create a new log file
                SYSTEMTIME lt;
                ::GetLocalTime(&lt);
                wchar_t temp[MAX_PATH];
                unsigned long i = 1;
                if (MCL_SNWPRINTF(temp, MAX_PATH - 1,
                    L"%ls%hu-%02hu-%02hu_%02hu-%02hu-%02hu.log",
                    directory_path, lt.wYear, lt.wMonth, lt.wDay, lt.wHour,
                    lt.wMinute, lt.wSecond
                ) == MAX_PATH - 1
                    ) return *this;
                while (::_waccess(temp, 0) == 0) {
                    if (MCL_SNWPRINTF(temp, MAX_PATH - 1,
                        L"%ls%hu-%02hu-%02hu_%02hu-%02hu-%02hu_(%lu).log",
                        directory_path, lt.wYear, lt.wMonth, lt.wDay, lt.wHour,
                        lt.wMinute, lt.wSecond, ++i
                    ) == MAX_PATH - 1
                        ) return *this;
                }
                MCL_WFOPEN(mcl_logf_obj.f_u8log, temp, L"w+t, ccs=UTF-8");
                if (!mcl_logf_obj.f_u8log) return *this;
            }
            else { // open the console
                mcl_logf_obj.b_console = true;
                ::AllocConsole();
                MCL_WFREOPEN(mcl_logf_obj.f_u8log, L"CONOUT$", L"w+t", stderr);
                if (!mcl_logf_obj.f_u8log) return *this;
                ::_wsystem(L"chcp 65001");
            }

            mcl_ptnewline(cll4m.Off);
            ::fwrite(L"mcl::clog4m::init  | LoggerStart \n", sizeof(wchar_t),
                //      12345678901234567890123456789012344
                34, mcl_logf_obj.f_u8log);
        } // !! not to re-enter the spin lock
        
        // Output operating environment
        mcl_ptheader ();
        return *this; 
    }

   /**
     * @function mcl_clog4m_t::init <src/clog4m.cpp>
     * @brief Create a log file or a console.
     * @param directory_path: The path to the folder where the log file
     *     will be created.
     * @return *this
     */
    mcl_clog4m_t& mcl_clog4m_t::
    init (char const* directory_path) noexcept{
        if (!directory_path) return init ();
        mcl_simpletls_ns::mcl_m2w_str_t wstr (directory_path);
        return init (wstr);
    }
    
   /**
    * @function mcl_clog4m_t::uninit <src/clog4m.h>
    * @brief Close the log file.
    * @return mcl_clog4m_t&
    */
    mcl_clog4m_t& mcl_clog4m_t::
    uninit (int code) noexcept{
        if (!mcl_logf_obj.f_u8log) return *this;
            
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_logf_obj.nrtlock_obj);
        if (!mcl_logf_obj.f_u8log) return *this;
        mcl_logf_obj.dw_indent = 0u;
        
        if (!mcl_logf_obj.b_newl) ::fputwc ('\n', mcl_logf_obj.f_u8log);
        mcl_ptnewline (cll4m.Off);
        ::fputwc ('\n', mcl_logf_obj.f_u8log);
        mcl_ptnewline (cll4m.Off);
        ::fwrite (L"mcl::clog4m::uninit  | LoggerCloseFile ",
            sizeof (wchar_t), 39, mcl_logf_obj.f_u8log);
        // string:  123456789012345678901234567890123456789
        // string: 0         1         2         3         
        wchar_t const* exp = nullptr;
        switch (code) {
            case 0:  exp = L"successful"; break;
            case -1: exp = L"abnormal termination"; break;
            case -2: exp = L"quick exit without resource clean-up"; break;
            default: exp = L"unknown code"; break;
        }
        ::fwprintf (mcl_logf_obj.f_u8log, L"{\"code\":\"%d\", \"debug\":\"%ls\"}\n", code, exp);

        unsigned long cnt = mcl_logf_obj.dw_cnt;
        if (cnt) {
            mcl_ptnewline (cll4m.Warn);
            ::fwprintf (mcl_logf_obj.f_u8log,
                L"  warning: %lu clog4m object destructor(s) were not"
                " called before uninit [-Wclog4m-memory-leak]\n", cnt );
            mcl_ptnewline (cll4m.Info);
            
            // string   123456789012345678901234567890123456789012344
            ::fwrite (L"  note: check that all threads end normally\n",
                sizeof (wchar_t), 44, mcl_logf_obj.f_u8log );
        }
        if (mcl_logf_obj.b_console) {
            mcl_ptnewline (cll4m.Off);
            ::_wsystem (L"pause");
            ::FreeConsole (); 
            mcl_logf_obj.b_console = false;
        } else {
            ::fclose (mcl_logf_obj.f_u8log);
        }
        mcl_logf_obj.f_u8log = nullptr;
        return *this; 
    }
    
   /**
    * @function mcl_clog4m_t::enable_event_level <src/clog4m.h>
    * @brief Set the event level for the log entry.
    * @param[in] logLevel: event level
    * @return enum cll4m_t
    */
    cll4m_t mcl_clog4m_t::
    enable_event_level (cll4m_t logLevel) noexcept{
        return cll4m_t{
            ::InterlockedExchange(&mcl_logf_obj.u_level, logLevel.value)
        };
    }

   /**
    * @function mcl_clog4m_t::get_event_level <src/clog4m.h>
    * @brief Get the event level for the log entry.
    * @return enum cll4m_t
    */
    cll4m_t mcl_clog4m_t::
    get_event_level () noexcept{
        return cll4m_t {mcl_logf_obj.u_level};
    }
    
    
/**
 * @class clog4m_t <src/clog4m.h>
 * @brief The actural work log stream objects
 */
    
   /**
    * @function clog4m_t::depth_incr <src/clog4m.h>
    * @brief Increase Log Indent.
    * @return *this
    */
    clog4m_t& clog4m_t::
    depth_incr () noexcept{
        if (!m_dataplus_) { 
            mcl_logbuf_t::get_tl_obj () -> bf_fail_ = true;
            return *this;
        }
        if (mcl_logf_obj.u_level >
            static_cast<mcl_logbuf_t*>(m_dataplus_) -> log_level_.value
            ) return *this;
        try { this -> flush (); } catch (...) {
            static_cast<mcl_logbuf_t*>(m_dataplus_)
                -> bf_fail_ = true;
        }
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_logf_obj.nrtlock_obj);
        if (mcl_logf_obj.dw_indent != 15) {
            ++ m_data_[0];
            ++ mcl_logf_obj.dw_indent;
        }
        if (!mcl_logf_obj.b_newl) {
            ::fputwc ('\n', mcl_logf_obj.f_u8log);
            mcl_logf_obj.b_newl = true;
        }
        return *this;
    }
    
   /**
    * @function clog4m_t::depth_decr <src/clog4m.h>
    * @brief Decrease Log Indent.
    * @return *this
    */
    clog4m_t& clog4m_t::
    depth_decr () noexcept{
        if (!(m_dataplus_ && m_data_[0])) { 
            mcl_logbuf_t::get_tl_obj() -> bf_fail_ = true;
            return *this;
        }
        try { this -> flush (); } catch (...) {
            static_cast<mcl_logbuf_t*>(m_dataplus_)
                -> bf_fail_ = true;
        }
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_logf_obj.nrtlock_obj);
        if (!mcl_logf_obj.b_newl) {
            ::fputwc ('\n', mcl_logf_obj.f_u8log);
            mcl_logf_obj.b_newl = true;
        }
        if (mcl_logf_obj.dw_indent != 0) {
            -- m_data_[0];
            -- mcl_logf_obj.dw_indent;
        } else {
            static_cast<mcl_logbuf_t*>(m_dataplus_)
                -> bf_fail_ = true;
        }
        return *this;
    }
    
   /**
    * @function clog4m_t::clog4m_t <src/clog4m.h>
    * @brief Constructor.
    * @return none
    */
    clog4m_t::clog4m_t () noexcept
    : std::basic_ostream< wchar_t, std::char_traits<wchar_t> >(
        static_cast<mcl_logbuf_t*>(
            m_dataplus_ = mcl_logbuf_t::get_tl_obj ()
    )), m_data_{ 0 } { } 
    
    mcl_logbuf_t::mcl_logbuf_t() noexcept
    : log_level_ (cll4m.All) {
        std::basic_streambuf<wchar_t, std::char_traits<wchar_t> >::
            setp (Cbuf_, Cbuf_ + buf_size);
        ::InterlockedIncrement (&mcl_logf_obj.dw_cnt);
    }
    
   /**
    * @function clog4m_t::clog4m_t() <src/clog4m.h>
    * @brief Copy constructor.
    * @param[in] rhs
    * @return none
    */
    clog4m_t:: clog4m_t  (clog4m_t const& rhs) noexcept
    : std::basic_ios< wchar_t, std::char_traits<wchar_t> >(),
      std::basic_ostream< wchar_t, std::char_traits<wchar_t> >(
        static_cast<mcl_logbuf_t*>(
            m_dataplus_ = mcl_logbuf_t::get_tl_obj ()
    )), m_data_{ 0 } {
        if (m_dataplus_)
            static_cast<mcl_logbuf_t*>(m_dataplus_) -> log_level_ =
                static_cast<mcl_logbuf_t*>(rhs.m_dataplus_) -> log_level_;
    }
    
   /**
    * @function clog4m_t::clog4m_t <src/clog4m.h>
    * @brief Move constructor.
    * @param[in] rhs
    * @return none
    */
    clog4m_t::clog4m_t (clog4m_t&& rhs) noexcept
    : std::basic_ios< wchar_t, std::char_traits<wchar_t> >(),
      std::basic_ostream< wchar_t, std::char_traits<wchar_t> >(
          static_cast<mcl_logbuf_t*>(
              m_dataplus_ = mcl_logbuf_t::get_tl_obj()
    )), m_data_{ 0 } {
        if (m_dataplus_)
            static_cast<mcl_logbuf_t*>(m_dataplus_) -> log_level_ =
                static_cast<mcl_logbuf_t*>(rhs.m_dataplus_) -> log_level_;
        rhs.m_dataplus_ = nullptr;
    }
    
   /**
    * @function clog4m_t::~clog4m_t <src/clog4m.h>
    * @brief Destructor.
    * @return none
    */
    clog4m_t::~clog4m_t () noexcept {
        if (!m_dataplus_) return ;
        try { this -> flush (); } catch (...) { }
        m_dataplus_ = nullptr;
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_logf_obj.nrtlock_obj);
        mcl_logf_obj.dw_indent -= m_data_[0];
        if (!mcl_logf_obj.b_newl) {
            ::fputwc ('\n', mcl_logf_obj.f_u8log);
            mcl_logf_obj.b_newl = true;
        }
    }
    mcl_logbuf_t::~mcl_logbuf_t () noexcept { 
        ::InterlockedDecrement (&mcl_logf_obj.dw_cnt);
    }
    
   /**
    * @function clog4m_t::operator! <src/clog4m.h>
    * @brief Test if the logger is available.
    * @return {bool}: True if no error occurred
    */
    bool clog4m_t::
    operator! () const noexcept{
        if (!m_dataplus_) return true;
        if (static_cast<mcl_logbuf_t*>(m_dataplus_) -> bf_fail_) {
            static_cast<mcl_logbuf_t*>(m_dataplus_) -> bf_fail_ = false;
            return true;
        }
        return !mcl_logf_obj.f_u8log || basic_ostream::fail ();
    }

   /**
    * @function clog4m_t::operator void* <src/clog4m.h>
    * @brief Test if the logger is available.
    * @return {void*}: True if error occurred
    */
    clog4m_t::
    operator void* () const noexcept{
        return !*this ?
             nullptr : const_cast<clog4m_t*>(this);
    }
    
   /**
    * @function clog4m_t::operator[] <src/clog4m.h>
    * @brief Set log level for logger for current thread.
    * @return *this
    */
    clog4m_t& clog4m_t::
    operator[] (cll4m_t logLevel) noexcept{
        if (!m_dataplus_) { 
            mcl_logbuf_t::get_tl_obj () -> bf_fail_ = true;
            return *this;
        }
        static_cast<mcl_logbuf_t*>(m_dataplus_)
            -> log_level_ = logLevel; 
        return *this;
    } 
    
   /**
    * @function clog4m_t::putchar <src/clog4m.h>
    * @param[in] {int} ch: char that needs to be output
    * @return int: passed char if successful; EOF if failed
    */
    int clog4m_t::
    putchar (int ch) noexcept{
        if (!m_dataplus_) return EOF;
        try { this -> put (static_cast<wchar_t>(ch)); }
        catch (...) { return EOF; }
        return ch;
    }

   /**
    * @function clog4m_t::putwchar <src/clog4m.h>
    * @param[in] {wchar_t} ch: char that needs to be output
    * @return wint_t: passed char if successful; WEOF if failed
    */
    wint_t clog4m_t::
    putwchar (wchar_t ch) noexcept{
        if (!m_dataplus_) return WEOF;
        try { this -> put (static_cast<wchar_t>(ch)); }
        catch (...) { return WEOF; }
        return ch;
    }
    
   /**
    * @function clog4m_t::puts <src/clog4m.h>
    * @param[in] str: string that needs to be output
    * @return int: string length (including a terminating null
    *     character) if successful; EOF if errors occur.
    */
    int clog4m_t::
    puts (char const* str) noexcept{
        if (!m_dataplus_ || !str) return EOF;
        mcl_simpletls_ns::mcl_m2w_str_t wstr (str);
        try { this -> write (wstr,
            static_cast<std::streamsize>(wstr.len ())). put ('\n'); }
        catch (...) { return EOF; }
        return static_cast<int>(wstr.len () + 1);
    }

    /**
     * @function clog4m_t::putws <src/clog4m.h>
     * @param[in] str: string that needs to be output
     * @return int: string length (including a terminating null
     *     character) if successful; EOF if errors occur.
     */
    int clog4m_t::
    putws (wchar_t const* str) noexcept{
        if (!m_dataplus_ || !str) return EOF;
        std::streamsize nch =
            static_cast<std::streamsize>(::wcslen(str));
        try { this -> write (str, nch). put ('\n'); }
        catch (...) { return EOF; }
        return static_cast<int>(nch + 1);
    }
    
   /**
    * @function clog4m_t::printf <src/clog4m.h>
    * @param[in] format
    * @param[in] {va_list} ...
    * @return int: string length (not counting the terminating
    *     null character) if successful; EOF if errors occur.
    */
    int clog4m_t::
    printf (char const* format, ...) noexcept{
        va_list arg_list;       va_start (arg_list, format);
        int ret = this -> vprintf (format, arg_list);
        va_end (arg_list);      return ret;
    }

    /**
     * @function clog4m_t::wprintf <src/clog4m.h>
     * @param[in] format
     * @param[in] {va_list} ...
     * @return int: string length (not counting the terminating
     *     null character) if successful; EOF if errors occur.
     */
    int clog4m_t::
    wprintf (wchar_t const* format, ...) noexcept{
        va_list arg_list;       va_start (arg_list, format);
        int ret = this -> vwprintf (format, arg_list);
        va_end (arg_list);      return ret;
    }
    
   /**
    * @function clog4m_t::println <src/clog4m.h>
    * @param[in] format
    * @param[in] {va_list} ...
    * @return *this
    */
    clog4m_t& clog4m_t::
    println (char const* format, ...) noexcept{
        va_list arg_list;       va_start (arg_list, format);
        this -> vprintln (format, arg_list);
        va_end (arg_list);      return *this;
    }

    /**
     * @function clog4m_t::wprintln <src/clog4m.h>
     * @param[in] format
     * @param[in] {va_list} ...
     * @return *this
     */
    clog4m_t& clog4m_t::
    wprintln (wchar_t const* format, ...) noexcept{
        va_list arg_list;       va_start (arg_list, format);
        this -> vwprintln (format, arg_list);
        va_end (arg_list);      return *this;
    }
    
   /**
    * @function clog4m_t::vprintf <src/clog4m.h>
    * @param[in] format
    * @param[in] {va_list} arg_list
    * @return int: string length (not counting the terminating
    *     null character) if successful, EOF if errors occur.
    */
    int clog4m_t::
    vprintf (char const* format, va_list arg_list) noexcept{
        if (!format || !m_dataplus_) return EOF;
        int nch = ::vsnprintf (nullptr, 0, format, arg_list);
        if (!nch) return 0;
        if (nch < 0) return EOF;
        
        mcl::mcl_simpletls_ns::mcl_auto_ptr_t<char>
            str (static_cast<size_t>(nch) + 1);
        if (!str) return EOF;
        nch = ::vsnprintf (str, static_cast<size_t>(nch) + 1, format, arg_list);
        if (nch <= 0) return EOF;
        
        mcl_simpletls_ns::mcl_m2w_str_t wstr (str, str + nch);
        try { this -> write (wstr, static_cast<std::streamsize>(wstr.len ())); }
        catch (...) { return EOF; }
        return static_cast<int>(wstr.len ());
    }

    /**
     * @function clog4m_t::vwprintf <src/clog4m.h>
     * @param[in] format
     * @param[in] {va_list} arg_list
     * @return int: string length (not counting the terminating
     *     null character) if successful, EOF if errors occur.
     */
    int clog4m_t::
    vwprintf (wchar_t const* format, va_list arg_list) noexcept{
        if (!format || !m_dataplus_) return EOF;
        int nch = MCL_VSNWPRINTF(nullptr, 0, format, arg_list);
        if (!nch) return 0;
        if (nch < 0) return EOF;

        mcl::mcl_simpletls_ns::mcl_auto_ptr_t<wchar_t>
            str (static_cast<size_t>(nch) + 1);
        if (!str) return EOF;
        nch = MCL_VSNWPRINTF(str, static_cast<size_t>(nch) + 1, format, arg_list);
        if (nch <= 0) return EOF;
        
        try { this -> write (str, nch); }
        catch (...) { return EOF; }
        return nch;
    }
    
   /**
    * @function clog4m_t::vprintln <src/clog4m.h>
    * @param[in] format
    * @param[in] {va_list} arg_list
    * @return *this
    */
    clog4m_t& clog4m_t::
    vprintln (char const* format, va_list arg_list) noexcept{
        if (!m_dataplus_) return *this;
        this -> vprintf (format, arg_list);
        try { this -> put ('\n'); return *this; }
        catch (...) { }
        if (!m_dataplus_) return *this;
        static_cast<mcl_logbuf_t*>(m_dataplus_) -> bf_fail_ = true;
        return *this;
    }
    
    /**
     * @function clog4m_t::vwprintln <src/clog4m.h>
     * @param[in] format
     * @param[in] {va_list} arg_list
     * @return *this
     */
    clog4m_t& clog4m_t::
    vwprintln (wchar_t const* format, va_list arg_list) noexcept{
        if (!m_dataplus_) return *this;
        this -> vwprintf (format, arg_list);
        try { this -> put('\n'); return *this; }
        catch (...) { }
        if (!m_dataplus_) return *this;
        static_cast<mcl_logbuf_t*>(m_dataplus_) -> bf_fail_ = true;
        return *this;
    }
    
   /**
    * @function clog4m_t::separator <src/clog4m.h>
    * @brief Start a new line if it is not at the
    *     beginning of the line, and then output
    *     33 '-' and a '\\n'
    * @return *this
    */
    clog4m_t& clog4m_t::
    separator () noexcept { 
        try { this -> flush (); }
        catch (...) { return *this; }
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_logf_obj.nrtlock_obj);
        if (!mcl_logf_obj.f_u8log || !m_dataplus_) {
            mcl_logbuf_t::get_tl_obj() -> bf_fail_ = true;
            return *this;
        }
        if (mcl_logf_obj.u_level > static_cast<mcl_logbuf_t*>(
          m_dataplus_) -> log_level_.value ) {
            return *this;
        }
        if (!mcl_logf_obj.b_newl) {
            ::fputwc ('\n', mcl_logf_obj.f_u8log);
            mcl_logf_obj.b_newl = true;
        }
        mcl_ptnewline (static_cast<mcl_logbuf_t*>(m_dataplus_) -> log_level_); 
        ::fwrite (L"---------------------------------\n",
        // string:  12345678901234567890123456789012344
            sizeof (wchar_t), 34, mcl_logf_obj.f_u8log);
        return *this;
   }
    
}
