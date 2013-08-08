/* -*- c++ -*- */
/*
 * Copyright 2003,2005,2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdexcept>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <errno.h>
#include <stdio.h>
#include "pagesize.h"
#include "vmcircbuf_createfilemapping.h"
#include <boost/format.hpp>

namespace gr {

#ifdef HAVE_CREATEFILEMAPPING
  // Print Windows error (could/should be global?)
  static void
  werror(char *where, DWORD last_error)
  {
    char buf[1024];

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,
                  NULL,
                  last_error,
                  0,    // default language
                  buf,
                  sizeof(buf)/sizeof(TCHAR),    // buffer size
                  NULL);
    fprintf(stderr, "%s: Error %d: %s", where, last_error, buf);
    return;
  }
#endif


  vmcircbuf_createfilemapping::vmcircbuf_createfilemapping(int size)
    : gr::vmcircbuf(size)
  {
#if !defined(HAVE_CREATEFILEMAPPING)
    fprintf(stderr, "%s: createfilemapping is not available\n", __FUNCTION__);
    throw std::runtime_error("gr::vmcircbuf_createfilemapping");
#else
    gr::thread::scoped_lock guard(s_vm_mutex);

    static int s_seg_counter = 0;

    if(size <= 0 || (size % gr::pagesize ()) != 0) {
      fprintf(stderr, "gr::vmcircbuf_createfilemapping: invalid size = %d\n", size);
      throw std::runtime_error ("gr::vmcircbuf_createfilemapping");
    }

    std::string seg_name = str(boost::format("/gnuradio-%d-%d") % getpid() % s_seg_counter);

    d_handle = CreateFileMapping(INVALID_HANDLE_VALUE,    // use paging file
                                 NULL,                    // default security
                                 PAGE_READWRITE,          // read/write access
                                 0,                       // max. object size
                                 size,                    // buffer size
                                 seg_name.c_str());       // name of mapping object

    s_seg_counter++;
    if(d_handle == NULL || d_handle == INVALID_HANDLE_VALUE) {
      std::string msg = str(boost::format(
	      "gr::vmcircbuf_mmap_createfilemapping: CreateFileMapping [%s]") %
	      seg_name);
      werror((char*)msg.c_str(), GetLastError());
      throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    // Allocate virtual memory of the needed size, then free it so we can use it
    LPVOID first_tmp;
    first_tmp = VirtualAlloc( NULL, 2*size, MEM_RESERVE, PAGE_NOACCESS );
    if(first_tmp == NULL) {
      werror("gr::vmcircbuf_mmap_createfilemapping: VirtualAlloc", GetLastError());
      CloseHandle(d_handle);         // cleanup
      throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    if(VirtualFree(first_tmp, 0, MEM_RELEASE) == 0) {
      werror("gr::vmcircbuf_mmap_createfilemapping: VirtualFree", GetLastError());
      CloseHandle(d_handle);         // cleanup
      throw std::runtime_error("gr::vmcircbuf_mmap_createfilemapping");
    }

    d_first_copy =  MapViewOfFileEx((HANDLE)d_handle,   // handle to map object
                                    FILE_MAP_WRITE,    // read/write permission
                                    0,
                                    0,
                                    size,
                                    first_tmp);
    if(d_first_copy != first_tmp) {
      werror( "gr::vmcircbuf_mmap_createfilemapping: MapViewOfFileEx(1)", GetLastError());
      CloseHandle(d_handle);         // cleanup
      throw std::runtime_error ("gr::vmcircbuf_mmap_createfilemapping");
    }

    d_second_copy =  MapViewOfFileEx((HANDLE)d_handle,   // handle to map object
                                     FILE_MAP_WRITE,     // read/write permission
                                     0,
                                     0,
                                     size,
                                     (char*)first_tmp + size);//(LPVOID) ((char *)d_first_copy + size));

    if(d_second_copy != (char *)first_tmp + size) {
      werror( "gr::vmcircbuf_mmap_createfilemapping: MapViewOfFileEx(2)", GetLastError());
      UnmapViewOfFile(d_first_copy);
      CloseHandle(d_handle);                      // cleanup
      throw std::runtime_error ("gr::vmcircbuf_mmap_createfilemapping");
    }

#ifdef DEBUG
    fprintf(stderr,"gr::vmcircbuf_mmap_createfilemapping: contiguous? mmap %p %p %p %p\n",
            (char*)d_first_copy, (char*)d_second_copy, size, (char*)d_first_copy + size);
#endif

    // Now remember the important stuff
    d_base = (char*)d_first_copy;
    d_size = size;
#endif /*HAVE_CREATEFILEMAPPING*/
  }

  vmcircbuf_createfilemapping::~vmcircbuf_createfilemapping()
  {
#ifdef HAVE_CREATEFILEMAPPING
    gr::thread::scoped_lock guard(s_vm_mutex);

    if(UnmapViewOfFile(d_first_copy) == 0) {
      werror("gr::vmcircbuf_createfilemapping: UnmapViewOfFile(d_first_copy)", GetLastError());
    }
    d_base=NULL;
    if(UnmapViewOfFile(d_second_copy) == 0) {
      werror("gr::vmcircbuf_createfilemapping: UnmapViewOfFile(d_second_copy)", GetLastError());
    }
    //d_second=NULL;
    CloseHandle(d_handle);
#endif
  }

  // ----------------------------------------------------------------
  //      The factory interface
  // ----------------------------------------------------------------

  gr::vmcircbuf_factory *vmcircbuf_createfilemapping_factory::s_the_factory = 0;

  gr::vmcircbuf_factory *
  vmcircbuf_createfilemapping_factory::singleton()
  {
    if(s_the_factory)
      return s_the_factory;
    s_the_factory = new vmcircbuf_createfilemapping_factory();
    return s_the_factory;
  }

  int
  vmcircbuf_createfilemapping_factory::granularity()
  {
#ifdef HAVE_CREATEFILEMAPPING
    //  return 65536;//TODO, check, is this needed or can we just use gr::pagesize()
    SYSTEM_INFO system_info;
    GetSystemInfo(&system_info);
    //fprintf(stderr,"win32 AllocationGranularity %p\n",(int)system_info.dwAllocationGranularity);
    return (int)system_info.dwAllocationGranularity;
#else
    return gr::pagesize();
#endif
  }

  gr::vmcircbuf *
  vmcircbuf_createfilemapping_factory::make(int size)
  {
    try {
      return new vmcircbuf_createfilemapping(size);
    }
    catch(...) {
      return 0;
    }
  }

} /* namespace gr */
