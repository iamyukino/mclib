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

    @file cpp/key.cpp
    This is a C++11 implementation file for working
    with the keyboard.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "mcl_control.h"

#include "../src/key.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for working with the keyboard.
    mcl_key_t key;

    mcl_key_t::
    operator void* () const noexcept{
        return mcl_control_obj.bIsReady ?
            const_cast<mcl_key_t*>(this) : 0;
    }
    bool mcl_key_t::
    operator! () const noexcept{
        return !mcl_control_obj.bIsReady;
    }

#if __cplusplus < 201703L
    mcl_key_t::type constexpr mcl_key_t::VkNul;
    mcl_key_t::type constexpr mcl_key_t::VkCancel;
    mcl_key_t::type constexpr mcl_key_t::VkBack;
    mcl_key_t::type constexpr mcl_key_t::VkTab;
    mcl_key_t::type constexpr mcl_key_t::VkClear;
    mcl_key_t::type constexpr mcl_key_t::VkReturn;
    mcl_key_t::type constexpr mcl_key_t::VkShift;
    mcl_key_t::type constexpr mcl_key_t::VkControl;
    mcl_key_t::type constexpr mcl_key_t::VkMenu;
    mcl_key_t::type constexpr mcl_key_t::VkPause;
    mcl_key_t::type constexpr mcl_key_t::VkCapital;
    mcl_key_t::type constexpr mcl_key_t::VkKana;
    mcl_key_t::type constexpr mcl_key_t::VkHanguel;
    mcl_key_t::type constexpr mcl_key_t::VkHangul;
    mcl_key_t::type constexpr mcl_key_t::VkIMEOn;
    mcl_key_t::type constexpr mcl_key_t::VkJunja;
    mcl_key_t::type constexpr mcl_key_t::VkFinal;
    mcl_key_t::type constexpr mcl_key_t::VkHanja;
    mcl_key_t::type constexpr mcl_key_t::VkKanji;
    mcl_key_t::type constexpr mcl_key_t::VkIMEOff;
    mcl_key_t::type constexpr mcl_key_t::VkEscape;
    mcl_key_t::type constexpr mcl_key_t::VkConvert;
    mcl_key_t::type constexpr mcl_key_t::VkNonConvert;
    mcl_key_t::type constexpr mcl_key_t::VkAccept;
    mcl_key_t::type constexpr mcl_key_t::VkModeChange;
    mcl_key_t::type constexpr mcl_key_t::VkSpace;
    mcl_key_t::type constexpr mcl_key_t::VkPrior;
    mcl_key_t::type constexpr mcl_key_t::VkNext;
    mcl_key_t::type constexpr mcl_key_t::VkEnd;
    mcl_key_t::type constexpr mcl_key_t::VkHome;
    mcl_key_t::type constexpr mcl_key_t::VkLeft;
    mcl_key_t::type constexpr mcl_key_t::VkUp;
    mcl_key_t::type constexpr mcl_key_t::VkRight;
    mcl_key_t::type constexpr mcl_key_t::VkDown;
    mcl_key_t::type constexpr mcl_key_t::VkSelect;
    mcl_key_t::type constexpr mcl_key_t::VkPrint;
    mcl_key_t::type constexpr mcl_key_t::VkExecute;
    mcl_key_t::type constexpr mcl_key_t::VkSnapshot;
    mcl_key_t::type constexpr mcl_key_t::VkInsert;
    mcl_key_t::type constexpr mcl_key_t::VkDelete;
    mcl_key_t::type constexpr mcl_key_t::VkHelp;
    mcl_key_t::type constexpr mcl_key_t::Vk0;
    mcl_key_t::type constexpr mcl_key_t::Vk1;
    mcl_key_t::type constexpr mcl_key_t::Vk2;
    mcl_key_t::type constexpr mcl_key_t::Vk3;
    mcl_key_t::type constexpr mcl_key_t::Vk4;
    mcl_key_t::type constexpr mcl_key_t::Vk5;
    mcl_key_t::type constexpr mcl_key_t::Vk6;
    mcl_key_t::type constexpr mcl_key_t::Vk7;
    mcl_key_t::type constexpr mcl_key_t::Vk8;
    mcl_key_t::type constexpr mcl_key_t::Vk9;
    mcl_key_t::type constexpr mcl_key_t::VkA; 
    mcl_key_t::type constexpr mcl_key_t::VkB;
    mcl_key_t::type constexpr mcl_key_t::VkC;
    mcl_key_t::type constexpr mcl_key_t::VkD;
    mcl_key_t::type constexpr mcl_key_t::VkE;
    mcl_key_t::type constexpr mcl_key_t::VkF;
    mcl_key_t::type constexpr mcl_key_t::VkG;
    mcl_key_t::type constexpr mcl_key_t::VkH;
    mcl_key_t::type constexpr mcl_key_t::VkI;
    mcl_key_t::type constexpr mcl_key_t::VkJ;
    mcl_key_t::type constexpr mcl_key_t::VkK;
    mcl_key_t::type constexpr mcl_key_t::VkL;
    mcl_key_t::type constexpr mcl_key_t::VkM;
    mcl_key_t::type constexpr mcl_key_t::VkN;
    mcl_key_t::type constexpr mcl_key_t::VkO;
    mcl_key_t::type constexpr mcl_key_t::VkP;
    mcl_key_t::type constexpr mcl_key_t::VkQ;
    mcl_key_t::type constexpr mcl_key_t::VkR;
    mcl_key_t::type constexpr mcl_key_t::VkS;
    mcl_key_t::type constexpr mcl_key_t::VkT;
    mcl_key_t::type constexpr mcl_key_t::VkU;
    mcl_key_t::type constexpr mcl_key_t::VkV;
    mcl_key_t::type constexpr mcl_key_t::VkW;
    mcl_key_t::type constexpr mcl_key_t::VkX;
    mcl_key_t::type constexpr mcl_key_t::VkY;
    mcl_key_t::type constexpr mcl_key_t::VkZ;
    mcl_key_t::type constexpr mcl_key_t::VkLWin;
    mcl_key_t::type constexpr mcl_key_t::VkRWin;
    mcl_key_t::type constexpr mcl_key_t::VkApps;
    mcl_key_t::type constexpr mcl_key_t::VkSleep;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad0;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad1;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad2;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad3;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad4;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad5;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad6;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad7;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad8;
    mcl_key_t::type constexpr mcl_key_t::VkNumpad9;
    mcl_key_t::type constexpr mcl_key_t::VkMultiply;
    mcl_key_t::type constexpr mcl_key_t::VkAdd;
    mcl_key_t::type constexpr mcl_key_t::VkSeparator;
    mcl_key_t::type constexpr mcl_key_t::VkSubtract;
    mcl_key_t::type constexpr mcl_key_t::VkDecimal;
    mcl_key_t::type constexpr mcl_key_t::VkDivide;
    mcl_key_t::type constexpr mcl_key_t::VkF1;
    mcl_key_t::type constexpr mcl_key_t::VkF2;
    mcl_key_t::type constexpr mcl_key_t::VkF3;
    mcl_key_t::type constexpr mcl_key_t::VkF4;
    mcl_key_t::type constexpr mcl_key_t::VkF5;
    mcl_key_t::type constexpr mcl_key_t::VkF6;
    mcl_key_t::type constexpr mcl_key_t::VkF7;
    mcl_key_t::type constexpr mcl_key_t::VkF8;
    mcl_key_t::type constexpr mcl_key_t::VkF9;
    mcl_key_t::type constexpr mcl_key_t::VkF10;
    mcl_key_t::type constexpr mcl_key_t::VkF11;
    mcl_key_t::type constexpr mcl_key_t::VkF12;
    mcl_key_t::type constexpr mcl_key_t::VkF13;
    mcl_key_t::type constexpr mcl_key_t::VkF14;
    mcl_key_t::type constexpr mcl_key_t::VkF15;
    mcl_key_t::type constexpr mcl_key_t::VkF16;
    mcl_key_t::type constexpr mcl_key_t::VkF17;
    mcl_key_t::type constexpr mcl_key_t::VkF18;
    mcl_key_t::type constexpr mcl_key_t::VkF19;
    mcl_key_t::type constexpr mcl_key_t::VkF20;
    mcl_key_t::type constexpr mcl_key_t::VkF21;
    mcl_key_t::type constexpr mcl_key_t::VkF22;
    mcl_key_t::type constexpr mcl_key_t::VkF23;
    mcl_key_t::type constexpr mcl_key_t::VkF24;
    mcl_key_t::type constexpr mcl_key_t::VkNumlock;
    mcl_key_t::type constexpr mcl_key_t::VkScroll;
    mcl_key_t::type constexpr mcl_key_t::VkLShift;
    mcl_key_t::type constexpr mcl_key_t::VkRShift;
    mcl_key_t::type constexpr mcl_key_t::VkLControl;
    mcl_key_t::type constexpr mcl_key_t::VkRControl;
    mcl_key_t::type constexpr mcl_key_t::VkLMenu;
    mcl_key_t::type constexpr mcl_key_t::VkRMenu;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserBack;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserForward;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserRefresh;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserStop;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserSearch;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserFavorites;
    mcl_key_t::type constexpr mcl_key_t::VkBrowserHome;
    mcl_key_t::type constexpr mcl_key_t::VkVolumeMute;
    mcl_key_t::type constexpr mcl_key_t::VkVolumeDown;
    mcl_key_t::type constexpr mcl_key_t::VkVolumeUp;
    mcl_key_t::type constexpr mcl_key_t::VkMediaNextTrack;
    mcl_key_t::type constexpr mcl_key_t::VkMediaPrevTrack;
    mcl_key_t::type constexpr mcl_key_t::VkMediaStop;
    mcl_key_t::type constexpr mcl_key_t::VkMediaPlayPause;
    mcl_key_t::type constexpr mcl_key_t::VkLaunchMail;
    mcl_key_t::type constexpr mcl_key_t::VkLaunchMediaSelect;
    mcl_key_t::type constexpr mcl_key_t::VkLaunchApp1;
    mcl_key_t::type constexpr mcl_key_t::VkLaunchApp2;
    mcl_key_t::type constexpr mcl_key_t::VkOEM1;
    mcl_key_t::type constexpr mcl_key_t::VkOEMPlus;
    mcl_key_t::type constexpr mcl_key_t::VkOEMComma;
    mcl_key_t::type constexpr mcl_key_t::VkOEMMinus;
    mcl_key_t::type constexpr mcl_key_t::VkOEMPeriod;
    mcl_key_t::type constexpr mcl_key_t::VkOEM2;
    mcl_key_t::type constexpr mcl_key_t::VkOEM3;
    mcl_key_t::type constexpr mcl_key_t::VkOEM4;
    mcl_key_t::type constexpr mcl_key_t::VkOEM5;
    mcl_key_t::type constexpr mcl_key_t::VkOEM6;
    mcl_key_t::type constexpr mcl_key_t::VkOEM7;
    mcl_key_t::type constexpr mcl_key_t::VkOEM8;
    mcl_key_t::type constexpr mcl_key_t::VkOEM102;
    mcl_key_t::type constexpr mcl_key_t::VkProcessKey;
    mcl_key_t::type constexpr mcl_key_t::VkPacket;
    mcl_key_t::type constexpr mcl_key_t::VkAttn;
    mcl_key_t::type constexpr mcl_key_t::VkCrsel;
    mcl_key_t::type constexpr mcl_key_t::VkExsel;
    mcl_key_t::type constexpr mcl_key_t::VkErEOF;
    mcl_key_t::type constexpr mcl_key_t::VkPlay;
    mcl_key_t::type constexpr mcl_key_t::VkZoom;
    mcl_key_t::type constexpr mcl_key_t::VkNoName;
    mcl_key_t::type constexpr mcl_key_t::VkPa1;
    mcl_key_t::type constexpr mcl_key_t::VkOemClear;

    mcl_key_t::mod_type constexpr mcl_key_t::ModNone;
    mcl_key_t::mod_type constexpr mcl_key_t::ModLShift;
    mcl_key_t::mod_type constexpr mcl_key_t::ModRShift;
    mcl_key_t::mod_type constexpr mcl_key_t::ModShift;
    mcl_key_t::mod_type constexpr mcl_key_t::ModLCtrl;
    mcl_key_t::mod_type constexpr mcl_key_t::ModRCtrl;
    mcl_key_t::mod_type constexpr mcl_key_t::ModCtrl;
    mcl_key_t::mod_type constexpr mcl_key_t::ModLAlt;
    mcl_key_t::mod_type constexpr mcl_key_t::ModRAlt;
    mcl_key_t::mod_type constexpr mcl_key_t::ModAlt;
    mcl_key_t::mod_type constexpr mcl_key_t::ModLMeta;
    mcl_key_t::mod_type constexpr mcl_key_t::ModRMeta;
    mcl_key_t::mod_type constexpr mcl_key_t::ModMeta;
    mcl_key_t::mod_type constexpr mcl_key_t::ModCaps;
    mcl_key_t::mod_type constexpr mcl_key_t::ModNum;
    mcl_key_t::mod_type constexpr mcl_key_t::ModMode;
#endif

    /**
     * @function mcl_key_t::get_focused <src/key.h>
     * @brief Check if the display is receiving keyboard input
     * @return bool
     */
    bool mcl_key_t::
    get_focused () noexcept{
        return mcl_control_obj.bIsReady
            && ((mcl_control_obj.bHasIMFocus & 1) || mcl_control_obj.hWndKeyGrabed);
    }

    /**
     * @function mcl_key_t::get_mods <src/key.h>
     * @brief Determine which modifier keys are being held
     * @return wchar_t
     */
    mcl_key_t::mod_type mcl_key_t::
    get_mods () noexcept{
        return mcl_control_obj.fModKeyState;
    }

    /**
     * @function mcl_key_t::get_pressed <src/key.h>
     * @brief Get the state of all keyboard buttons
     * @return bools
     */
    std::vector<bool> mcl_key_t::
    get_pressed () noexcept{
        std::vector<bool> lhs(256, 0);
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.keymaplock, L"mcl_key_t::get_pressed");
        {
        std::vector<BYTE>* rhs =
            reinterpret_cast<std::vector<BYTE>*>(mcl_control_obj.keymap);        
        if (rhs)
            for (size_t i = 0; i != 256u; ++ i)
                lhs[i] = bool((*rhs)[i] & 0x80);
        }
        return lhs;
    }

    /**
     * @function mcl_key_t::get_async_pressed <src/key.h>
     * @brief Determine whether a key is up or down. focus not required
     * @return bool
     */
    bool mcl_key_t::
    get_async_pressed (type vk_index) noexcept{
        return ::GetAsyncKeyState (vk_index) & 0x8000;
    }

    /**
     * @function mcl_key_t::get_async_mods <src/key.h>
     * @brief Determine whether a modifier key is up or down. focus not required
     * @return bool
     */
    mcl_key_t::mod_type mcl_key_t::
    get_async_mods (mod_type mod_mask) noexcept{
        mod_type mods = 0;
        if (mod_mask & ModLShift && (::GetAsyncKeyState (VK_LSHIFT)   & 0x8000)) mods |= ModLShift;
        if (mod_mask & ModRShift && (::GetAsyncKeyState (VK_RSHIFT)   & 0x8000)) mods |= ModRShift;
        if (mod_mask & ModLCtrl  && (::GetAsyncKeyState (VK_LCONTROL) & 0x8000)) mods |= ModLCtrl;
        if (mod_mask & ModRCtrl  && (::GetAsyncKeyState (VK_RCONTROL) & 0x8000)) mods |= ModRCtrl;
        if (mod_mask & ModLAlt   && (::GetAsyncKeyState (VK_LMENU)    & 0x8000)) mods |= ModLAlt;
        if (mod_mask & ModRAlt   && (::GetAsyncKeyState (VK_RMENU)    & 0x8000)) mods |= ModRAlt;
        if (mod_mask & ModLMeta  && (::GetAsyncKeyState (VK_LWIN)     & 0x8000)) mods |= ModLMeta;
        if (mod_mask & ModRMeta  && (::GetAsyncKeyState (VK_RWIN)     & 0x8000)) mods |= ModRMeta;
        if (mod_mask & ModCaps   && (::GetKeyState      (VK_CAPITAL)  & 0x1)   ) mods |= ModCaps;
        if (mod_mask & ModNum    && (::GetKeyState      (VK_NUMLOCK)  & 0x1)   ) mods |= ModNum;
        if (mod_mask & ModMode   && (::GetAsyncKeyState (VK_LCONTROL)
                                    &::GetAsyncKeyState (VK_RMENU)    & 0x8000)) mods |= ModMode;
        return mods;
    }

    /**
     * @function mcl_key_t::set_repeat <src/key.h>
     * @brief Control how held keys are repeated
     * @return bool
     */
    bool mcl_key_t::
    set_repeat (bool b_repeat) noexcept{
        bool prev = mcl_control_obj.bRepeatCount;
        mcl_control_obj.bRepeatCount = b_repeat;
        return prev;
    }

    /**
     * @function mcl_key_t::get_repeat <src/key.h>
     * @brief See how held keys are repeated
     * @return bool
     */
    bool mcl_key_t::
    get_repeat () noexcept{
        return mcl_control_obj.bRepeatCount;
    }

    /**
     * @function mcl_key_t::name <src/key.h>
     * @brief Get the name of a key identifier
     * @return str
     */
    wchar_t const* mcl_key_t::
    name (type vkey) noexcept{
        switch (vkey) {
            case VkNul:        return L"nul";
            case VkCancel:     return L"cancel";
            case VkBack:       return L"back";
            case VkTab:        return L"tab";
            case VkClear:      return L"clear";
            case VkReturn:     return L"return";
            case VkShift:      return L"shift";
            case VkControl:    return L"control";
            case VkMenu:       return L"menu";
            case VkPause:      return L"pause";
            case VkCapital:    return L"capital";
            case VkKana:       return L"kana";
            case VkIMEOn:      return L"ime_on";
            case VkJunja:      return L"junja";
            case VkFinal:      return L"final";
            case VkKanji:      return L"kanji";
            case VkIMEOff:     return L"ime_off";
            case VkEscape:     return L"escape";
            case VkConvert:    return L"convert";
            case VkNonConvert: return L"non_convert";
            case VkAccept:     return L"accept";
            case VkModeChange: return L"mode_change";
            case VkSpace:      return L"space";
            case VkPrior:      return L"prior";
            case VkNext:       return L"next";
            case VkEnd:        return L"end";
            case VkHome:       return L"home";
            case VkLeft:       return L"left";
            case VkUp:         return L"up";
            case VkRight:      return L"right";
            case VkDown:       return L"down";
            case VkSelect:     return L"select";
            case VkPrint:      return L"print";
            case VkExecute:    return L"execute";
            case VkSnapshot:   return L"snapshot";
            case VkInsert:     return L"insert";
            case VkDelete:     return L"delete";
            case VkHelp:       return L"help";
            case Vk0:          return L"0";
            case Vk1:          return L"1";
            case Vk2:          return L"2";
            case Vk3:          return L"3";
            case Vk4:          return L"4";
            case Vk5:          return L"5";
            case Vk6:          return L"6";
            case Vk7:          return L"7";
            case Vk8:          return L"8";
            case Vk9:          return L"9";
            case VkA:          return L"a"; 
            case VkB:          return L"b";
            case VkC:          return L"c";
            case VkD:          return L"d";
            case VkE:          return L"e";
            case VkF:          return L"f";
            case VkG:          return L"g";
            case VkH:          return L"h";
            case VkI:          return L"i";
            case VkJ:          return L"j";
            case VkK:          return L"k";
            case VkL:          return L"l";
            case VkM:          return L"m";
            case VkN:          return L"n";
            case VkO:          return L"o";
            case VkP:          return L"p";
            case VkQ:          return L"q";
            case VkR:          return L"r";
            case VkS:          return L"s";
            case VkT:          return L"t";
            case VkU:          return L"u";
            case VkV:          return L"v";
            case VkW:          return L"w";
            case VkX:          return L"x";
            case VkY:          return L"y";
            case VkZ:          return L"z";
            case VkLWin:       return L"lwin";
            case VkRWin:       return L"rwin";
            case VkApps:       return L"apps";
            case VkSleep:      return L"sleep";
            case VkNumpad0:    return L"numpad_0";
            case VkNumpad1:    return L"numpad_1";
            case VkNumpad2:    return L"numpad_2";
            case VkNumpad3:    return L"numpad_3";
            case VkNumpad4:    return L"numpad_4";
            case VkNumpad5:    return L"numpad_5";
            case VkNumpad6:    return L"numpad_6";
            case VkNumpad7:    return L"numpad_7";
            case VkNumpad8:    return L"numpad_8";
            case VkNumpad9:    return L"numpad_9";
            case VkMultiply:   return L"multiply";
            case VkAdd:        return L"add";
            case VkSeparator:  return L"separator";
            case VkSubtract:   return L"subtract";
            case VkDecimal:    return L"decimal";
            case VkDivide:     return L"divide";
            case VkF1:         return L"f1";
            case VkF2:         return L"f2";
            case VkF3:         return L"f3";
            case VkF4:         return L"f4";
            case VkF5:         return L"f5";
            case VkF6:         return L"f6";
            case VkF7:         return L"f7";
            case VkF8:         return L"f8";
            case VkF9:         return L"f9";
            case VkF10:        return L"f10";
            case VkF11:        return L"f11";
            case VkF12:        return L"f12";
            case VkF13:        return L"f13";
            case VkF14:        return L"f14";
            case VkF15:        return L"f15";
            case VkF16:        return L"f16";
            case VkF17:        return L"f17";
            case VkF18:        return L"f18";
            case VkF19:        return L"f19";
            case VkF20:        return L"f20";
            case VkF21:        return L"f21";
            case VkF22:        return L"f22";
            case VkF23:        return L"f23";
            case VkF24:        return L"f24";
            case VkNumlock:    return L"numlock";
            case VkScroll:     return L"scroll";
            case VkLShift:     return L"lshift";
            case VkRShift:     return L"rshift";
            case VkLControl:   return L"lcontrol";
            case VkRControl:   return L"rcontrol";
            case VkLMenu:      return L"lmenu";
            case VkRMenu:      return L"rmenu";
            case VkBrowserBack:       return L"browser_back";
            case VkBrowserForward:    return L"browser_forward";
            case VkBrowserRefresh:    return L"browser_refresh";
            case VkBrowserStop:       return L"browser_stop";
            case VkBrowserSearch:     return L"browser_search";
            case VkBrowserFavorites:  return L"browser_favorites";
            case VkBrowserHome:       return L"browser_home";
            case VkVolumeMute:        return L"volume_mute";
            case VkVolumeDown:        return L"volume_down";
            case VkVolumeUp:          return L"volume_up";
            case VkMediaNextTrack:    return L"media_next_track";
            case VkMediaPrevTrack:    return L"media_prev_track";
            case VkMediaStop:         return L"media_stop";
            case VkMediaPlayPause:    return L"media_play_pause";
            case VkLaunchMail:        return L"launchmail";
            case VkLaunchMediaSelect: return L"launch_media_select";
            case VkLaunchApp1:        return L"launch_app1";
            case VkLaunchApp2:        return L"launch_app2";
            case VkOEM1:       return L"oem_1";
            case VkOEMPlus:    return L"oem_plus";
            case VkOEMComma:   return L"oem_comma";
            case VkOEMMinus:   return L"oem_minus";
            case VkOEMPeriod:  return L"oem_period";
            case VkOEM2:       return L"oem_2";
            case VkOEM3:       return L"oem_3";
            case VkOEM4:       return L"oem_4";
            case VkOEM5:       return L"oem_5";
            case VkOEM6:       return L"oem_6";
            case VkOEM7:       return L"oem_7";
            case VkOEM8:       return L"oem_8";
            case VkOEM102:     return L"oem_102";
            case VkProcessKey: return L"process_key";
            case VkPacket:     return L"packet";
            case VkAttn:       return L"attn";
            case VkCrsel:      return L"crsel";
            case VkExsel:      return L"exsel";
            case VkErEOF:      return L"er_eof";
            case VkPlay:       return L"play";
            case VkZoom:       return L"zoom";
            case VkNoName:     return L"no_name";
            case VkPa1:        return L"pa1";
            case VkOemClear:   return L"oem_clear";
        }
        return L"unknown";
    }

    char const* mcl_key_t::
    name_a (type vkey) noexcept{
        switch (vkey) {
            case VkNul:        return "nul";
            case VkCancel:     return "cancel";
            case VkBack:       return "back";
            case VkTab:        return "tab";
            case VkClear:      return "clear";
            case VkReturn:     return "return";
            case VkShift:      return "shift";
            case VkControl:    return "control";
            case VkMenu:       return "menu";
            case VkPause:      return "pause";
            case VkCapital:    return "capital";
            case VkKana:       return "kana";
            case VkIMEOn:      return "ime_on";
            case VkJunja:      return "junja";
            case VkFinal:      return "final";
            case VkKanji:      return "kanji";
            case VkIMEOff:     return "ime_off";
            case VkEscape:     return "escape";
            case VkConvert:    return "convert";
            case VkNonConvert: return "non_convert";
            case VkAccept:     return "accept";
            case VkModeChange: return "mode_change";
            case VkSpace:      return "space";
            case VkPrior:      return "prior";
            case VkNext:       return "next";
            case VkEnd:        return "end";
            case VkHome:       return "home";
            case VkLeft:       return "left";
            case VkUp:         return "up";
            case VkRight:      return "right";
            case VkDown:       return "down";
            case VkSelect:     return "select";
            case VkPrint:      return "print";
            case VkExecute:    return "execute";
            case VkSnapshot:   return "snapshot";
            case VkInsert:     return "insert";
            case VkDelete:     return "delete";
            case VkHelp:       return "help";
            case Vk0:          return "0";
            case Vk1:          return "1";
            case Vk2:          return "2";
            case Vk3:          return "3";
            case Vk4:          return "4";
            case Vk5:          return "5";
            case Vk6:          return "6";
            case Vk7:          return "7";
            case Vk8:          return "8";
            case Vk9:          return "9";
            case VkA:          return "a"; 
            case VkB:          return "b";
            case VkC:          return "c";
            case VkD:          return "d";
            case VkE:          return "e";
            case VkF:          return "f";
            case VkG:          return "g";
            case VkH:          return "h";
            case VkI:          return "i";
            case VkJ:          return "j";
            case VkK:          return "k";
            case VkL:          return "l";
            case VkM:          return "m";
            case VkN:          return "n";
            case VkO:          return "o";
            case VkP:          return "p";
            case VkQ:          return "q";
            case VkR:          return "r";
            case VkS:          return "s";
            case VkT:          return "t";
            case VkU:          return "u";
            case VkV:          return "v";
            case VkW:          return "w";
            case VkX:          return "x";
            case VkY:          return "y";
            case VkZ:          return "z";
            case VkLWin:       return "lwin";
            case VkRWin:       return "rwin";
            case VkApps:       return "apps";
            case VkSleep:      return "sleep";
            case VkNumpad0:    return "numpad_0";
            case VkNumpad1:    return "numpad_1";
            case VkNumpad2:    return "numpad_2";
            case VkNumpad3:    return "numpad_3";
            case VkNumpad4:    return "numpad_4";
            case VkNumpad5:    return "numpad_5";
            case VkNumpad6:    return "numpad_6";
            case VkNumpad7:    return "numpad_7";
            case VkNumpad8:    return "numpad_8";
            case VkNumpad9:    return "numpad_9";
            case VkMultiply:   return "multiply";
            case VkAdd:        return "add";
            case VkSeparator:  return "separator";
            case VkSubtract:   return "subtract";
            case VkDecimal:    return "decimal";
            case VkDivide:     return "divide";
            case VkF1:         return "f1";
            case VkF2:         return "f2";
            case VkF3:         return "f3";
            case VkF4:         return "f4";
            case VkF5:         return "f5";
            case VkF6:         return "f6";
            case VkF7:         return "f7";
            case VkF8:         return "f8";
            case VkF9:         return "f9";
            case VkF10:        return "f10";
            case VkF11:        return "f11";
            case VkF12:        return "f12";
            case VkF13:        return "f13";
            case VkF14:        return "f14";
            case VkF15:        return "f15";
            case VkF16:        return "f16";
            case VkF17:        return "f17";
            case VkF18:        return "f18";
            case VkF19:        return "f19";
            case VkF20:        return "f20";
            case VkF21:        return "f21";
            case VkF22:        return "f22";
            case VkF23:        return "f23";
            case VkF24:        return "f24";
            case VkNumlock:    return "numlock";
            case VkScroll:     return "scroll";
            case VkLShift:     return "lshift";
            case VkRShift:     return "rshift";
            case VkLControl:   return "lcontrol";
            case VkRControl:   return "rcontrol";
            case VkLMenu:      return "lmenu";
            case VkRMenu:      return "rmenu";
            case VkBrowserBack:       return "browser_back";
            case VkBrowserForward:    return "browser_forward";
            case VkBrowserRefresh:    return "browser_refresh";
            case VkBrowserStop:       return "browser_stop";
            case VkBrowserSearch:     return "browser_search";
            case VkBrowserFavorites:  return "browser_favorites";
            case VkBrowserHome:       return "browser_home";
            case VkVolumeMute:        return "volume_mute";
            case VkVolumeDown:        return "volume_down";
            case VkVolumeUp:          return "volume_up";
            case VkMediaNextTrack:    return "media_next_track";
            case VkMediaPrevTrack:    return "media_prev_track";
            case VkMediaStop:         return "media_stop";
            case VkMediaPlayPause:    return "media_play_pause";
            case VkLaunchMail:        return "launchmail";
            case VkLaunchMediaSelect: return "launch_media_select";
            case VkLaunchApp1:        return "launch_app1";
            case VkLaunchApp2:        return "launch_app2";
            case VkOEM1:       return "oem_1";
            case VkOEMPlus:    return "oem_plus";
            case VkOEMComma:   return "oem_comma";
            case VkOEMMinus:   return "oem_minus";
            case VkOEMPeriod:  return "oem_period";
            case VkOEM2:       return "oem_2";
            case VkOEM3:       return "oem_3";
            case VkOEM4:       return "oem_4";
            case VkOEM5:       return "oem_5";
            case VkOEM6:       return "oem_6";
            case VkOEM7:       return "oem_7";
            case VkOEM8:       return "oem_8";
            case VkOEM102:     return "oem_102";
            case VkProcessKey: return "process_key";
            case VkPacket:     return "packet";
            case VkAttn:       return "attn";
            case VkCrsel:      return "crsel";
            case VkExsel:      return "exsel";
            case VkErEOF:      return "er_eof";
            case VkPlay:       return "play";
            case VkZoom:       return "zoom";
            case VkNoName:     return "no_name";
            case VkPa1:        return "pa1";
            case VkOemClear:   return "oem_clear";
        }
        return "unknown";
    }

    void mcl_key_t::
    start_text_input () noexcept{
        ::SendMessage (mcl_control_obj.hwnd, WM_IME_NOTIFY, 0, 1);
    }

    void mcl_key_t::
    stop_text_input () noexcept{
        ::SendMessage (mcl_control_obj.hwnd, WM_IME_NOTIFY, 0, 0);
    }

    void mcl_key_t::
    set_text_input_rect (rect_t rect, point2d_t start) noexcept{
        using igdiw_t = decltype(ImmGetDefaultIMEWnd);
        igdiw_t* fpImmGetDefaultIMEWnd = mcl_get_immfunc<igdiw_t>("ImmGetDefaultIMEWnd");
        if (!fpImmGetDefaultIMEWnd) return ;

        if (!rect.w) rect.w = 1;
        if (!rect.h) rect.h = 1;
        if (rect.w < 0) rect.x += rect.w + 1, rect.w = -rect.w;
        if (rect.h < 0) rect.y += rect.h + 1, rect.h = -rect.h;

        COMPOSITIONFORM cpf{ 0, {0, 0}, {0, 0, 0, 0} };
        cpf.dwStyle = CFS_FORCE_POSITION | CFS_RECT;
        cpf.rcArea.left = rect.x;
        cpf.rcArea.top  = rect.y;
        cpf.rcArea.right = rect.x + rect.w;
        cpf.rcArea.bottom = rect.y + rect.h;
        
        /*
        // Make sure that the rect is inside the display window
        if (cpf.rcArea.left   >= mcl_control_obj.dc_w) cpf.rcArea.left   = mcl_control_obj.dc_w - 1;
        if (cpf.rcArea.top    >= mcl_control_obj.dc_h) cpf.rcArea.top    = mcl_control_obj.dc_h - 1;
        if (cpf.rcArea.right  >  mcl_control_obj.dc_w) cpf.rcArea.right  = mcl_control_obj.dc_w;
        if (cpf.rcArea.bottom >  mcl_control_obj.dc_h) cpf.rcArea.bottom = mcl_control_obj.dc_h;

        if (cpf.rcArea.left   <  0) cpf.rcArea.left   = 0;
        if (cpf.rcArea.top    <  0) cpf.rcArea.top    = 0;
        if (cpf.rcArea.right  <= 0) cpf.rcArea.right  = 1;
        if (cpf.rcArea.bottom <= 0) cpf.rcArea.bottom = 1;
        */
        
        if (start.x == -1 && start.y == -1) {
            cpf.ptCurrentPos.x = cpf.rcArea.left;
            cpf.ptCurrentPos.y = cpf.rcArea.top;
        } else {
            cpf.ptCurrentPos.x = start.x;
            cpf.ptCurrentPos.y = start.y;

            if (cpf.ptCurrentPos.x <  cpf.rcArea.left)   cpf.ptCurrentPos.x = cpf.rcArea.left;
            if (cpf.ptCurrentPos.y <  cpf.rcArea.top)    cpf.ptCurrentPos.y = cpf.rcArea.top;
            if (cpf.ptCurrentPos.x >= cpf.rcArea.right)  cpf.ptCurrentPos.x = cpf.rcArea.right - 1;
            if (cpf.ptCurrentPos.y >= cpf.rcArea.bottom) cpf.ptCurrentPos.y = cpf.rcArea.bottom - 1;
        }

        if (cpf.ptCurrentPos.x != mcl_control_obj.immcpf.ptCurrentPos.x || 
            cpf.ptCurrentPos.y != mcl_control_obj.immcpf.ptCurrentPos.y ||
            cpf.rcArea.left    != mcl_control_obj.immcpf.rcArea.left    ||
            cpf.rcArea.top     != mcl_control_obj.immcpf.rcArea.top     ||
            cpf.rcArea.right   != mcl_control_obj.immcpf.rcArea.right   ||
            cpf.rcArea.bottom  != mcl_control_obj.immcpf.rcArea.bottom
        ) {
            HWND idefhwnd = fpImmGetDefaultIMEWnd (mcl_control_obj.hwnd);
            if (!idefhwnd) return ;
            ::SendMessage (idefhwnd, WM_IME_CONTROL, IMC_SETCOMPOSITIONWINDOW,
                reinterpret_cast<LPARAM>(reinterpret_cast<void*>(&cpf)));
            // Also: Use IMN_SETCOMPOSITIONFONT to change font size.
            mcl_control_obj.immcpf = cpf;
        }
    }

    void mcl_key_t::
    set_text_input_rect (rect_t rect) noexcept{
        set_text_input_rect (rect, { -1, -1 });
    }

}
