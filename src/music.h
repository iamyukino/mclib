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
    
    @file src/music.h
    This is a C++11 header for controling streamed audio.
*/

#ifndef MCL_MIXER
#  error Never use "src\music.h" directly; include\
 "src\mixer.h" instead.
#else

# ifndef MCL_MUSIC
#  define MCL_MUSIC

// no include
// no define

namespace
mcl {
    
   /**
    * @class mcl_mixer_t::mcl_music_t <src/music.h>
    * @brief Module for controlling streamed audio.
    * 
    * @ingroup mixer
    * @ingroup mclib
    * @{
    */
    class mcl_mixer_t::mcl_music_t { public:
        
        explicit     mcl_music_t () = default;
                    ~mcl_music_t () noexcept;
                     mcl_music_t (mcl_music_t const& rhs) = delete;
        mcl_music_t& operator=   (mcl_music_t const& rhs) = delete;
        operator     void*       () const noexcept;
        bool         operator!   () const noexcept; 
        
        // Load a music file for playback.
        bool         load        (char    const* filename) noexcept;
        // Load a music file for playback.
        bool         load        (wchar_t const* filename) noexcept;
        // Queue a sound file to follow the current.
        bool         queue       (char    const* filename, int loops = 0) noexcept;
        // Queue a sound file to follow the current.
        bool         queue       (wchar_t const* filename, int loops = 0) noexcept;
        // Unload the currently loaded music to free up resources.
        bool         unload      () noexcept;
        // Start the playback of the music stream.
        bool         play        (int loops = 0, float start_s = .0f, float fadein_s = .0f) noexcept;
        // Restart music.
        bool         rewind      () noexcept;                     
        // Stop the music playback.
        bool         stop        () noexcept;                     
        // Temporarily stop music playback.
        bool         pause       () noexcept;           
        // Resume paused music.
        bool         unpause     () noexcept;
        // Stop music playback after fading out.
        bool         fadeout     (float fadeout_s) noexcept;
        // Set the music volume.
        bool         set_volume  (float volume) noexcept;
        // Get the music volume.
        float        get_volume  () noexcept;
        // Get the music play time.
        float        get_pos     () noexcept;
        // Set the music play time.
        bool         set_pos     (float pos_s) noexcept;
        // Get the music length.
        float        get_length  () noexcept;
        // Returns true if the music stream is playing.
        bool         is_busy     () noexcept;
    };
   /** @}  */
    
} // namespace

# endif // MCL_MUSIC

#endif // defined(MCL_MIXER)
