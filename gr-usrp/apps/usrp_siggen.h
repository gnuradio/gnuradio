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
#include <usrp_sink_c.h>
#include <gr_sig_source_c.h>
#include <gr_noise_source_c.h>

usrp_subdev_spec str_to_subdev(std::string spec_str);

class usrp_siggen;
typedef boost::shared_ptr<usrp_siggen> usrp_siggen_sptr;
usrp_siggen_sptr make_usrp_siggen(int which, usrp_subdev_spec spec, 
				  double rf_freq, int interp, double wfreq,
				  int waveform, float amp, float gain, 
				  float offset, long long nsamples);

class usrp_siggen : public gr_top_block
{
private:
    usrp_siggen(int which, usrp_subdev_spec spec, 
		double rf_freq, int interp, double wfreq,
		int waveform, float amp, float gain, 
		float offset, long long nsamples);
    friend usrp_siggen_sptr make_usrp_siggen(int which, usrp_subdev_spec spec, 
					     double rf_freq, int interp, double wfreq,
					     int waveform, float amp, float gain, 
					     float offset,
					     long long nsamples);
    
 public:
    gr_block_sptr source;
    gr_sig_source_c_sptr siggen;
    gr_noise_source_c_sptr noisegen;
};
