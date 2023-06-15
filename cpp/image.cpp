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

    @file cpp/image.cpp
    This is a C++11 implementation file for image transfer.
*/

#ifdef _MSC_VER
# pragma warning(push)
# pragma warning(disable: 4464)
#endif // Relative paths include ".."

#include "../src/image.h"
#include "../src/surface.h"
#include "mcl_control.h"

#include "../src/clog4m.h"

#ifdef _MSC_VER
# pragma warning(pop)
#endif

namespace
mcl {

    // Module for image transfer.
    mcl_image_t image;
    
    mcl_image_t::operator void* () const noexcept{ return const_cast<mcl_image_t*>(this); }
    bool mcl_image_t::operator! () const noexcept{ return false; }

    /**
     * @function mcl_image_t::load <src/surface.cpp>
     * @brief load new image from a file.
     * @param filename: path to the image file.
     * @return surface_t
     */
    surface_t mcl_image_t::
    load (char const* filename, void*) noexcept{
        if (!filename)
            return *reinterpret_cast<surface_t*>(0);
        mcl_simpletls_ns::mcl_m2w_str_t wstr(filename);
        return load (static_cast<wchar_t*>(wstr), {0, 0});
    }

    /**
     * @function mcl_image_t::load <src/surface.cpp>
     * @brief load new image from a file.
     * @param filename: path to the image file.
     * @param size: size of the dst surface.
     * @return surface_t
     */
    surface_t mcl_image_t::
    load (char const* filename, point2d_t size) noexcept{
        if (!filename)
            return *reinterpret_cast<surface_t*>(0);
        mcl_simpletls_ns::mcl_m2w_str_t wstr(filename);
        return load (static_cast<wchar_t*>(wstr), size);
    }

    /**
     * @function mcl_image_t::load <src/surface.cpp>
     * @brief load new image from a file.
     * @param filename: path to the image file.
     * @return surface_t
     */
    surface_t mcl_image_t::
    load (wchar_t const* filename, void*) noexcept{
        return load (filename, {0, 0});
    }

    /**
     * @function mcl_image_t::load <src/surface.cpp>
     * @brief load new image from a file.
     * @param filename: path to the image file.
     * @param size: size of the dst surface.
     * @return surface_t
     */
    surface_t mcl_image_t::
    load (wchar_t const* filename, point2d_t size) noexcept{
        if (!filename)
            return *reinterpret_cast<surface_t*>(0);

        // Load the image from file
        HBITMAP hbmp = static_cast<HBITMAP>(
            ::LoadImageW (nullptr, filename, IMAGE_BITMAP,
                size.x, size.y, LR_LOADFROMFILE) );
        if (!hbmp) {
            clog4m[cll4m.Info] << L"info:  Failed to load image file \'"
                << filename << L"\' [-WImage-bmp-loadbmp]\n";
            return *reinterpret_cast<surface_t*>(0);
        }
        
        // Create a dc that is compatible with the window
        HDC hldc = nullptr, refdc = nullptr;
        if (mcl_control_obj.bIsReady)
            refdc = ::GetDC (mcl_control_obj.hwnd);

        hldc = ::CreateCompatibleDC (refdc);
        if (!hldc) {
            ::DeleteObject (hbmp);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return *reinterpret_cast<surface_t*>(0);
        }

        // Get the bmp's params
        BITMAP qbmp{ 0, 0, 0, 0, 0, 0, 0 };
        int retgo = ::GetObjectW (reinterpret_cast<HGDIOBJ>(hbmp),
            sizeof (BITMAP), reinterpret_cast<LPVOID>(&qbmp));
        if (!retgo) {
            ::DeleteDC (hldc);
            ::DeleteObject (hbmp);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return *reinterpret_cast<surface_t*>(0);
        }
            
        // Select loaded bmp into dc
        HBITMAP hbmpold = static_cast<HBITMAP>(
            ::SelectObject (hldc, hbmp) );
        if (!hbmpold) {
            ::DeleteDC (hldc);
            ::DeleteObject (hbmp);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return *reinterpret_cast<surface_t*>(0);
        }            

        // Create a surface
        surface_t surf;
        mcl_imagebuf_t*& ibuf = *reinterpret_cast<mcl_imagebuf_t**>(&surf);
        ibuf = new (std::nothrow) mcl_imagebuf_t{ qbmp.bmWidth, qbmp.bmHeight };
        if (!surf) {
            ::SelectObject (hldc, hbmpold);
            ::DeleteDC (hldc);
            ::DeleteObject (hbmp);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return *reinterpret_cast<surface_t*>(0);
        }
        
        // Blit the dc
        char* cbuf = mcl_get_surface_data (&surf);
        BOOL retbilt = ::BitBlt (ibuf -> m_hdc, 0, 0,
            qbmp.bmWidth, qbmp.bmHeight, hldc, 0, 0, SRCCOPY);
        
        // Unitialize
        ::SelectObject (hldc, hbmpold);
        ::DeleteDC (hldc);
        ::DeleteObject (hbmp);
        
        if (!retbilt)
            return *reinterpret_cast<surface_t*>(0);
        if (cbuf[0]) {
            // Convert into per pixel transparency
            for (color_t* p = ibuf -> m_pbuffer,
                *e = p + static_cast<long long>(ibuf -> m_width) * ibuf -> m_height;
                p != e; ++p) {
                *p |= 0xff000000;
            }
        }
        if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
        return surf;
    }

    /**
     * @function mcl_image_t::save <src/surface.cpp>
     * @brief save an image to file.
     * @param surface: surface to save.
     * @param filename: name to the image file.
     * @return bool
     */
    bool mcl_image_t::
    save (surface_t const& surface, char const* filename) noexcept{
        if (!filename)
            return false;
        mcl_simpletls_ns::mcl_m2w_str_t wstr(filename);
        return save (surface, static_cast<wchar_t*>(wstr));
    }

    /**
     * @function mcl_image_t::save <src/surface.cpp>
     * @brief save an image to file.
     * @param surface: surface to save.
     * @param filename: name to the image file.
     * @return bool
     */
    bool mcl_image_t::
    save (surface_t const& surface, wchar_t const* filename) noexcept{
        FILE* fp = nullptr;
        MCL_WFOPEN (fp, filename, L"wb");
        bool ret = save (surface, fp);
        if (fp) fclose (fp);
        return ret;
    }

    /**
     * @function mcl_image_t::save <src/surface.cpp>
     * @brief save an image to file.
     * @param surface: surface to save.
     * @param fileobj: cpp file-like object.
     * @return bool
     */
    bool mcl_image_t::
    save (surface_t const& surface, FILE* fileobj) noexcept{
        if (!(fileobj && &surface)) return false;

        // Get image data
        mcl_imagebuf_t* ibuf =
            mcl_get_surface_dataplus (&const_cast<surface_t&>(surface));
        if (!ibuf) return false;

        mcl_simpletls_ns::mcl_spinlock_t lk(ibuf -> m_nrtlock);
        if (!ibuf -> m_width) return false;

        // Write bmp header
        BITMAPFILEHEADER bmpfHead = { 0, 0, 0, 0, 0 };
        BITMAPINFOHEADER bmpinfo = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        size_t pitch = static_cast<size_t>(ibuf -> m_width) * 3;
        size_t addbit = 4 - (pitch & 3), zero = 0;
        if (pitch & 3) pitch = ((pitch + 4) & ~3);
        else addbit = 0;

        char bmpfHead_str[3] = { 'B', 'M', 0 };
        bmpfHead.bfType = *reinterpret_cast<WORD*>(bmpfHead_str);
        bmpfHead.bfOffBits = sizeof (BITMAPFILEHEADER) + sizeof (BITMAPINFOHEADER);
        bmpfHead.bfSize = static_cast<DWORD>(sizeof (BITMAPFILEHEADER)
            + sizeof (BITMAPINFOHEADER))+ static_cast<DWORD>(pitch) * ibuf -> m_height;
        bmpinfo.biSize = sizeof (BITMAPINFOHEADER);
        bmpinfo.biBitCount = 24;
        bmpinfo.biHeight = ibuf -> m_height;
        bmpinfo.biWidth = ibuf -> m_width;
        bmpinfo.biPlanes = 1;
        bmpinfo.biSizeImage = static_cast<DWORD>(pitch) * ibuf -> m_height;

        // Create a file
        ::fwrite (&bmpfHead, sizeof(bmpfHead), 1, fileobj);
        ::fwrite (&bmpinfo, sizeof(bmpinfo), 1, fileobj);

        // Write image data
        point1d_t x = 0, y = 0;
        for (y = ibuf -> m_height - 1; y >= 0; -- y) {
            for (x = 0; x < ibuf -> m_width; ++ x) {
                DWORD col = ibuf -> m_pbuffer[y * ibuf -> m_width + x];
                size_t ret = ::fwrite (&col, 3, 1, fileobj);
                if (!ret) {
                    ::fclose (fileobj);
                    return false;
                }
            }
            if (addbit)
                ::fwrite (&zero, addbit, 1, fileobj);
        }
        return true;
    }

    /**
     * @function mcl_image_t::frombuffer <src/surface.cpp>
     * @brief create a new surface that shares data
     *    inside a bytes buffer.
     * @param bytes: color buffer that shares pixel data.
     * @param size: size of the surface.
     * @return surface_t
     */
    surface_t mcl_image_t::
    frombuffer (color_t* bytes, point2d_t size) noexcept{
        if (!(bytes && size.x && size.y)) return *reinterpret_cast<surface_t*>(0);
        
        BITMAP bmp{ 0, 0, 0, 0, 0, 0, 0 };
        bmp.bmWidth = size.x;
        bmp.bmHeight = size.y;
        bmp.bmWidthBytes = size.x * 4;
        bmp.bmPlanes = 1;
        bmp.bmBitsPixel = 32;
        bmp.bmBits = reinterpret_cast<LPVOID>(bytes);

        HBITMAP hbmp = ::CreateBitmapIndirect (&bmp);
        
        // Create a memory device context
        HDC hldc = nullptr, refdc = nullptr;
        if (mcl_control_obj.bIsReady)
            refdc = ::GetDC (mcl_control_obj.hwnd);

        hldc = ::CreateCompatibleDC (refdc);
        if (!hldc) {
            ::DeleteObject (hbmp);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return *reinterpret_cast<surface_t*>(0);
        }

        // Select loaded bmp into dc
        ::SelectObject (hldc, hbmp);

        surface_t surf;
        mcl_imagebuf_t*& ibuf = *reinterpret_cast<mcl_imagebuf_t**>(&surf);
        ibuf = new (std::nothrow) mcl_imagebuf_t ();
        if (!ibuf) {
            ::DeleteObject (hbmp);
            ::DeleteDC (hldc);
            if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
            return *reinterpret_cast<surface_t*>(0);
        }
        ibuf -> m_hbmp = hbmp;
        ibuf -> m_hdc = hldc;
        ibuf -> m_pbuffer = bytes;
        ibuf -> m_width = size.x;
        ibuf -> m_height = size.y;
        
        if (refdc) ::ReleaseDC (mcl_control_obj.hwnd, refdc);
        return surf;
    }
    
}
