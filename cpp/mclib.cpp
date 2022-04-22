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
    
    @file cpp/mclib.cpp
    This is a C++11 implementation file for the top level
    mclib package.
*/


#include "../src/mclib.h"
#include "mcl_base.h" 

namespace
mcl {
    
    // Small module containing version information
    mcl_version_t version;

    char const* const mcl_version_t::
        ver_a  = mcl_simpletls_ns::mcl_version_number_a ();
    wchar_t const* const mcl_version_t::
        ver    = mcl_simpletls_ns::mcl_version_number ();
    pytuple<int, int, int> const mcl_version_t::
        vernum{  mcl_simpletls_ns::mcl_major_number (),
                 mcl_simpletls_ns::mcl_minor_number (),
                 mcl_simpletls_ns::mcl_patch_number ()   };
    int const mcl_version_t::
        rev    = mcl_simpletls_ns::mcl_revision_number ();
    
   /**
    * @function init <src/mclib.h>
    * @brief Initialize all mclib modules
    * @return pytuple(numpass, numfail)
    */
    pytuple<int, int>
    init () noexcept{
        int numpass = 0, numfail = 0;
        if (display.init ()) ++ numpass;
        else                 ++ numfail; 
        return maktuple (numpass, numfail);
    }

   /**
    * @function quit <src/mclib.h>
    * @brief Returns true if the window has been initialized
    * @return bool
    */
    bool
    get_init () noexcept{
        return display.get_init () || mixer.music.get_busy ();
    }
    
   /**
    * @function mcl_register_quit <src/mclib.h>
    * @brief Register a function to be called when mclib quits
    * @param fun: functions that need to be called when quit() is called
    * @return bool
    */
    bool
    mcl_register_quit (std::function<void()> fun)
    noexcept {
        using _onquit_t = std::function<void()>;
        mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.quitlock);
        if (mcl_base_obj.atquit_registered
            >= mcl_base_obj.atquit_table_size)
        {
            _onquit_t* newtable;
            newtable = static_cast<_onquit_t*>(
                ::calloc (mcl_base_obj.atquit_table_size + 32,
                    sizeof (_onquit_t)));
            if (!newtable) {
                clog4m[cll4m.Warn].putws (L"warning:  Calloc failed.  "
                   L"In mcl_register_quit  [-mcl-malloc-quit]");
                return false;
            }
            if (mcl_base_obj.atquit_table) {
                ::memcpy (newtable, mcl_base_obj.atquit_table,
                    mcl_base_obj.atquit_table_size);
                ::free (mcl_base_obj.atquit_table);
            }
            mcl_base_obj.atquit_table = newtable;
        }
        mcl_base_obj.atquit_table_size += 32;
        mcl_base_obj.atquit_table
            [mcl_base_obj.atquit_registered] = fun;
        ++ mcl_base_obj.atquit_registered;
        return true;
    }
    
    void* mcl_new_mcl_logbuf_t ();
    void mcl_del_mcl_logbuf_t (void**);

   /**
    * @function quit <src/mclib.h>
    * @brief Uninitialize all mclib modules
    * @return void
    */
    void
    quit () noexcept{
        // Execute the registered function
        {
          mcl_simpletls_ns::mcl_spinlock_t lock (mcl_base_obj.quitlock);
          if (mcl_base_obj.atquit_table_size) {
              mcl_base_obj.mcl_threadid_after_exit = ::GetCurrentThreadId ();
              mcl_base_obj.mcl_clog4m_after_exit = mcl_new_mcl_logbuf_t ();

              while (mcl_base_obj.atquit_registered) try{
                  mcl_base_obj.atquit_table[-- mcl_base_obj.atquit_registered] ();
              } catch (...) {
                  clog4m[cll4m.Warn].putws (L"warning:  atquit: exception caught");
              }
              ::free (mcl_base_obj.atquit_table);
              mcl_base_obj.atquit_table_size = 0;

              mcl_base_obj.mcl_threadid_after_exit = 0;
              mcl_del_mcl_logbuf_t (&mcl_base_obj.mcl_clog4m_after_exit);
          }
        } 
        
        // Uninit all modules
        display.uninit ();
        mixer.music.unload ();
        clog4m.uninit ();
    }
    
}
