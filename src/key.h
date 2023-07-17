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
    
    @file src/key.h
    This is a C++11 header for working with the keyboard.
*/


#ifndef MCL_KEY
# define MCL_KEY

# include "mclfwd.h"
# include <vector>

namespace
mcl {
    
   /**
    * @class mcl_key_t <src/key.h>
    * @brief module for working with the keyboard
    * 
    * @ingroup key
    * @ingroup event
    * @ingroup mclib
    */
    class
    mcl_key_t { public:
        explicit       mcl_key_t ()                       = default;
                      ~mcl_key_t ()                       = default;
                       mcl_key_t (mcl_key_t const& rhs)   = delete;
        mcl_key_t&     operator= (mcl_key_t const& rhs)   = delete;

    public:
        using type = unsigned char;
        using mod_type = wchar_t;
 
        static type constexpr VkNul       = 0x00;
        static type constexpr VkCancel    = 0x03;
        static type constexpr VkBack      = 0x08;
        static type constexpr VkTab       = 0x09;
        static type constexpr VkClear     = 0x0c;
        static type constexpr VkReturn    = 0x0d;
        static type constexpr VkShift     = 0x10;
        static type constexpr VkControl   = 0x11;
        static type constexpr VkMenu      = 0x12;
        static type constexpr VkPause     = 0x13;
        static type constexpr VkCapital   = 0x14;
        static type constexpr VkKana      = 0x15;
        static type constexpr VkHanguel   = 0x15;
        static type constexpr VkHangul    = 0x15;
        static type constexpr VkIMEOn     = 0x16;
        static type constexpr VkJunja     = 0x17;
        static type constexpr VkFinal     = 0x18;
        static type constexpr VkHanja     = 0x19;
        static type constexpr VkKanji     = 0x19;
        static type constexpr VkIMEOff    = 0x1a;
        static type constexpr VkEscape    = 0x1b;
        static type constexpr VkConvert   = 0x1c;
        static type constexpr VkNonConvert= 0x1d;
        static type constexpr VkAccept    = 0x1e;
        static type constexpr VkModeChange= 0x1f;
        static type constexpr VkSpace     = 0x20;
        static type constexpr VkPrior     = 0x21;
        static type constexpr VkNext      = 0x22;
        static type constexpr VkEnd       = 0x23;
        static type constexpr VkHome      = 0x24;
        static type constexpr VkLeft      = 0x25;
        static type constexpr VkUp        = 0x26;
        static type constexpr VkRight     = 0x27;
        static type constexpr VkDown      = 0x28;
        static type constexpr VkSelect    = 0x29;
        static type constexpr VkPrint     = 0x2a;
        static type constexpr VkExecute   = 0x2b;
        static type constexpr VkSnapshot  = 0x2c;
        static type constexpr VkInsert    = 0x2d;
        static type constexpr VkDelete    = 0x2e;
        static type constexpr VkHelp      = 0x2f;
        static type constexpr Vk0         = 0x30;
        static type constexpr Vk1         = 0x31;
        static type constexpr Vk2         = 0x32;
        static type constexpr Vk3         = 0x33;
        static type constexpr Vk4         = 0x34;
        static type constexpr Vk5         = 0x35;
        static type constexpr Vk6         = 0x36;
        static type constexpr Vk7         = 0x37;
        static type constexpr Vk8         = 0x38;
        static type constexpr Vk9         = 0x39;
        static type constexpr VkA         = 0x41; 
        static type constexpr VkB         = 0x42;
        static type constexpr VkC         = 0x43;
        static type constexpr VkD         = 0x44;
        static type constexpr VkE         = 0x45;
        static type constexpr VkF         = 0x46;
        static type constexpr VkG         = 0x47;
        static type constexpr VkH         = 0x48;
        static type constexpr VkI         = 0x49;
        static type constexpr VkJ         = 0x4a;
        static type constexpr VkK         = 0x4b;
        static type constexpr VkL         = 0x4c;
        static type constexpr VkM         = 0x4d;
        static type constexpr VkN         = 0x4e;
        static type constexpr VkO         = 0x4f;
        static type constexpr VkP         = 0x50;
        static type constexpr VkQ         = 0x51;
        static type constexpr VkR         = 0x52;
        static type constexpr VkS         = 0x53;
        static type constexpr VkT         = 0x54;
        static type constexpr VkU         = 0x55;
        static type constexpr VkV         = 0x56;
        static type constexpr VkW         = 0x57;
        static type constexpr VkX         = 0x58;
        static type constexpr VkY         = 0x59;
        static type constexpr VkZ         = 0x5a;
        static type constexpr VkLWin      = 0x5b;
        static type constexpr VkRWin      = 0x5c;
        static type constexpr VkApps      = 0x5d;
        static type constexpr VkSleep     = 0x5f;
        static type constexpr VkNumpad0   = 0x60;
        static type constexpr VkNumpad1   = 0x61;
        static type constexpr VkNumpad2   = 0x62;
        static type constexpr VkNumpad3   = 0x63;
        static type constexpr VkNumpad4   = 0x64;
        static type constexpr VkNumpad5   = 0x65;
        static type constexpr VkNumpad6   = 0x66;
        static type constexpr VkNumpad7   = 0x67;
        static type constexpr VkNumpad8   = 0x68;
        static type constexpr VkNumpad9   = 0x69;
        static type constexpr VkMultiply  = 0x6a;
        static type constexpr VkAdd       = 0x6b;
        static type constexpr VkSeparator = 0x6c;
        static type constexpr VkSubtract  = 0x6d;
        static type constexpr VkDecimal   = 0x6e;
        static type constexpr VkDivide    = 0x6f;
        static type constexpr VkF1        = 0x70;
        static type constexpr VkF2        = 0x71;
        static type constexpr VkF3        = 0x72;
        static type constexpr VkF4        = 0x73;
        static type constexpr VkF5        = 0x74;
        static type constexpr VkF6        = 0x75;
        static type constexpr VkF7        = 0x76;
        static type constexpr VkF8        = 0x77;
        static type constexpr VkF9        = 0x78;
        static type constexpr VkF10       = 0x79;
        static type constexpr VkF11       = 0x7a;
        static type constexpr VkF12       = 0x7b;
        static type constexpr VkF13       = 0x7c;
        static type constexpr VkF14       = 0x7d;
        static type constexpr VkF15       = 0x7e;
        static type constexpr VkF16       = 0x7f;
        static type constexpr VkF17       = 0x80;
        static type constexpr VkF18       = 0x81;
        static type constexpr VkF19       = 0x82;
        static type constexpr VkF20       = 0x83;
        static type constexpr VkF21       = 0x84;
        static type constexpr VkF22       = 0x85;
        static type constexpr VkF23       = 0x86;
        static type constexpr VkF24       = 0x87;
        static type constexpr VkNumlock   = 0x90;
        static type constexpr VkScroll    = 0x91;
        static type constexpr VkLShift    = 0xa0;
        static type constexpr VkRShift    = 0xa1;
        static type constexpr VkLControl  = 0xa2;
        static type constexpr VkRControl  = 0xa3;
        static type constexpr VkLMenu     = 0xa4;
        static type constexpr VkRMenu     = 0xa5;
        static type constexpr VkBrowserBack      = 0xa6;
        static type constexpr VkBrowserForward   = 0xa7;
        static type constexpr VkBrowserRefresh   = 0xa8;
        static type constexpr VkBrowserStop      = 0xa9;
        static type constexpr VkBrowserSearch    = 0xaa;
        static type constexpr VkBrowserFavorites = 0xab;
        static type constexpr VkBrowserHome      = 0xac;
        static type constexpr VkVolumeMute       = 0xad;
        static type constexpr VkVolumeDown       = 0xae;
        static type constexpr VkVolumeUp         = 0xaf;
        static type constexpr VkMediaNextTrack   = 0xb0;
        static type constexpr VkMediaPrevTrack   = 0xb1;
        static type constexpr VkMediaStop        = 0xb2;
        static type constexpr VkMediaPlayPause   = 0xb3;
        static type constexpr VkLaunchMail       = 0xb4;
        static type constexpr VkLaunchMediaSelect= 0xb5;
        static type constexpr VkLaunchApp1       = 0xb6;
        static type constexpr VkLaunchApp2       = 0xb7;
        static type constexpr VkOEM1      = 0xba;
        static type constexpr VkOEMPlus   = 0xbb;
        static type constexpr VkOEMComma  = 0xbc;
        static type constexpr VkOEMMinus  = 0xbd;
        static type constexpr VkOEMPeriod = 0xbe;
        static type constexpr VkOEM2      = 0xbf;
        static type constexpr VkOEM3      = 0xc0;
        static type constexpr VkOEM4      = 0xdb;
        static type constexpr VkOEM5      = 0xdc;
        static type constexpr VkOEM6      = 0xdd;
        static type constexpr VkOEM7      = 0xde;
        static type constexpr VkOEM8      = 0xdf;
        static type constexpr VkOEM102    = 0xe2;
        static type constexpr VkProcessKey= 0xe5;
        static type constexpr VkPacket    = 0xe7;
        static type constexpr VkAttn      = 0xf6;
        static type constexpr VkCrsel     = 0xf7;
        static type constexpr VkExsel     = 0xf8;
        static type constexpr VkErEOF     = 0xf9;
        static type constexpr VkPlay      = 0xfa;
        static type constexpr VkZoom      = 0xfb;
        static type constexpr VkNoName    = 0xfc;
        static type constexpr VkPa1       = 0xfd;
        static type constexpr VkOemClear  = 0xfe;

        static mod_type constexpr ModNone   =   0x0;
        static mod_type constexpr ModLShift =   0x1;
        static mod_type constexpr ModRShift =   0x2;
        static mod_type constexpr ModShift  =   0x3;
        static mod_type constexpr ModLCtrl  =   0x4;
        static mod_type constexpr ModRCtrl  =   0x8;
        static mod_type constexpr ModCtrl   =   0xc;
        static mod_type constexpr ModLAlt   =  0x10;
        static mod_type constexpr ModRAlt   =  0x20;
        static mod_type constexpr ModAlt    =  0x30;
        static mod_type constexpr ModLMeta  =  0x40;
        static mod_type constexpr ModRMeta  =  0x80;
        static mod_type constexpr ModMeta   =  0xc0;
        static mod_type constexpr ModCaps   = 0x100;
        static mod_type constexpr ModNum    = 0x200;
        static mod_type constexpr ModMode   = 0x400;

    public:
        operator       void*       () const noexcept;
        bool           operator!   () const noexcept;
        
        // get the mouse cursor position
        bool              get_focused () noexcept;
        // get the state of all keyboard buttons
        std::vector<bool> get_pressed () noexcept;
        // determine which modifier keys are being held
        wchar_t           get_mods    () noexcept;
        // determine whether a key is up or down. focus not required
        bool              get_async_pressed (type vk_index) noexcept;
        // determine whether a modifier key is up or down. focus not required
        bool              get_async_mods (mod_type mod_mask) noexcept;
        // control how held keys are repeated
        bool              set_repeat  (bool b_repeat = false) noexcept;
        bool              get_repeat  () noexcept;
        // get the name of a key identifier
        wchar_t const*    name        (type vkey) noexcept;
        char const*       name_a      (type vkey) noexcept;
        // start handling unicode text input events
        void              start_text_input () noexcept;
        void              stop_text_input () noexcept;
        void              set_text_input_rect (rect_t rect) noexcept;
        void              set_text_input_rect (rect_t rect, point2d_t start) noexcept;

    };
    extern mcl_key_t key; // Module for working with the keyboard.

} // namespace

#endif // MCL_KEY
