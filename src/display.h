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
    
    @file src/display.h
    This is a C++11 header for controling the display window
    and screen.
*/


#ifndef MCL_DISPLAY
# define MCL_DISPLAY

# include "mclfwd.h"
# include <cstdlib>  // for size_t

# ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4365)
# endif

# include <string>
# include <ostream>

# ifdef _MSC_VER
#  pragma warning(pop)
# endif

// no define

namespace
mcl {
    
   /**
    * @enum mcl_display_mode_flag_enum <src/display.h>
    * @brief window style
    * 
    * @ingroup display
    * @ingroup mclib
    */
    enum
    mcl_display_mode_flag_enum
    : flag32_t {
        dpmShown      = 0x0,
        dpmHidden     = 0x1,
        dpmMinimize   = 0x2,
        dpmMaximize   = 0x4,
                
        dpmMovable       = 0x10,
        dpmNoFrame       = 0x20,
        dpmNoMinimizeBox = 0x40,
        
//      dpmBySizeForm = 0x100 | dpmMovable,
//      dpmDoubleBuf  = 0x200,
//      dpmLayered    = 0x400,
    };

   /**
    * @class mcl_display_t <src/display.h>
    * @brief
    *  This class offers control over the mclib display. Mclib
    *  has a single display Surface that is either contained in
    *  a window or runs full screen. Once you create the display
    *  you treat it as a regular Surface. Changes are not
    *  immediately visible onscreen; you must choose one of the
    *  two flipping functions to update the actual display.
    *
    *  The origin of the display, where x = 0 and y = 0, is the
    *  top left of the screen. Both axes increase positively
    *  towards the bottom right of the screen.
    *  
    *  The mclib display can actually be initialized in one of
    *  several modes. By default, the display is a basic software
    *  driven framebuffer. You can request special modules like
    *  automatic scaling. These are controlled by flags passed to
    *  mcl::display.set_mode().
    *  
    *  Mclib can only have a single display active at any time.
    *  Creating a new one with mcl::display.set_mode() will
    *  close the previous display. To detect the number and size
    *  of attached screens, you can use mcl::display.get_desktop_sizes
    *  and then select appropriate window size and display index to
    *  pass to mcl::display.set_mode().
    *  
    *  For backward compatibility mcl::display allows precise control
    *  over the pixel format or display resolutions. This used to be
    *  necessary with old grahics cards and CRT screens, but is
    *  usually not needed any more. Use the functions mcl::display.mode_ok(),
    *  mcl::display.list_modes(), and mcl::display.Info() to query
    *  detailed information about the display.
    *  
    *  Once the display Surface is created, the functions from this
    *  module affect the single existing display. The Surface becomes
    *  invalid if the module is uninitialized. If a new display mode
    *  is set, the existing Surface will automatically switch to
    *  operate on the new display.
    *  
    *  When the display mode is set, several events are placed on
    *  the mclib event queue. mcl::QUIT is sent when the user has
    *  requested the program to shut down. The window will receive
    *  mcl::ACTIVEEVENT events as the display gains and loses input
    *  focus. If the display is set with the mcl::RESIZABLE flag,
    *  mcl::VIDEORESIZE events will be sent when the user adjusts
    *  the window dimensions. Hardware displays that draw direct to
    *  the screen will get mcl::VIDEOEXPOSE events when portions of
    *  the window must be redrawn.
    *
    *  A new windowevent API was introduced in mclib 2.0.1. Check
    *  event module docs for more information on that.
    *
    *  Some display environments have an option for automatically
    *  stretching all windows. When this option is enabled, this
    *  automatic stretching distorts the appearance of the mclib window.
    *  In the mclib examples directory, there is example code
    *  (prevent_display_stretching.py) which shows how to disable this
    *  automatic stretching of the mclib display on Microsoft Windows
    *  (Vista or newer required).
    * 
    * @ingroup display
    * @ingroup mclib
    * @{
    */
    class
    mcl_display_t { public:
        
        explicit          mcl_display_t ()                         = default;
                         ~mcl_display_t ()                         = default;
                          mcl_display_t (mcl_display_t const& rhs) = delete;
        mcl_display_t&    operator=     (mcl_display_t const& rhs) = delete;
        operator          void*         () const noexcept;              // Same as is_init()
        bool              operator!     () const noexcept;

        mcl_display_t&    init             () noexcept;                 // Initialize the window
        bool              is_init          () const noexcept;           // Returns true if the window has been created
        mcl_display_t&    uninit           () noexcept;                 // Uninitialize the window. NOT end the program
        flag32_t          get_flags        () const noexcept;           // Get dpm_flags for set_mode
        inline mcl_display_t& set_mode     (char = 0) noexcept{ return init (); }
        inline mcl_display_t& set_mode     (char, flag32_t dpm_flags) noexcept{ return set_mode (get_window_size (), dpm_flags); }
        mcl_display_t&    set_mode         (point2d_t size, flag32_t dpm_flags) noexcept;
        mcl_display_t&    set_mode         (point2d_t size) noexcept;   // Initialize a window or screen for display
        char const*       get_driver_a     () const noexcept;           // Get the name of the pygame display backend.  This is always 'windows' at present.
        wchar_t const*    get_driver       () const noexcept;           // Get the name of the pygame display backend.  This is always 'windows' at present.
        mcl_display_t&    set_caption      (char    const* caption = nullptr) noexcept; 
        mcl_display_t&    set_caption      (wchar_t const* caption = nullptr) noexcept; 
                                                                        // Set the current window caption
        wchar_t const*    get_caption      () const noexcept;           // Get the current window caption
//      surface&          get_surface      () const noexcept;
//      mcl_display_t&    flip             () noexcept;
//      mcl_display_t&    update           () noexcept;
        size_t            get_num_displays () const noexcept;           // Return the number of available displays. This is always 1 at present. 
        bool              set_allow_screensaver (bool b_allow = true) noexcept;  // Set whether the screensaver may run
        bool              is_allow_screensaver  () const noexcept;      // Return whether the screensaver is allowed to run
        mcl_display_t&    hide             (bool b_hide = true) noexcept;        // Make the window invisible
        mcl_display_t&    toggle_fullscreen() noexcept;                 // Switch between fullscreen and windowed displays
        bool              is_fullscreen    () const noexcept;           // Return true if full screen mode is set.
        mcl_display_t&    iconify          () noexcept;                 // Iconify the display surface
        mcl_display_t&    maximize         () noexcept;                 // Maximize the display surface
        point2d_t         get_window_pos   () const noexcept;           // Get position of desktop 
        point2d_t         get_window_size  () const noexcept;           // Return the size of the window or screen
        point2d_t         get_desktop_size () const noexcept;           // Get size of desktop 
        mcl_display_t&    set_window_alpha (double f_alpha = 0.) noexcept;       // Set WS_EX_LAYERED on this window.
        wmi_dict_t        get_wm_info      () const noexcept;           // Get information about the current windowing system
        bool              is_active        () const noexcept;           // Returns true when the display is active on the screen and may be visible to the user
    
    };
    
   /** 
    * @class wmi_dict_t <src/display.h>
    * @brief simplified python dict for mcl::display.get_wm_info() return-type
    * 
    * @code
    * // example (keys are all lowercase): 
    *     HDC       hdc  = mcl::display.get_wm_info()["hdc"];
    *     HINSTANCE inst = mcl::display.get_wm_info()["hinstance"];
    *     HANDLE    th   = mcl::display.get_wm_info()["taskhandle"];
    *     HWND      hwnd = mcl::display.get_wm_info()["window"];
    * @endcode
    * @ingroup display
    * @ingroup py-style
    */
    class wmi_dict_t {  static wchar_t const* get_ (size_t it) noexcept;
    public: 
        class map_proxy_t { void* m_ptr; constexpr explicit map_proxy_t (void* p) noexcept: m_ptr (p) { } 
          public: friend class wmi_dict_t; template<class T> constexpr operator T* () noexcept{ return static_cast<T*>(m_ptr); }
          friend inline std::ostream& operator<< (std::ostream& os, map_proxy_t const& mp) { return os << mp.m_ptr; }
          friend inline std::wostream& operator<< (std::wostream& os, map_proxy_t const& mp) { return os << mp.m_ptr; }  };
    public: 
        friend size_t                len        (wmi_dict_t const dict) noexcept;
        friend std::string           str_a      (wmi_dict_t const dict) noexcept;
        friend std::wstring          str        (wmi_dict_t const dict) noexcept;
        friend constexpr int         cmp        (wmi_dict_t const, wmi_dict_t const) noexcept{ return 0; }
        friend inline std::ostream&  operator<< (std::ostream&  os, wmi_dict_t const& mp) { return os << str_a (mp); }
        friend inline std::wostream& operator<< (std::wostream& os, wmi_dict_t const& mp) { return os << str (mp); }
        map_proxy_t                  get        (char    const* key, void* defaultv = nullptr) const noexcept;
        map_proxy_t                  get        (wchar_t const* key, void* defaultv = nullptr) const noexcept;
        bool                         has_key    (char    const* key) const noexcept;
        bool                         has_key    (wchar_t const* key) const noexcept;
        map_proxy_t                  operator[] (char    const* key) const noexcept;
        map_proxy_t                  operator[] (wchar_t const* key) const noexcept;
    private:
        template<typename T, typename Tva = wchar_t const*> class iterator {  size_t iter_; friend class wmi_dict_t; 
          constexpr explicit iterator   (size_t iter) noexcept: iter_ (iter) { }  public:
          constexpr Tva      operator*  () noexcept{ return T::get_ (iter_); }
          constexpr bool     operator!= (iterator const& rhs) const noexcept{ return this->iter_ != rhs.iter_; }
          inline iterator&   operator++ () noexcept{ ++ iter_; return *this; }  };
    public:
        constexpr iterator<wmi_dict_t>  begin () noexcept{ return iterator<wmi_dict_t> {0ul};               }
        inline iterator<wmi_dict_t>  end   () noexcept{ return iterator<wmi_dict_t> {len (wmi_dict_t{})}; }
    }; 
    
/** @}  */


   /**
    * @name Mclib Display Control Objects
    * @brief 
    *   The &lt;display&gt; header declares the two <em>standard display
    *   control objects</em>.  For more information, see the section of
    *   the manual linked to above.
    * @{
    */
    extern mcl_display_t display;
    
/** @}  */

} // namespace

#endif // MCL_DISPLAY
