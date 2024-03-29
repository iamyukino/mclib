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
    
    @file cpp/mcl_base.h
    This is a C++11 inside stdafx header.
*/


#ifndef MCL_MCLBASE
# define MCL_MCLBASE

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4464)
# endif // Relative paths include ".."

# include "../src/mclfwd.h"

#ifdef _MSC_VER
#  pragma warning(pop)
#  pragma warning(push)
#  pragma warning(disable: 4365 5039)
# endif

# include <tchar.h>
# include <functional>
# include <utility>
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <windowsx.h>
# include <process.h>
# include <shellapi.h>
# undef WIN32_LEAN_AND_MEAN

# ifdef _MSC_VER
#  pragma warning(pop)
#  pragma warning(disable: 4514)
# endif

// define

# if __cplusplus < 201703L
#  define MCL_NODISCARD_CXX17
#  define MCL_FALLTHROUGH_CXX17
# else
#  define MCL_NODISCARD_CXX17 [[nodiscard]]
#  define MCL_FALLTHROUGH_CXX17 [[fallthrough]];
# endif

# ifdef _MSC_VER
#  define MCL_GETTICKCOUNT() (::GetTickCount64 ())
#  define MCL_TCSNCPY(de, sc, cnt) (::_tcsncpy_s (de, cnt, sc, cnt))
#  define MCL_STRNCPY(de, sc, cnt) (::strncpy_s (de, cnt, sc, cnt))
#  define MCL_WCSNCPY(de, sc, cnt) (::wcsncpy_s (de, cnt, sc, cnt))
#  define MCL_WFOPEN(file, name, mode) \
      (static_cast<FILE*>(::_wfopen_s (&(file), name, mode) ? ((file) = nullptr) : (file)))
#  define MCL_WFREOPEN(file, name, mode, s) \
      (static_cast<FILE*>(::_wfreopen_s (&(file), name, mode, s) ? ((file) = nullptr) : (file)))
#  define MCL_SNWPRINTF(buf, cnt, fmt, ...) \
      (cnt ? ::_snwprintf_s (buf, cnt, cnt, fmt, __VA_ARGS__) : ::_scwprintf (fmt, __VA_ARGS__))
#  define MCL_VSNWPRINTF(buf, cnt, fmt, list) \
      (cnt ? ::_vsnwprintf_s (buf, cnt, cnt, fmt, list) : ::_vscwprintf (fmt, list))
#  define MCL_TGETFILENAME(path, f) \
      (::_tsplitpath_s (path, nullptr, 0, nullptr, 0, f, _MAX_FNAME, nullptr, 0))
#  define MCL_WGETFILEEXT(path, f) \
      (::_wsplitpath_s (path, nullptr, 0, nullptr, 0, nullptr, 0, f, _MAX_EXT))
# else
#  define MCL_GETTICKCOUNT() (::GetTickCount ())
#  define MCL_TCSNCPY(de, sc, cnt) (::_tcsncpy (de, sc, cnt))
#  define MCL_STRNCPY(de, sc, cnt) (::strncpy (de, sc, cnt))
#  define MCL_WCSNCPY(de, sc, cnt) (::wcsncpy (de, sc, cnt))
#  define MCL_WFOPEN(file, name, mode) ((file) = ::_wfopen (name, mode))
#  define MCL_WFREOPEN(file, name, mode, s) ((file) = ::_wfreopen (name, mode, s))
#  define MCL_SNWPRINTF(buf, cnt, fmt, ...)    (::_snwprintf (buf, cnt, fmt, __VA_ARGS__))
#  define MCL_VSNWPRINTF(buf, cnt, fmt, list) (::_vsnwprintf (buf, cnt, fmt, list))
#  define MCL_TGETFILENAME(path, f) (::_tsplitpath (path, nullptr, nullptr, f, nullptr), 0)
#  define MCL_WGETFILEEXT(path, f) (::_wsplitpath (path, nullptr, nullptr, nullptr, f), 0)
# endif

namespace mcl
{   
    namespace mcl_simpletls_ns
    {
    
   /**
    * @function mcl_name <cpp/mcl_base.h>
    * @brief mclib version information
    */
    constexpr char const*    mcl_version_number_a (){ return        "0.5.2";            }
    constexpr wchar_t const* mcl_version_number ()  { return       L"0.5.2";            }
    constexpr char const*    mcl_name_a ()          { return  "mclib 0.5.2-Debug.3472"; }
    constexpr wchar_t const* mcl_name ()            { return L"mclib 0.5.2-Debug.3472"; }
    constexpr int            mcl_major_number ()    { return         0; }
    constexpr int            mcl_minor_number ()    { return           5; }
    constexpr int            mcl_patch_number ()    { return             2; }
    constexpr int            mcl_revision_number () { return                     3472; }
    
   /**
    * @class mcl_spinlock_t <cpp/mcl_base.h>
    * @brief spinlock
    */
    class
    mcl_spinlock_t {
    public:
        using lock_t = unsigned long;
        mcl_spinlock_t (lock_t& lk, wchar_t const* name, unsigned long uWaitMs = 256ul) noexcept;
        mcl_spinlock_t& operator= (mcl_spinlock_t&) = delete;
        ~mcl_spinlock_t () noexcept;
    private: lock_t& lk_; bool islock_;
           char : 8; char : 8; char : 8; char : 8; char : 8;
           char : 8; char : 8;
    };
    void mcl_lock (mcl_spinlock_t::lock_t& lk_, unsigned& m_nrt_count) noexcept;
    void mcl_unlock (mcl_spinlock_t::lock_t& lk_, unsigned& m_nrt_count) noexcept;
    
    
   /**
    * @class mcl_auto_ptr_t <cpp/mcl_base.h>
    * @brief auto pointer
    */
    template <typename T>
    class
    mcl_auto_ptr_t {
        T* m_ptr;
    public:
        explicit mcl_auto_ptr_t (size_t sz) noexcept
          : m_ptr (new (std::nothrow) T[sz]) { }
        ~mcl_auto_ptr_t () noexcept{ delete[] m_ptr; }
        constexpr bool operator! () const noexcept { return !m_ptr; }
        constexpr operator T*() const noexcept{ return m_ptr; }
        constexpr T* ptr() const noexcept{ return m_ptr; }
    };

   /**
    * @class mcl_shared_ptr_t <cpp/mcl_base.h>
    * @brief shared pointer
    */
    template <typename T>
    class
    mcl_shared_ptr_t {
        T m_ptr;
        unsigned long* ref_cnt;
        std::function<void()> fun_del;
    public:
        explicit mcl_shared_ptr_t (T rptr, std::function<void()> fun)
            noexcept : m_ptr(rptr), ref_cnt (new (std::nothrow)
                unsigned long(1)), fun_del (fun) {
            if (!m_ptr) { if (ref_cnt) delete ref_cnt; ref_cnt = 0; }
        }
        void add () noexcept { if (ref_cnt && *ref_cnt) ++ *ref_cnt; }
        bool del () noexcept{
            if (ref_cnt && *ref_cnt) if (!-- *ref_cnt)
            { fun_del(); delete ref_cnt; ref_cnt = 0; m_ptr = 0; return true; }
            return false;
        }
        constexpr bool operator! () const noexcept { return !m_ptr; }
        constexpr operator T() const noexcept{ return m_ptr; }
        constexpr T ptr() const noexcept{ return m_ptr; }
    };
    
    
   /**
    * @class mcl_m2w_str_t <cpp/mcl_base.h>
    * @brief cast from string to wstring
    */
    class
    mcl_m2w_str_t {
        size_t len_; // must be the first
        mcl_auto_ptr_t<wchar_t> auto_ptr_;
    public:
        explicit mcl_m2w_str_t (char const* begin,
            char const* end = nullptr) noexcept;
        explicit mcl_m2w_str_t (wchar_t const* begin,
            wchar_t const* end = nullptr) noexcept;
        ~mcl_m2w_str_t () noexcept{ }
        inline operator wchar_t* () const noexcept{ return auto_ptr_; }
        inline size_t len () const noexcept { return len_; }
    };
    class
    mcl_w2m_str_t {
        size_t len_; // must be the first
        mcl_auto_ptr_t<char> auto_ptr_;
    public:
        explicit mcl_w2m_str_t (char const* begin,
            char const* end = nullptr) noexcept;
        explicit mcl_w2m_str_t (wchar_t const* begin,
            wchar_t const* end = nullptr) noexcept;
        ~mcl_w2m_str_t () noexcept{ }
        inline operator char* () const noexcept{ return auto_ptr_; }
        inline size_t len () const noexcept { return len_; }
    };
    
    
   /**
    * @function bind_mf <cpp/mcl_base.h>
    * @brief bind member function to callback function
    */
    template <int id, typename> struct mcl_callback_t;
    template <int id, typename Ret, typename Cls, typename... Params>
    struct mcl_callback_t<id, Ret(Cls::*)(Params...)> {
        template <typename... Args>
        static Ret callback(Args... args)
        { return (cls->*fun) (std::forward<decltype(args)>(args)...); }
        static Ret(Cls::* fun)(Params...);
        static Cls* cls;
    };
    template <int id, typename Ret, typename Cls, typename... Params>
    Ret(Cls::* mcl_callback_t<id, Ret(Cls::*)(Params...)>::fun)(Params...);
    template <int id, typename Ret, typename Cls, typename... Params>
    Cls* mcl_callback_t<id, Ret(Cls::*)(Params...)>::cls;

#   define MCL_DEF_BIND_MF(...) \
    template <int id, typename Ret, typename Cls, typename... Params> \
    MCL_NODISCARD_CXX17 Ret (* \
        bind_mf (Ret(Cls::*fun)(Params...) __VA_ARGS__, Cls* cls) noexcept \
    ) (Params...) \
    { \
        using NewType = Ret(Params...); \
        using OldType = Ret(Cls::*)(Params...); \
        mcl_callback_t<id, OldType>::fun = fun; \
        mcl_callback_t<id, OldType>::cls = cls; \
        return static_cast<NewType*>(mcl_callback_t<id, OldType>::callback); \
    }

    MCL_DEF_BIND_MF()
    MCL_DEF_BIND_MF(const)
    MCL_DEF_BIND_MF(volatile)
    MCL_DEF_BIND_MF(const volatile)
#   if __cplusplus >= 201703L
    // mangled name will change in C++17 because the exception
    // specification is part of a function type
    MCL_DEF_BIND_MF(noexcept)
    MCL_DEF_BIND_MF(const noexcept)
    MCL_DEF_BIND_MF(volatile noexcept)
    MCL_DEF_BIND_MF(const volatile noexcept)
#   endif
#   undef MCL_DEF_BIND_MF

    
    } // namespace
    
    
    class mcl_base_t {
    public:
        mcl_base_t () noexcept;
        long long frequency;
    public:
        void* mcl_clog4m_after_exit = nullptr;
        unsigned mcl_threadid_after_exit = 0;
        char : 8; char : 8; char : 8; char : 8;

    public:
        size_t atquit_registered = 0;
        size_t atquit_table_size = 0;
        std::function<void()>* atquit_table = nullptr;

    public: // non-reentrant spinlock 
        typename mcl_simpletls_ns:: // for display
            mcl_spinlock_t::lock_t nrtlock = 0ul;
        typename mcl_simpletls_ns:: // for atquit
            mcl_spinlock_t::lock_t quitlock = 0ul;
        typename mcl_simpletls_ns:: // for set_timer
            mcl_spinlock_t::lock_t timerlock = 0ul;
        typename mcl_simpletls_ns:: // for drop_file
            mcl_spinlock_t::lock_t droplock = 0ul;
        typename mcl_simpletls_ns:: // for keymap
            mcl_spinlock_t::lock_t keymaplock = 0ul;
        char : 8; char : 8; char : 8; char : 8;
    };
    extern mcl_base_t mcl_base_obj;
    
}

#endif
