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
    
    @file src/mclfwd.h
    This is a C++11 header that contains forward declarations of mclib.
*/


#ifndef MCL_MCLFWD
# define MCL_MCLFWD

/*
    Mclib internally uses many syntax and features under the 
    C++ 11 standard. In order to ensure compilation reliability,
    so you must use a compiler that supports the C++ 11 standard,
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

# include <cstdlib>

namespace
mcl {

   /**
    * @defgroup mclib
    * @brief Moudles forward declaration 
    * @{
    */
    
    // proxy class for mclib.h
    template <typename... T> class pytuple;
    struct mcl_version_t;
    
    // class for log file streams.  see clog4m.h
    class clog4m_t; 
    class mcl_clog4m_t;
    
    // type for basic color models.  see colors.h
    using color_t = unsigned long;
    using colorcv_t = unsigned char; // components of color models
    
    // class for graphics interface control.  see display.h
    using flag32_t = unsigned long;
    class mcl_display_t;
    class wmi_dict_t;
    
    // class for interacting with events and queues.  see event.h
    class mcl_event_t;
    
    // class for loading and playing sounds.  see mixer.h
    class mcl_mixer_t;
    
    // class for working with the mouse.  see mouse.h
    using point1d_t = long;  // coordinates that drawn to scale
    class point2d_t { public: point1d_t x, y; };
    class mcl_mouse_t;    
    
    // class for monitoring time.  see timer.h
    class mcl_time_t;
        
/** @}  */


} // namespace

# endif  // __cplusplus
#endif   // MCL_MCLFWD
