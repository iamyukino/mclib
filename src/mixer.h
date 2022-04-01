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
    
    @file src/mixer.h
    This is a C++11 header for loading and playing sounds.
*/


#ifndef MCL_MIXER
# define MCL_MIXER

# include "mclfwd.h"

// no define

namespace
mcl {
    
   /**
    * @class mcl_mixer_t
    * @brief Module for loading and playing sounds
    * 
    * @ingroup mixer
    * @ingroup mclib
    * @{
    */
    class
    mcl_mixer_t { public:
        
        class mcl_music_t;
        static mcl_music_t music;
    };
   /** @}  */


  /**
   * @name Mclib Music Playing Objects
   * @{
   */
    extern mcl_mixer_t mixer;
    
/** @}  */
    
} // namespace

# include "music.h" 

#endif // MCL_MIXER
