/*
 * Copyright 2008 Free Software Foundation, Inc.
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

#include <gr_top_block.h>
#include <usrp_source_base.h>
#include <usrp_source_c.h>
#include <usrp_source_s.h>
#include <gr_file_sink.h>

class usrp_rx_cfile;
typedef boost::shared_ptr<usrp_rx_cfile> usrp_rx_cfile_sptr;
usrp_rx_cfile_sptr make_usrp_rx_cfile(int which, usrp_subdev_spec spec, 
				      int decim, double freq, float gain,
				      bool width8, bool nohb,
				      bool output_shorts, int nsamples,
				      const std::string &filename);

class usrp_rx_cfile : public gr_top_block
{
private:
    usrp_rx_cfile(int which, usrp_subdev_spec spec, 
		  int decim, double freq, float gain,
		  bool width8, bool nohb,
		  bool output_shorts, int nsamples,
		  const std::string &filename);
    friend usrp_rx_cfile_sptr make_usrp_rx_cfile(int which, usrp_subdev_spec spec, 
						 int decim, double freq, float gain,
						 bool width8, bool nohb,
						 bool output_shorts, int nsamples,
						 const std::string &filename);

    int d_which;
    usrp_subdev_spec d_spec;
    int d_decim;
    double d_freq;
    float d_gain;
    bool d_width8, d_nohb;
    int d_nsamples;
    std::string d_filename;

 public:
    gr_block_sptr d_head;
    gr_block_sptr d_dst;
};
