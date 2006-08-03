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

#ifndef INCLUDED_GRI_IIR_H
#define INCLUDED_GRI_IIR_H

#include <vector>
#include <stdexcept>

/*!
 * \brief base class template for Infinite Impulse Response filter (IIR)
 */
template<class i_type, class o_type, class tap_type> 
class gri_iir {
public:
  /*!
   * \brief Construct an IIR with the given taps.
   *
   * This filter uses the Direct Form I implementation, where
   * \p fftaps contains the feed-forward taps, and \p fbtaps the feedback ones.
   *
   * \p fftaps and \p fbtaps must have equal numbers of taps
   *
   * The input and output satisfy a difference equation of the form

   \f[
   y[n] - \sum_{k=1}^{N} a_k y[n-k] = \sum_{k=0}^{M} b_k x[n-k]
   \f]

   * with the corresponding rational system function

   \f[
   H(z) = \frac{\sum_{k=0}^{M} b_k z^{-k}}{1 - \sum_{k=1}^{N} a_k z^{-k}}
   \f]

   * Note that some texts define the system function with a + in the denominator.
   * If you're using that convention, you'll need to negate the feedback taps.
   */
  gri_iir (const std::vector<tap_type>& fftaps,
	  const std::vector<tap_type>& fbtaps) throw (std::invalid_argument)
  {
    set_taps (fftaps, fbtaps);
  }

  gri_iir () : d_latest(0) { }

  ~gri_iir () {}

  /*!
   * \brief compute a single output value.
   * \returns the filtered input value.
   */
  o_type filter (const i_type input);

  /*!
   * \brief compute an array of N output values.
   * \p input must have N valid entries.
   */
  void filter_n (o_type output[], const i_type input[], long n);

  /*!
   * \return number of taps in filter.
   */
  unsigned ntaps () const { return d_fftaps.size (); }

  /*!
   * \brief install new taps.
   */
  void set_taps (const std::vector<tap_type> &fftaps, 
		 const std::vector<tap_type> &fbtaps) throw (std::invalid_argument)
  { 
    if (fftaps.size () != fbtaps.size ())
      throw std::invalid_argument ("gri_iir::set_taps");

    d_latest = 0;
    d_fftaps = fftaps; 
    d_fbtaps = fbtaps; 

    int n = fftaps.size ();
    d_prev_input.resize (2 * n);
    d_prev_output.resize (2 * n);

    for (int i = 0; i < 2 * n; i++){
      d_prev_input[i] = 0;
      d_prev_output[i] = 0;
    }
  }

protected:
  std::vector<tap_type>	d_fftaps;
  std::vector<tap_type>	d_fbtaps;
  int 			d_latest;
  std::vector<tap_type>	d_prev_output;
  std::vector<i_type>	d_prev_input;
};


//
// general case.  We may want to specialize this
//
template<class i_type, class o_type, class tap_type> 
o_type
gri_iir<i_type, o_type, tap_type>::filter (const i_type input)
{
  tap_type	acc;
  unsigned	i = 0;
  unsigned	n = ntaps ();

  if (n == 0)
    return (o_type) 0;

  int latest = d_latest;
  
  acc = d_fftaps[0] * input;
  for (i = 1; i < n; i ++)
    acc += (d_fftaps[i] * d_prev_input[latest + i]
	    + d_fbtaps[i] * d_prev_output[latest + i]);

  // store the values twice to avoid having to handle wrap-around in the loop
  d_prev_output[latest] = acc;
  d_prev_output[latest+n] = acc;
  d_prev_input[latest] = input;
  d_prev_input[latest+n] = input;

  latest--;
  if (latest < 0)
    latest += n;

  d_latest = latest;
  return (o_type) acc;
}


template<class i_type, class o_type, class tap_type> 
void 
gri_iir<i_type, o_type, tap_type>::filter_n (o_type output[],
					     const i_type input[],
					     long n)
{
  for (int i = 0; i < n; i++)
    output[i] = filter (input[i]);
}

#endif /* INCLUDED_GRI_IIR_H */

