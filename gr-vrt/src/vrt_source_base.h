/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VRT_SOURCE_BASE_H
#define INCLUDED_VRT_SOURCE_BASE_H

#include <gr_sync_block.h>
#include <vrt/rx.h>
#include <gr_io_signature.h>

/*!
 * Base class for all VRT source blocks
 */
class vrt_source_base : public gr_sync_block
{
protected:
  vrt_source_base(const char *name,
		  gr_io_signature_sptr output_signature);

public:
  ~vrt_source_base();

  virtual vrt::rx::sptr vrt_rx() const = 0;
};

#endif /* INCLUDED_VRT_SOURCE_BASE_H */
