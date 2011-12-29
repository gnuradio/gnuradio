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

enum snr_est_type_t {
  SNR_EST_SIMPLE = 0,	// Simple estimator (>= 7 dB)
  SNR_EST_SKEW,	        // Skewness-base est (>= 5 dB)
  SNR_EST_M2M4,         // 2nd & 4th moment est (>= 1 dB)
  SNR_EST_SVR           // SVR-based est (>= 0dB)
};

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
		     gr_vector_const_void_star &input_items);

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
	     gr_vector_const_void_star &input_items);
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
	     gr_vector_const_void_star &input_items);
  double snr();
};


class DIGITAL_API digital_impl_mpsk_snr_est_m2m4 :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2;
  
 public:
  /*! \brief SNR Estimator using 2nd and 4th-order moments.
   *
   *  An SNR estimator for M-PSK signals that uses 2nd (M2) and 4th
   *  (M4) order moments. This estimator uses knowledge of the
   *  kurtosis of the signal (k_a) and noise (k_w) to make its
   *  estimation. We use Beaulieu's approximations here to M-PSK
   *  signals and AWGN channels such that k_a=1 and k_w=2. These
   *  approximations significantly reduce the complexity of the
   *  calculations (and computations) required.
   *
   *  Reference:
   *  D. R. Pauluzzi and N. C. Beaulieu, "A comparison of SNR
   *  estimation techniques for the AWGN channel," IEEE
   *  Trans. Communications, Vol. 48, No. 10, pp. 1681-1691, 2000.
   */
  digital_impl_mpsk_snr_est_m2m4(double alpha);
  ~digital_impl_mpsk_snr_est_m2m4() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items);
  double snr();
};

class DIGITAL_API digital_impl_snr_est_m2m4 :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2;
  double d_ka, d_kw;
  
 public:
  /*! \brief SNR Estimator using 2nd and 4th-order moments.
   *
   *  An SNR estimator for M-PSK signals that uses 2nd (M2) and 4th
   *  (M4) order moments. This estimator uses knowledge of the
   *  kurtosis of the signal (k_a) and noise (k_w) to make its
   *  estimation. In this case, you can set your own estimations for
   *  k_a and k_w, the kurtosis of the signal and noise, to fit this
   *  estimation better to your signal and channel conditions.
   *
   *  A word of warning: this estimator has not been fully tested or
   *  proved with any amount of rigor. The estimation for M4 in
   *  particular might be ignoring effectf of when k_a and k_w are
   *  different. Use this estimator with caution and a copy of the
   *  reference on hand.
   *
   *  The digital_mpsk_snr_est_m2m4 assumes k_a and k_w to simplify
   *  the computations for M-PSK and AWGN channels. Use that estimator
   *  unless you have a way to guess or estimate these values here.
   *
   *  Original paper: 
   *  R. Matzner, "An SNR estimation algorithm for complex baseband
   *  signal using higher order statistics," Facta Universitatis
   *  (Nis), no. 6, pp. 41-52, 1993.
   *
   *  Reference used in derivation:
   *  D. R. Pauluzzi and N. C. Beaulieu, "A comparison of SNR
   *  estimation techniques for the AWGN channel," IEEE
   *  Trans. Communications, Vol. 48, No. 10, pp. 1681-1691, 2000.
   */
  digital_impl_snr_est_m2m4(double alpha, double ka, double kw);
  ~digital_impl_snr_est_m2m4() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items);
  double snr();
};


class DIGITAL_API digital_impl_mpsk_snr_est_svr :
  public digital_impl_mpsk_snr_est
{
 private:
  double d_y1, d_y2;
  
 public:
  /*! \brief Signal-to-Variation Ratio SNR Estimator.
   *
   *  This estimator actually comes from an SNR estimator for M-PSK
   *  signals in fading channels, but this implementation is
   *  specifically for AWGN channels. The math was simplified to
   *  assume a signal and noise kurtosis (k_a and k_w) for M-PSK
   *  signals in AWGN. These approximations significantly reduce the
   *  complexity of the calculations (and computations) required.
   *
   *  Original paper:
   *  A. L. Brandao, L. B. Lopes, and D. C. McLernon, "In-service
   *  monitoring of multipath delay and cochannel interference for
   *  indoor mobile communication systems," Proc. IEEE
   *  Int. Conf. Communications, vol. 3, pp. 1458-1462, May 1994.
   *
   *  Reference:
   *  D. R. Pauluzzi and N. C. Beaulieu, "A comparison of SNR
   *  estimation techniques for the AWGN channel," IEEE
   *  Trans. Communications, Vol. 48, No. 10, pp. 1681-1691, 2000.
   */
  digital_impl_mpsk_snr_est_svr(double alpha);
  ~digital_impl_mpsk_snr_est_svr() {}

  int update(int noutput_items,
	     gr_vector_const_void_star &input_items);
  double snr();
};

#endif /* INCLUDED_DIGITAL_IMPL_MPSK_SNR_EST_H */
