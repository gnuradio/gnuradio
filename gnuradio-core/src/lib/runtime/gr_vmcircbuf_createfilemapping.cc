/* -*- c++ -*- */
/*
 * Copyright 2003,2005 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
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
#include <gr_pagesize.h>
#include <gr_tmp_path.h>
#include <gr_vmcircbuf_createfilemapping.h>


gr_vmcircbuf_createfilemapping::gr_vmcircbuf_createfilemapping (int size)
  : gr_vmcircbuf (size)
{
#if !defined(HAVE_CREATEFILEMAPPING)
  fprintf (stderr, "%s: createfilemapping is not available\n",__FUNCTION__);
  throw std::runtime_error ("gr_vmcircbuf_createfilemapping");
#else
  static int s_seg_counter = 0;

  if (size <= 0 || (size % gr_pagesize ()) != 0){
    fprintf (stderr, "gr_vmcircbuf_createfilemapping: invalid size = %d\n", size);
    throw std::runtime_error ("gr_vmcircbuf_createfilemapping");
  }

  char    seg_name[1024];
  snprintf (seg_name, sizeof (seg_name), "/gnuradio-%d-%d", getpid (), s_seg_counter);

  d_handle = CreateFileMapping(INVALID_HANDLE_VALUE,    // use paging file
			       NULL,                    // default security
			       PAGE_READWRITE,          // read/write access
			       0,                       // max. object size
			       size,                    // buffer size
			       seg_name);               // name of mapping object

  s_seg_counter++;
  if (d_handle == NULL || d_handle == INVALID_HANDLE_VALUE){
    char msg[1024];
    snprintf (msg, sizeof (msg), "gr_vmcircbuf_mmap_createfilemapping: CreateFileMapping [%s] :%d", seg_name,(int)GetLastError());
    perror (msg);
    throw std::runtime_error ("gr_vmcircbuf_mmap_createfilemapping");
  }

  int i = 0;
  d_first_copy = d_second_copy = NULL;
    
  while (i++ < 8 && d_second_copy == NULL){
    // keep the first map allocation to force allocation in a new address
    // space
    LPVOID first_tmp = d_first_copy;
        
    d_first_copy =  MapViewOfFile((HANDLE)d_handle,   // handle to map object
				  FILE_MAP_WRITE,     // read/write permission
				  0,
				  0,
				  size);
   
    if (d_first_copy == NULL){
      if (first_tmp)
	UnmapViewOfFile(first_tmp);

      CloseHandle(d_handle);         // cleanup
      char msg[1024];
      snprintf (msg, sizeof (msg),
		"gr_vmcircbuf_mmap_createfilemapping: MapViewOfFile (1) :%d", (int)GetLastError());
      perror (msg);
      throw std::runtime_error ("gr_vmcircbuf_mmap_createfilemapping");
    }

    // NOTE: d_second_copy will be NULL if MapViewFileEx() fails to allocate the
    //       requested address space
    d_second_copy =  MapViewOfFileEx((HANDLE)d_handle,   // handle to map object
				     FILE_MAP_WRITE,     // read/write permission
				     0,
				     0,
				     size,
				     (char *)d_first_copy + size);//(LPVOID) ((char *)d_first_copy + size));

    if (first_tmp)
      UnmapViewOfFile(first_tmp);

#ifdef DEBUG
    fprintf (stderr,"gr_vmcircbuf_mmap_createfilemapping: contiguous? mmap %p %p %p %p, %d\n",
	     (char *)d_first_copy, (char *)d_second_copy, size, (char *)d_first_copy + size,i);
#endif
  }

  if (d_second_copy == NULL){    // cleanup
    fprintf (stderr,"gr_vmcircbuf_mmap_createfilemapping: non contiguous mmap - %p %p %p %p\n",
	     d_first_copy, d_second_copy, size, (char *)d_first_copy + size);
    UnmapViewOfFile(d_first_copy);
    CloseHandle(d_handle);                      // cleanup
    throw std::runtime_error ("gr_vmcircbuf_mmap_createfilemapping");
  }

  // Now remember the important stuff
  d_base = (char *) d_first_copy;
  d_size = size;
#endif /*HAVE_CREATEFILEMAPPING*/
}

gr_vmcircbuf_createfilemapping::~gr_vmcircbuf_createfilemapping ()
{
#ifdef HAVE_CREATEFILEMAPPING
  if (UnmapViewOfFile(d_first_copy) == 0)
  {
    perror ("gr_vmcircbuf_createfilemapping: UnmapViewOfFile(d_first_copy)");
  }
  d_base=NULL;
  if (UnmapViewOfFile(d_second_copy) == 0)
  {
    perror ("gr_vmcircbuf_createfilemapping: UnmapViewOfFile(d_second_copy)");
  }
  //d_second=NULL;
  CloseHandle(d_handle);
#endif
}

// ----------------------------------------------------------------
//      The factory interface
// ----------------------------------------------------------------


gr_vmcircbuf_factory *gr_vmcircbuf_createfilemapping_factory::s_the_factory = 0;

gr_vmcircbuf_factory *
gr_vmcircbuf_createfilemapping_factory::singleton ()
{
  if (s_the_factory)
    return s_the_factory;
  s_the_factory = new gr_vmcircbuf_createfilemapping_factory ();
  return s_the_factory;
}

int
gr_vmcircbuf_createfilemapping_factory::granularity ()
{
#ifdef HAVE_CREATEFILEMAPPING
  //  return 65536;//TODO, check, is this needed or can we just use gr_pagesize()
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  //fprintf(stderr,"win32 AllocationGranularity %p\n",(int)system_info.dwAllocationGranularity);
  return (int)system_info.dwAllocationGranularity;
#else
  return gr_pagesize ();
#endif
}

gr_vmcircbuf *
gr_vmcircbuf_createfilemapping_factory::make (int size)
{
  try
  {
    return new gr_vmcircbuf_createfilemapping (size);
  }
  catch (...)
  {
    return 0;
  }
}
