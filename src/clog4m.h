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
    
    @file src/clog4m.h
    This is a C++11 header for logging.
*/


#ifndef MCL_CLOG4M
# define MCL_CLOG4M

# include "mclfwd.h"
# include <type_traits>
# include <vadefs.h>   // for C variadic parameter lists

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4365)
# endif

# include <iosfwd>
# include <ostream>    // for template class 'basic_ostream'

# ifdef _MSC_VER
#  pragma warning(pop)
# endif

// no define

namespace
mcl {

   /**
    * @enum logLevel <src/clog4m.h>
    * @brief An api logger for mclib
    * 
    * @ingroup logger
    * @ingroup mclib
    * @ingroup debug
    * 
    *  mcl::clog4m write a log entry to a log only if
    *  the event level set for the logger is greater
    *  than or equal to the event level for the log
    *  entry. The following enum lists the event levels
    *  that mclib supports and the manifest constants
    *  that represent these event levels.  logInt is
    *  the lowest event level, followed by the next
    *  highest event level logTrace, and so on. logFatal
    *  is the highest event level. logAll and logOff
    *  are only for opening or closing logs.
    */
    enum 
    mcl_loglevel_enum
    : unsigned {
    
        logAll,
          // For opening all logs
       
        logInt,       // internal (buffered)
          // For mclib internal function, it is generally
          // used in the mclib functions. 
         
        logTrace,     // trace (buffered)
          // Designates finer-grained informational events
          // than the logDebug.
        
        logDebug,     // debug (buffered)
          // For function developers, it is generally used
          // in the function development stage to check
          // whether the function is normal for developers.
        
        logInfo,      // information (buffered)
          // The internal beta version can output this log
          // to check whether the newly developed features
          // or bug fix are normal.
        
        logWarn,      // warning (unbuffered)
          // Logs above this level will be set to unbuffered.
          // Abnormal processes that will not affect the
          // system.
        
        logError,     // error (unbuffered) 
          // Abnormal processes that will affect the system
          // can be self repaired without affecting the
          // stability of the system.
            
        logFatal,     // fatal error (unbuffered)
          // An irreversible error has occurred and the
          // system cannot work normally.
        
        logOff,
          // For closing all logs    
    };
    
    
    
    
   /**
    * @class clog4m_t <src/clog4m.h>
    * @brief The actural work log stream objects
    * 
    * @ingroup logger
    * @ingroup mclib
    * @ingroup debug
    * 
    *  This is the class for the mclib api log output stream.
    *  It inherits from ostream, so any function provided in
    *  std::ostream (std::cout) can be used in this class.
    *  At the same time, this output stream also provides
    *  C-style output and other convenience loging functions.
    *  
    *  They are NOT required by default to cooperate with the
    *  global C++ library's @cpp output streams, but to be
    *  available during program startup and termination, while
    *  ensuring thread safety and nothrow guarantee.
    *  
    *  It is preferable to initialize clog4m_t objects with
    *  the return value of clog4m[logLevel] to ensure thread
    *  safety. At the same time, it is preferable to utilize a
    *  distinct clog4m_t object for each thread.
    */
    class clog4m_t
      : public std::basic_ostream
      <wchar_t, std::char_traits<wchar_t> > {
    
    public:
        explicit   clog4m_t   () noexcept;
                   clog4m_t   (clog4m_t const& rhs) noexcept;
                   clog4m_t   (clog4m_t&& rhs) noexcept;
                  ~clog4m_t   () noexcept;
        clog4m_t&  operator=  (clog4m_t const& rhs) = delete;
        clog4m_t&  operator=  (clog4m_t&& rhs) = delete;
        
        operator   void*      () const noexcept;
        bool       operator!  () const noexcept; 
        clog4m_t&  operator[] (mcl_loglevel_enum logLevel) noexcept;
        
        clog4m_t&  depth_incr () noexcept; 
        clog4m_t&  depth_decr () noexcept; 
        int        putchar    (int ch) noexcept;
        wint_t     putwchar   (wchar_t ch) noexcept;
        int        puts       (char const* str) noexcept;
        int        putws      (wchar_t const* str) noexcept;
        int        printf     (char const* format, ...) noexcept;
        int        wprintf    (wchar_t const* format, ...) noexcept;
        int        vprintf    (char const* format, va_list arg_list) noexcept;
        int        vwprintf   (wchar_t const* format, va_list arg_list) noexcept;
        clog4m_t&  println    (char const* format, ...) noexcept;
        clog4m_t&  wprintln   (wchar_t const* format, ...) noexcept;
        clog4m_t&  vprintln   (char const* format, va_list arg_list) noexcept;
        clog4m_t&  vwprintln  (wchar_t const* format, va_list arg_list) noexcept;
        clog4m_t&  separator  () noexcept;
        
    private:
        char m_data_[1];
        void* m_dataplus_;
    };
    
    
    
    
   /**
    * @class mcl_clog4m_t <src/clog4m.h>
    * @brief An encapsulation class for clog4m_t
    *
    * @ingroup logger
    * @ingroup mclib
    * @ingroup debug
    *
    *  This is an encapsulation class for clog4m_t.
    *  For more information, see the section of the manual
    *  linked to above.  Here are examples:
    *  
    *  @code
    *      clog4m[logInt] << "Hello!";
    *      // will end up with a newline and flush here
    *      clog4m_t logger;
    *      logger[logInt].wprintf (L"This is %c ", 'a');
    *      logger[logInt].println ("test of clog4m.");
    *      logger[logInt] << std::flush;
    *      // will flush here
    *  @endcode
    *  
    *  @code
    *      clog4m[logInt] << "Hello!";
    *        // is same as
    *      clog4m[logInt] << "Hello!\n";
    *        // or
    *      clog4m_t logger;
    *      logger[logInt] << "Hello!";
    *      logger[logInt] << std::endl;
    *        // or
    *      {
    *          clog4m_t logger;
    *          logger[logInt] << "Hello!";
    *      }
    *  @endcode
    */
    class
    mcl_clog4m_t {
    
    public:
                      mcl_clog4m_t () = default;
                      mcl_clog4m_t (mcl_clog4m_t const& rhs) = delete;
                      mcl_clog4m_t (mcl_clog4m_t&& rhs) = delete;
                     ~mcl_clog4m_t () = default;
        mcl_clog4m_t& operator=    (mcl_clog4m_t&) = delete;
        
        operator      void*        () const noexcept;
        bool          operator!    () const noexcept;
        clog4m_t      operator[]   (mcl_loglevel_enum logLevel) noexcept;
        
        mcl_clog4m_t& init         (char const* directory_path) noexcept;
        mcl_clog4m_t& init         (wchar_t const* directory_path = nullptr) noexcept;
        mcl_clog4m_t& uninit       () noexcept;
        inline mcl_clog4m_t& is_init () noexcept{ return *this; }
        
        mcl_loglevel_enum enable_event_level (mcl_loglevel_enum logLevel) noexcept;
        mcl_loglevel_enum get_event_level    () noexcept;
    
    };
    extern mcl_clog4m_t clog4m;
    
} // namespace

#endif // MCL_CLOG4M
