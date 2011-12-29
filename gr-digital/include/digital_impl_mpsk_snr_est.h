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
#ifndef INCLUDED_DIGITAL_IMPL_MPSK_SNR_EST_H
#define INCLUDED_DIGITAL_IMPL_MPSK_SNR_EST_H

#include <digital_api.h>
#include <gr_sync_block.h>

/*!
 * Parent class for SNR Estimators
 */
class DIGITAL_API digital_impl_mpsk_snr_est
{
 protected:
  double d_alpha, d_beta;

 public:
  digital_impl_mpsk_snr_est(double alpha);
  virtual ~digital_impl_mpsk_snr_est();

  //! Get the running-average coefficient
  double alpha() const;

  //! Set the running-average coefficient
  void set_alpha(double alpha);

  //! Update the current registers
  virtual int update(int noutput_items,
		     gr_vector_const_void_star &input_items,
		     gr_vector_void_star &output_items);

  //! Use the register values to compute a new estimate
  virtual double snr();
};

  
class DIGITAL_API digital_impl_mpsk_snr_est_simple :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2;
  
 public:
  digital_impl_mpsk_snr_est_simple(double alpha);
  ~digital_impl_mpsk_snr_est_simple() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items,
	     gr_vector_void_star &output_items);
  double snr();
};


class DIGITAL_API digital_impl_mpsk_snr_est_skew :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2, d_y3;
  
 public:
  digital_impl_mpsk_snr_est_skew(double alpha);
  ~digital_impl_mpsk_snr_est_skew() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items,
	     gr_vector_void_star &output_items);
  double snr();
};


class DIGITAL_API digital_impl_mpsk_snr_est_m2m4 :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2;
  
 public:
  digital_impl_mpsk_snr_est_m2m4(double alpha);
  ~digital_impl_mpsk_snr_est_m2m4() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items,
	     gr_vector_void_star &output_items);
  double snr();
};


class DIGITAL_API digital_impl_mpsk_snr_est_svn :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2;
  
 public:
  digital_impl_mpsk_snr_est_svn(double alpha);
  ~digital_impl_mpsk_snr_est_svn() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items,
	     gr_vector_void_star &output_items);
  double snr();
};

#endif /* INCLUDED_DIGITAL_IMPL_MPSK_SNR_EST_H */
