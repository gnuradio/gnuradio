/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef _GR_SINGLE_ZERO_REC_H_
#define _GR_SINGLE_ZERO_REC_H_

#include <stdexcept>

/*!
 * \brief class template for single zero recursive filter
 */
template<class o_type, class i_type, class tap_type> 
class gr_single_zero_rec {
public:
  /*!
   * \brief construct new single zero IIR with given alpha
   *
   * computes y(i) = alpha * x(i) - (1-alpha) * y(i-1)
   */
  gr_single_zero_rec (tap_type alpha = 1.0)
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

  tap_type prev_output () { return d_prev_output; }
    
protected:
  tap_type	d_alpha;
  tap_type	d_one_minus_alpha;
  tap_type	d_prev_output;
};


//
// general case.  We may want to specialize this
//
template<class o_type, class i_type, class tap_type> 
o_type
gr_single_zero_rec<o_type, i_type, tap_type>::filter (const i_type input)
{
  tap_type	output;

  output = d_alpha * input - d_one_minus_alpha * d_prev_output;
  d_prev_output = output;

  return (o_type) output;
}


template<class o_type, class i_type, class tap_type> 
void 
gr_single_zero_rec<o_type, i_type, tap_type>::filterN (o_type output[],
						       const i_type input[],
						       unsigned long n)
{
  for (unsigned i = 0; i < n; i++)
    output[i] = filter (input[i]);
}


#endif /* _GR_SINGLE_ZERO_REC_H_ */
