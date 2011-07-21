/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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

#ifndef _GR_CIRCULAR_FILE_H_
#define _GR_CIRCULAR_FILE_H_

#include <gr_core_api.h>

/*
 * writes input data into a circular buffer on disk.
 *
 * the file contains a fixed header:
 *   0x0000:    int32 magic (0xEB021026)
 *   0x0004:	int32 size in bytes of header (constant 4096)
 *   0x0008:	int32 size in bytes of circular buffer (not including header)
 *   0x000C:    int32 file offset to beginning of circular buffer
 *   0x0010:    int32 byte offset from beginning of circular buffer to 
 *		      current start of data
 *
 */
class GR_CORE_API gr_circular_file {
  int		 d_fd;
  int		*d_header;
  unsigned char	*d_buffer;
  int		 d_mapped_size;
  int		 d_bytes_read;

public:
  gr_circular_file (const char *filename, bool writable = false, int size = 0);
  ~gr_circular_file ();

  bool write (void *data, int nbytes);

  // returns # of bytes actually read or 0 if end of buffer, or -1 on error.
  int read (void *data, int nbytes);

  // reset read pointer to beginning of buffer.
  void reset_read_pointer ();
};

#endif /* _GR_CIRCULAR_FILE_H_ */