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
# include <initializer_list>

# ifdef _MSC_VER
#  pragma warning(pop)
# endif

// no define

namespace
mcl {
    
   /**
    * @unimplemented
    *     pygame.display.list_modes()
    *     pygame.display.mode_ok()
    *     pygame.display.set_palette()
    * 
    * @unfinished
    *     pygame.display.set_mode()
    *     pygame.display.get_desktop_sizes()
    *     pygame.display.get_num_displays()
    * 
    * @feature
    *     mcl::display.maximize()
    *     mcl::display.hide()
    *     mcl::display.toggle_dynamic_wallpaper()
    */

   /**
    * @enum class mcl_dflags_t <src/display.h>
    * @brief Flags that controls which type of display you want.
    * 
    * @ingroup display
    * @ingroup mclib
    */
    class
    mcl_dflags_t {
    
    public:
        using type = unsigned long;

        // Window is opened in visible mode (default)
        static type constexpr Shown      = 0x0;
        // Window is opened in hidden mode
        static type constexpr Hidden     = 0x1;
        // Create an iconified display
        static type constexpr Minimize   = 0x2;
        // Create an maximized display
        static type constexpr Maximize   = 0x4;
        // Display window should be resizable
        static type constexpr Resizable     = 0x10;
        // Display window will have no border or controls
        static type constexpr NoFrame       = 0x20;
        // Display window will disable minimizing
        static type constexpr NoMinimizeBox = 0x40;
        // Display window will be double buffered
        static type constexpr DoubleBuf     = 0x80;
        // Allow dropping files into display window
        static type constexpr AllowDropping = 0x100;
    
    };
    using dflags_t = mcl_dflags_t::type; // Display flags type for mcl::display.init() 
    extern mcl_dflags_t dflags; // Enum class of display flags.



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
    */
    class
    mcl_display_t { public:
        
        explicit          mcl_display_t ()                         = default;
                         ~mcl_display_t ()                         = default;
                          mcl_display_t (mcl_display_t const& rhs) = delete;
        mcl_display_t&    operator=     (mcl_display_t const& rhs) = delete;

        // Same as get_init()
        operator          void*         () const noexcept;
        // Contrary to the result of get_init()
        bool              operator!     () const noexcept;
        
        // Initialize the window
        mcl_display_t&    init          () noexcept;
        // Uninitialize the window. NOT end the program
        mcl_display_t&    quit          () noexcept;
        // Returns true if the window has been created
        bool              get_init      () const noexcept;

        // Initialize a default window or screen for display
        inline surface_t& set_mode      (void* = 0) noexcept{ return init (), get_surface (); }
        // Initialize a window or screen with display flags for display
        inline surface_t& set_mode      (void*, dflags_t dpm_flags) noexcept{ return set_mode (get_window_size (), dpm_flags); }
        // Initialize a window or screen with size and display flags for display
        surface_t&        set_mode      (point2d_t size, dflags_t dpm_flags) noexcept;
        // Initialize a window or screen of specified size for display
        surface_t&        set_mode      (point2d_t size) noexcept;
        // Get dpm_flags for set_mode
        dflags_t          get_flags     () const noexcept;
        
        
        // Get a reference to the currently set display surface
        surface_t&        get_surface   () const noexcept;
        // Update the full display Surface to the screen
        mcl_display_t&    flip          () noexcept;
        // Update portions of the screen for software displays
        mcl_display_t&    update        (void* = 0) noexcept{ return flip (); }
        // Update portions of the screen for software displays
        mcl_display_t&    update        (rect_t recta) noexcept;
        // Update portions of the screen for software displays
        mcl_display_t&    update        (std::initializer_list<rect_t>&& rects) noexcept;

        // Get the name of the pygame display backend.  This is always "windows" at present.
        char const*       get_driver_a  () const noexcept;
        // Get the name of the pygame display backend.  This is always L"windows" at present.
        wchar_t const*    get_driver    () const noexcept;
        // Get information about the current windowing system
        wmi_dict_t        get_wm_info   () const noexcept;     
        // Get size of desktop 
        point2d_t         get_desktop_size () const noexcept;
        // Get rect of desktop 
        rect_t            get_desktop_rect () const noexcept;

        // Returns true when the display is active on the screen and may be visible to the user
        bool              get_active    () const noexcept;
        // Make the window invisible
        mcl_display_t&    hide          (bool b_hide = true) noexcept;
        // Iconify the display surface
        mcl_display_t&    iconify       () noexcept;
        // Maximize the display surface
        mcl_display_t&    maximize      () noexcept;

        // Switch between fullscreen and windowed displays
        mcl_display_t&    toggle_fullscreen () noexcept;
        // Return true if full screen mode is set.
        bool              get_fullscreen () const noexcept;

        // Switch between wallpaper and windowed displays (Experimental)
        mcl_display_t&    toggle_dynamic_wallpaper () noexcept;
        // Return true if wallpaper mode is set.
        bool              get_dynamic_wallpaper () const noexcept;

        // Set the current window caption
        mcl_display_t&    set_caption   (char    const* caption = nullptr) noexcept;
        // Set the current window caption
        mcl_display_t&    set_caption   (wchar_t const* caption = nullptr) noexcept;
        // Get the current window caption
        std::wstring      get_caption   () const noexcept;
        // Get the current window caption
        std::string       get_caption_a () const noexcept;
        // Change the system image for the display window
        mcl_display_t&    set_icon      (surface_t const& icon) noexcept;
        // Set WS_EX_LAYERED on this window.
        mcl_display_t&    set_window_alpha (double f_alpha = 0.) noexcept; 
    
        // Return the number of available displays. This is always 1 at present.
        size_t            get_num_displays () const noexcept;
        // Get position of desktop 
        point2d_t         get_window_pos () const noexcept;
        // Return the size of the window or screen
        point2d_t         get_window_size () const noexcept;
        // Return the position and size of the window or screen
        rect_t            get_window_rect () const noexcept;
        // Set the coordinates of the top left corner of the form
        mcl_display_t&    set_window_pos (point2d_t topleft) noexcept;

        // Set whether the screensaver may run
        bool              set_allow_screensaver (bool b_allow = true) noexcept; 
        // Return whether the screensaver is allowed to run
        bool              get_allow_screensaver () const noexcept;

    };
    extern mcl_display_t display; // Module for controling the display.


    
   /** 
    * @class wmi_dict_t <src/display.h>
    * @brief simplified python dict for mcl::display.get_wm_info() return-type
    * 
    * @code
    * // example (keys are all lowercase): 
    *     HDC       hdc  = mcl::display.get_wm_info()["hdc"];
    *     HINSTANCE instance = mcl::display.get_wm_info()["hinstance"];
    *     HANDLE    th   = mcl::display.get_wm_info()["taskhandle"];
    *     HWND      hwnd = mcl::display.get_wm_info()["window"];
    * @endcode
    * @ingroup display
    * @ingroup py-style
    * @ingroup mclib
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
        constexpr iterator<wmi_dict_t> begin () noexcept{ return iterator<wmi_dict_t> {0ul};                }
        inline iterator<wmi_dict_t>    end   () noexcept{ return iterator<wmi_dict_t> {len (wmi_dict_t{})}; }
    }; 

} // namespace

#endif // MCL_DISPLAY
