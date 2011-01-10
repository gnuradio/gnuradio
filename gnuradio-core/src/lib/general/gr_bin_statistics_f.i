/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// Directors are only supported in Python, Java and C#.  gr_feval_dd uses directors
#ifdef SWIGPYTHON

GR_SWIG_BLOCK_MAGIC(gr,bin_statistics_f);

gr_bin_statistics_f_sptr
gr_make_bin_statistics_f(unsigned int vlen,	// vector length
			 gr_msg_queue_sptr msgq,
			 gr_feval_dd *tune,	// callback
			 size_t tune_delay,	// samples
			 size_t dwell_delay);	// samples


class gr_bin_statistics_f : public gr_sync_block
{
private:
  gr_bin_statistics_f(unsigned int vlen,
		      gr_msg_queue_sptr msgq,
		      gr_feval_dd *tune,
		      size_t tune_delay,
		      size_t dwell_delay);
public:
  ~gr_bin_statistics_f();
};

#endif
