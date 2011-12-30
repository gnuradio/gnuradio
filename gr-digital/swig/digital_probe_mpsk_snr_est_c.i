/* -*- c++ -*- */
/*
 * Copyright 2011 Free Software Foundation, Inc.
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

GR_SWIG_BLOCK_MAGIC(digital,probe_mpsk_snr_est_c);

digital_probe_mpsk_snr_est_c_sptr
digital_make_probe_mpsk_snr_est_c(snr_est_type_t type,
				  int msg_nsamples=10000,
				  double alpha=0.001);

class digital_probe_mpsk_snr_est_c : public gr_sync_block
{
private:
  void digital_probe_mpsk_snr_est_c(snr_est_type_t type,
				    int msg_nsamples,
				    double alpha);

public:
  double snr();
  snr_est_type_t type() const;
  int msg_nsample() const;
  double alpha() const;
  void set_type(snr_est_type_t t);
  void set_msg_nsample(int n);
  void set_alpha(double alpha);
};
