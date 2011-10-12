/* -*- c++ -*- */
/*
 * Copyright 2007,2011 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOCODER_CVSD_DECODE_BS_H
#define INCLUDED_VOCODER_CVSD_DECODE_BS_H

#include <vocoder_api.h>
#include <gr_sync_interpolator.h>

class vocoder_cvsd_decode_bs;

typedef boost::shared_ptr<vocoder_cvsd_decode_bs> vocoder_cvsd_decode_bs_sptr;

 /*!
  * \brief Constructor parameters to initialize the CVSD decoder.  The default
  * values are modeled after the Bluetooth standard and should not be changed,
  * except by an advanced user
  *
  * \ingroup vocoder_blk
  *
  * \param min_step       Minimum step size used to update the internal reference.  Default: "10"
  * \param max_step       Maximum step size used to update the internal reference.  Default: "1280"
  * \param step_decay     Decay factor applied to step size when there is not a run of J output 1s or 0s.  Default: "0.9990234375"  (i.e. 1-1/1024)
  * \param accum_decay    Decay factor applied to the internal reference during every interation of the codec.  Default: "0.96875"  (i.e. 1-1/32)
  * \param K;             Size of shift register; the number of output bits remembered by codec (must be less or equal to 32).  Default: "32"
  * \param J;             Number of bits in the shift register that are equal; i.e. the size of a run of 1s, 0s. Default: "4"
  * \param pos_accum_max  Maximum integer value allowed for the internal reference.  Default: "32767" (2^15 - 1 or MAXSHORT)
  * \param neg_accum_max  Minimum integer value allowed for the internal reference.  Default: "-32767" (-2^15 + 1 or MINSHORT+1)
  *
  */
VOCODER_API vocoder_cvsd_decode_bs_sptr vocoder_make_cvsd_decode_bs (short min_step=10,
							 short max_step=1280,
							 double step_decay=0.9990234375,
							 double accum_decay= 0.96875,
							 int K=32,
							 int J=4,
							 short pos_accum_max=32767,
							 short neg_accum_max=-32767);

/*!
 * \brief This block performs CVSD audio decoding.  Its design and implementation
 * is modeled after the CVSD encoder/decoder specifications defined in the
 * Bluetooth standard.
 *
 * \ingroup vocoder_blk
 *
 * CVSD is a method for encoding speech that seeks to reduce the
 * bandwidth required for digital voice transmission.  CVSD takes
 * advantage of strong correlation between samples, quantizing the
 * difference in amplitude between two consecutive samples.  This
 * difference requires fewer quantization levels as compared to other
 * methods that quantize the actual amplitude level, reducing the
 * bandwidth.  CVSD employs a two level quantizer (one bit) and an
 * adaptive algorithm that allows for continuous step size adjustment.
 *
 * The coder can represent low amplitude signals with accuracy without
 * sacrificing performance on large amplitude signals, a trade off that
 * occurs in some non-adaptive modulations.
 *
 * The CVSD decoder effectively provides 1-to-8 decompression.  More
 * specifically, for each incoming input bit, the decoder outputs one
 * audio sample.  If the input is a "1" bit, the internal reference is
 * increased appropriately and then outputted as the next estimated audio
 * sample.  If the input is a "0" bit, the internal reference is
 * decreased appropriately and then likewise outputted as the next estimated
 * audio sample.  Grouping 8 input bits together, the encoder essentially
 * produces 8 output audio samples for everyone one input byte.
 *
 * This decoder requires that output audio samples are 2-byte short signed
 * integers.  The result bandwidth conversion, therefore, is 1 byte of
 * encoded audio data to 16 output bytes of raw audio data.
 *
 * The CVSD decoder module must be post-fixed by a down-converter to
 * under-sample the audio data after decoding.  The Bluetooth standard
 * specifically calls for a 8-to-1 decimating down-converter.  This is
 * required so that so that output sampling rate equals the original input
 * sampling rate present before the encoder.  In all cases, the output
 * down-converter rate must be the inverse of the input up-converter rate
 * before the CVSD encoder.
 *
 * References:
 * 1.  Continuously Variable Slope Delta Modulation (CVSD) A Tutorial,
 *         Available: http://www.eetkorea.com/ARTICLES/2003AUG/A/2003AUG29_NTEK_RFD_AN02.PDF.
 * 2.  Specification of The Bluetooth System
 *         Available: http://grouper.ieee.org/groups/802/15/Bluetooth/core_10_b.pdf.
 * 3.  McGarrity, S., Bluetooth Full Duplex Voice and Data Transmission. 2002.
 *         Bluetooth Voice Simulink® Model, Available:
 *         http://www.mathworks.com/company/newsletters/digest/nov01/bluetooth.html
 *
 */

class VOCODER_API vocoder_cvsd_decode_bs : public gr_sync_interpolator
{
private:
  friend VOCODER_API vocoder_cvsd_decode_bs_sptr vocoder_make_cvsd_decode_bs (short min_step,
								  short max_step,
								  double step_decay,
								  double accum_decay,
								  int K,
								  int J,
								  short pos_accum_max,
								  short neg_accum_max);

  vocoder_cvsd_decode_bs (short min_step, short max_step, double step_decay,
			  double accum_decay, int K, int J,
			  short pos_accum_max, short neg_accum_max);

  //! Member functions required by the encoder/decoder
  //! \brief Rounding function specific to CVSD
  //! \return the input value rounded to the nearest integer
  int cvsd_round(double input);
  
  //! \brief A power function specific to CVSD data formats
  //! \return (radix)^power, where radix and power are short integers
  unsigned int cvsd_pow (short radix, short power);
 
  //! \brief Sums number of 1's in the input
  //! \return the number of 1s in the four bytes of an input unsigned integer
  unsigned char cvsd_bitwise_sum (unsigned int input);

  short d_min_step;
  short d_max_step;
  double d_step_decay;
  double d_accum_decay;

  int d_K;  //!< \brief Size of shift register; the number of output bits remembered in shift register
  int d_J;  //!< \brief Number of bits in the shift register that are equal; size of run of 1s, 0s

  short d_pos_accum_max;
  short d_neg_accum_max;

  int d_accum;                //!< \brief Current value of internal reference
  int d_loop_counter;         //!< \brief Current value of the loop counter
  unsigned int d_runner;      //!< \brief Current value of the shift register
  unsigned int d_runner_mask; //!< \brief Value of the mask to access the last J bits of the shift register
  short d_stepsize;           //!< \brief Current value of the step sizer

 public:
  ~vocoder_cvsd_decode_bs ();	// public destructor

  short min_step() { return d_min_step; }
  short max_step() { return d_max_step; }
  double step_decay() { return d_step_decay; }
  double accum_decay() { return d_accum_decay; }
  int K() { return d_K; }
  int J() { return d_J; }
  short pos_accum_max() { return d_pos_accum_max; }
  short neg_accum_max() { return d_neg_accum_max; }

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_VOCODER_CVSD_DECODE_BS_H */
