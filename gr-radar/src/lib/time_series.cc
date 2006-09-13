/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "time_series.h"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif


time_series::time_series(size_t itemsize, const std::string filename,
			 unsigned long long starting_offset,
			 long long nsamples_to_process)
  : d_itemsize(itemsize), d_filename(filename), d_fd(-1), 
    d_start(starting_offset), d_buffer(0)
{
  if ((d_fd = open(d_filename.c_str(), O_RDONLY | O_LARGEFILE, 0660)) == -1){
    perror(d_filename.c_str());
    throw std::string("open failed: ") + d_filename;
  }

  struct stat statbuf;
  if (fstat(d_fd, &statbuf) == -1){
    perror(d_filename.c_str());
    throw std::string("fstat failed: ") + d_filename;
  }
  d_filesize = statbuf.st_size;
  d_limit = d_filesize / d_itemsize;

  if (d_start > d_limit){
    std::string s = std::string("d_start > filesize: ") + d_filename;
    std::cerr << s
	      << " d_start " << d_start
	      << " d_limit " << d_limit << std::endl;
    throw s;
  }

  if (nsamples_to_process > 0)
    if ((d_start + nsamples_to_process) < d_limit)
      d_limit = d_start + nsamples_to_process;

  d_buffer = mmap(0, d_filesize, PROT_READ, MAP_SHARED, d_fd, 0);
  if (d_buffer == MAP_FAILED){
    perror("mmap");
    throw std::string("mmap failed: ") + d_filename;
  }
}

time_series::~time_series()
{
  munmap(d_buffer, d_filesize);
  close(d_fd);
}

const void *
time_series::seek(unsigned long long pos, unsigned long long blocksize) const
{
  if ((d_start + pos + blocksize) >= d_limit)
    return 0;

  return (const void *)((char *)d_buffer + ((d_start + pos) * d_itemsize));
}

long long
time_series::nsamples_available(unsigned long long pos) const
{
  if ((d_start + pos) >= d_limit)
    return 0;

  return d_limit - (d_start + pos);
}
