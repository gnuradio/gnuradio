/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
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
#ifndef INCLUDED_ATSC_FS_CHECKER_H
#define INCLUDED_ATSC_FS_CHECKER_H

#include <atsc_api.h>
#include <atsci_fs_checker.h>
#include <gr_sync_block.h>

class atsc_fs_checker;
typedef boost::shared_ptr<atsc_fs_checker> atsc_fs_checker_sptr;

ATSC_API atsc_fs_checker_sptr atsc_make_fs_checker();

/*!
 * \brief ATSC field sync checker (float,syminfo --> float,syminfo)
 * \ingroup atsc
 *
 * first output is delayed version of input.
 * second output is set of tags, one-for-one with first output.
 */

class ATSC_API atsc_fs_checker : public gr_sync_block
{
  friend ATSC_API atsc_fs_checker_sptr atsc_make_fs_checker();

  atsc_fs_checker();

public:
  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void reset() { /* nop */ }

  ~atsc_fs_checker ();

protected:
  atsci_fs_checker	*d_fsc;

};


#endif /* INCLUDED_ATSC_FS_CHECKER_H */
