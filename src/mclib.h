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
    
    @file src/mclib.h
    This is a C++11 header that contains most headers of mclib.
    If you don't need some of these files, please manually
    include the required files.
*/


#ifndef MCL_MCLIB
# define MCL_MCLIB

/*
    Mclib internally uses many syntax and features under the 
    C++ 11 standard. In order to ensure compilation reliability,
    so you must use a compiler that supports the C++11 standard,
    or you need to enable it with the -std=c++11 or -std=gnu++11
    compiler options.
*/
# ifndef __cplusplus
#   error 'mclib.h' is only for C++. You must use C++\
 compiler, or you need filename with '.cpp' suffix.

# elif __cplusplus < 201103L
#   error This header requires compiler and library support\
 for the ISO C++ 2011 standard. This support is currently\
 experimental, and must be enabled with the -std=c++11 or\
 -std=gnu++11 compiler options.

# else

// @{
//  This header is for mclib standard log output stream.
#   include "clog4m.h"

//  This header is for color models.
#   include "colors.h"

//  This header is for mclib standard graphics control.
#   include "display.h"

//  This header is for drawing shapes.
#   include "draw.h"

//  This header is for interacting with events and queues.
#   include "event.h"

//  This header is for working with the mouse.
#   include "mouse.h"

//  This header is for working with the keyboard.
#   include "key.h"

//  This header is for cursor resources.
#   include "cursors.h"

//  This header is for image transfer.
#   include "image.h"

//  This header is for surfaces transfer.
#   include "transform.h"

//  This header is for loading and playing sounds.
//  inline include music.h
#   include "mixer.h"

//  This header is for representing any image.
//  inline include bufferproxy.h
#   include "surface.h"

//  This header is for monitoring time.
#   include "timer.h"

//  This header is for data types in python.
#   include "pyobj.h"
// @}


#   ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable: 4365)
#   endif

#   include <functional>

#   ifdef _MSC_VER
#    pragma warning(pop)
#   endif

namespace
mcl {

   /**
    * @class mclib 
    * @ingroup mclib
    * @{
    */
    // Initialize all mclib modules
    pytuple<int, int> init    () noexcept;

    // Uninitialize all mclib modules. 
    void              quit    () noexcept;

    // Returns True if any mclib module is currently initialized
    bool              get_init () noexcept;

    // Register a function of any type to be called when mclib quits.
    // This function is defined elsewhere in this file.
//  bool              register_quit (function, parms...) noexcept;

    // Small module containing version information
    struct mcl_version_t {
        static char const* const            ver_a;
        static wchar_t const* const         ver;
        static pytuple<int, int, int> const vernum;
        static int const                    rev;
    };
    extern mcl_version_t version; // Small module containing version information
    

    bool mcl_register_quit (std::function<void()>) noexcept;
    // Register a function of any type to be called when mclib quits.
    template<class Fun, typename... Args>
    bool constexpr register_quit (Fun&& f, Args&&... args) noexcept
    { return mcl_register_quit ([&f, &args...]{ f (args...); }); }
    // Register a member function of any type to be called when mclib quits.
    template<class Cls, class Fun, typename... Args>
    bool constexpr register_quit (Fun&& f, Cls& c, Args&&... args) noexcept
    { return mcl_register_quit ([&f, &c, &args...]{ (c.*f) (args...); }); }
    
    /** @}  */
    
}

# endif  // __cplusplus
#endif   // MCL_MCLIB
