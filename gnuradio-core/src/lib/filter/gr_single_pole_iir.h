/* -*- c++ -*- */
/*
 * Copyright 2002,2006 Free Software Foundation, Inc.
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
#ifndef _GR_SINGLE_POLE_IIR_H_
#define _GR_SINGLE_POLE_IIR_H_

#include <gr_core_api.h>
#include <stdexcept>
#include <gr_complex.h>
/*!
 * \brief class template for single pole IIR filter
 */
template<class o_type, class i_type, class tap_type> 
class gr_single_pole_iir {
public:
  /*!
   * \brief construct new single pole IIR with given alpha
   *
   * computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)
   */
  gr_single_pole_iir (tap_type alpha = 1.0)
  {
    d_prev_output = 0;
    set_taps (alpha);
  }

  /*!
   * \brief compute a single output value.
   * \returns the filtered input value.
   */
  o_type filter (const i_type input);

  /*!
   * \brief compute an array of N output values.
   * \p input must have n valid entries.
   */
  void filterN (o_type output[], const i_type input[], unsigned long n);

  /*!
   * \brief install \p alpha as the current taps.
   */
  void set_taps (tap_type alpha)
  { 
    if (alpha < 0 || alpha > 1)
      throw std::out_of_range ("Alpha must be in [0, 1]\n");

    d_alpha = alpha;
    d_one_minus_alpha = 1.0 - alpha;
  }

  //! reset state to zero
  void reset ()
  {
    d_prev_output = 0;
  }

  o_type prev_output () const { return d_prev_output; }
    
protected:
  tap_type	d_alpha;
  tap_type	d_one_minus_alpha;
  o_type	d_prev_output;
};


//
// general case.  We may want to specialize this
//
template<class o_type, class i_type, class tap_type> 
o_type
gr_single_pole_iir<o_type, i_type, tap_type>::filter (const i_type input)
{
  o_type	output;

  output = d_alpha * input + d_one_minus_alpha * d_prev_output;
  d_prev_output = output;

  return (o_type) output;
}


template<class o_type, class i_type, class tap_type> 
void 
gr_single_pole_iir<o_type, i_type, tap_type>::filterN (o_type output[],
						       const i_type input[],
						       unsigned long n)
{
  for (unsigned i = 0; i < n; i++)
    output[i] = filter (input[i]);
}


//
// Specialized case for gr_complex output and double taps
// We need to have a gr_complexd type for the calculations and prev_output variable (in stead of double)

template<class i_type> 
class gr_single_pole_iir<gr_complex, i_type, double>  {
public:
  /*!
   * \brief construct new single pole IIR with given alpha
   *
   * computes y(i) = (1-alpha) * y(i-1) + alpha * x(i)
   */
  gr_single_pole_iir (double alpha = 1.0)
  {
    d_prev_output = 0;
    set_taps (alpha);
  }

  /*!
   * \brief compute a single output value.
   * \returns the filtered input value.
   */
 gr_complex filter (const i_type input);

  /*!
   * \brief compute an array of N output values.
   * \p input must have n valid entries.
   */
  void filterN (gr_complex output[], const i_type input[], unsigned long n);

  /*!
   * \brief install \p alpha as the current taps.
   */
  void set_taps (double alpha)
  { 
    if (alpha < 0 || alpha > 1)
      throw std::out_of_range ("Alpha must be in [0, 1]\n");

    d_alpha = alpha;
    d_one_minus_alpha = 1.0 - alpha;
  }

  //! reset state to zero
  void reset ()
  {
    d_prev_output = 0;
  }

  gr_complexd prev_output () const { return d_prev_output; }
    
protected:
  double	d_alpha;
  double	d_one_minus_alpha;
  gr_complexd	d_prev_output;
};

template< class i_type> 
gr_complex
gr_single_pole_iir<gr_complex, i_type, double>::filter (const i_type input)
{
  gr_complexd	output;

  output = d_alpha * (gr_complexd)input + d_one_minus_alpha * d_prev_output;
  d_prev_output = output;

  return (gr_complex) output;
}

//Do we need to specialize this, although it is the same as the general case?

template<class i_type> 
void 
gr_single_pole_iir<gr_complex, i_type, double>::filterN (gr_complex output[],
						       const i_type input[],
						       unsigned long n)
{
  for (unsigned i = 0; i < n; i++)
    output[i] = filter (input[i]);
}

#endif /* _GR_SINGLE_POLE_IIR_H_ */
