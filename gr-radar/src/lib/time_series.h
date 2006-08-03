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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef INCLUDED_TIME_SERIES_H
#define INCLUDED_TIME_SERIES_H

#include <string>

/*!
 * \brief Memory mapped input for complex time series data
 *
 * Inspired by "iqts" by John Sahr, Univ of Washington
 */

class time_series {
  size_t		d_itemsize;	    // user specified item size
  std::string		d_filename;
  int			d_fd;		    // file descriptor
  unsigned long long	d_filesize;	    // in bytes
  unsigned long long 	d_start;  	    // in items
  unsigned long long	d_limit;	    // in items
  void	       	       *d_buffer;	    // points to base of file

public:
  /*!
   * \brief Create read-only mapped file accessor.
   * \param item_size	         size of item in bytes
   * \param filename	         name of file to open
   * \param starting_offset      offset in file in item_size units at which to start
   * \param nsamples_to_process  maximum number of samples to map in starting at \p start.  -1 implies no limit.
   *
   * \throws string on error opening file, etc.
   */
  time_series(size_t item_size, const std::string filename,
	      unsigned long long starting_offset=0,
	      long long nsamples_to_process=-1);
  ~time_series();

  /*!
   * \brief Return a pointer to a buffer of data at file offset pos.
   *
   * \param pos		offset from beginning of file in itemsize units.
   * \param blocksize	minimum size of returned buffer in itemsize units.
   *
   * "Seek" to pos in file and return a pointer to the data at that
   * location.  The returned pointer will have at least blocksize valid	
   * elements.  Return 0 if pos is out of bounds, or if there isn't
   * at least blocksize units available in the file.
   */
  const void *seek(unsigned long long pos, unsigned long long blocksize) const;

  long long nsamples_available(unsigned long long pos) const;
};

#endif /* INCLUDED_TIME_SERIES_H */
