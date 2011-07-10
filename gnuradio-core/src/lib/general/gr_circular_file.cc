/* -*- c++ -*- */
/*
 * Copyright 2002,2010 Free Software Foundation, Inc.
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
#include <config.h>
#endif

#include <gr_circular_file.h>

#include <unistd.h>
#ifdef HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <stdio.h>
#include <string.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

static const int HEADER_SIZE = 4096;
static const int HEADER_MAGIC = 0xEB021026;

static const int HD_MAGIC = 0;
static const int HD_HEADER_SIZE = 1;	// integer offsets into header
static const int HD_BUFFER_SIZE = 2;
static const int HD_BUFFER_BASE = 3;
static const int HD_BUFFER_CURRENT = 4;

gr_circular_file::gr_circular_file (const char *filename,
				    bool writable, int size)
  : d_fd (-1), d_header (0), d_buffer (0), d_mapped_size (0), d_bytes_read (0)
{
  int	mm_prot;
  if (writable){
#ifdef HAVE_MMAP
    mm_prot = PROT_READ | PROT_WRITE;
#endif
    d_fd = open (filename, O_CREAT | O_RDWR | O_TRUNC, 0664);
    if (d_fd < 0){
      perror (filename);
      exit (1);
    }
#ifdef HAVE_MMAP	/* FIXME */
    if(ftruncate (d_fd, size + HEADER_SIZE) != 0) {
      perror (filename);
      exit (1);
    }
#endif
  }
  else {
#ifdef HAVE_MMAP
    mm_prot = PROT_READ;
#endif
    d_fd = open (filename, O_RDONLY);
    if (d_fd < 0){
      perror (filename);
      exit (1);
    }
  }

  struct stat statbuf;
  if (fstat (d_fd, &statbuf) < 0){
    perror (filename);
    exit (1);
  }

  if (statbuf.st_size < HEADER_SIZE){
    fprintf (stderr, "%s: file too small to be circular buffer\n", filename);
    exit (1);
  }

  d_mapped_size = statbuf.st_size;
#ifdef HAVE_MMAP
  void *p = mmap (0, d_mapped_size, mm_prot, MAP_SHARED, d_fd, 0);
  if (p == MAP_FAILED){
    perror ("gr_circular_file: mmap failed");
    exit (1);
  }

  d_header = (int *) p;
#else
    perror ("gr_circular_file: mmap unsupported by this system");
    exit (1);
#endif

  if (writable){       	// init header

    if (size < 0){
      fprintf (stderr, "gr_circular_buffer: size must be > 0 when writable\n");
      exit (1);
    }

    d_header[HD_MAGIC] = HEADER_MAGIC;
    d_header[HD_HEADER_SIZE] = HEADER_SIZE;
    d_header[HD_BUFFER_SIZE] = size;
    d_header[HD_BUFFER_BASE] = HEADER_SIZE;	// right after header
    d_header[HD_BUFFER_CURRENT] = 0;
  }

  // sanity check (the asserts are a bit unforgiving...)

  assert (d_header[HD_MAGIC] == HEADER_MAGIC);
  assert (d_header[HD_HEADER_SIZE] == HEADER_SIZE);
  assert (d_header[HD_BUFFER_SIZE] > 0);
  assert (d_header[HD_BUFFER_BASE] >= d_header[HD_HEADER_SIZE]);
  assert (d_header[HD_BUFFER_BASE] + d_header[HD_BUFFER_SIZE] <= d_mapped_size);
  assert (d_header[HD_BUFFER_CURRENT] >= 0 &&
	  d_header[HD_BUFFER_CURRENT] < d_header[HD_BUFFER_SIZE]);

  d_bytes_read = 0;
  d_buffer = (unsigned char *) d_header + d_header[HD_BUFFER_BASE];
}

gr_circular_file::~gr_circular_file ()
{
#ifdef HAVE_MMAP
  if (munmap ((char *) d_header, d_mapped_size) < 0){
    perror ("gr_circular_file: munmap");
    exit (1);
  }
#endif
  close (d_fd);
}

bool
gr_circular_file::write (void *vdata, int nbytes)
{
  unsigned char *data = (unsigned char *) vdata;
  int	buffer_size = d_header[HD_BUFFER_SIZE];
  int	buffer_current = d_header[HD_BUFFER_CURRENT];
  
  while (nbytes > 0){
    int n = std::min (nbytes, buffer_size - buffer_current);
    memcpy (d_buffer + buffer_current, data, n);
    
    buffer_current += n;
    if (buffer_current >= buffer_size)
      buffer_current = 0;
    
    data += n;
    nbytes -= n;
  }

  d_header[HD_BUFFER_CURRENT] = buffer_current;
  return true;
}

int
gr_circular_file::read (void *vdata, int nbytes)
{
  unsigned char *data = (unsigned char *) vdata;
  int	buffer_current = d_header[HD_BUFFER_CURRENT];
  int	buffer_size = d_header[HD_BUFFER_SIZE];
  int	total = 0;
  
  nbytes = std::min (nbytes, buffer_size - d_bytes_read);

  while (nbytes > 0){
    int offset = (buffer_current + d_bytes_read) % buffer_size;
    int n = std::min (nbytes, buffer_size - offset);
    memcpy (data, d_buffer + offset, n);
    data += n;
    d_bytes_read += n;
    total += n;
    nbytes -= n;
  }
  return total;
}

void
gr_circular_file::reset_read_pointer ()
{
  d_bytes_read = 0;
}
