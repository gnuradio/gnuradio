/* -*- c++ -*- */
/*
 * Copyright 2007,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_PN_CORRELATOR_CC_H
#define INCLUDED_GR_PN_CORRELATOR_CC_H

#include <digital_api.h>
#include <gr_sync_decimator.h>
#include <gri_glfsr.h>

class digital_pn_correlator_cc;
typedef boost::shared_ptr<digital_pn_correlator_cc> digital_pn_correlator_cc_sptr;

DIGITAL_API digital_pn_correlator_cc_sptr
digital_make_pn_correlator_cc(int degree, int mask=0, int seed=1);
/*!
 * \brief PN code sequential search correlator
 *
 * \ingroup sync_blk
 *
 * Receives complex baseband signal, outputs complex correlation
 * against reference PN code, one sample per PN code period. The PN
 * sequence is generated using a GLFSR.
 *
 * \param degree Degree of shift register must be in [1, 32]. If mask
 *               is 0, the degree determines a default mask (see
 *               digital_impl_glfsr.cc for the mapping).
 * \param repeat Set to repeat sequence.
 * \param mask   Allows a user-defined bit mask for indexes of the shift
 *               register to feed back.
 * \param seed   Initial setting for values in shift register.
 */
class DIGITAL_API digital_pn_correlator_cc : public gr_sync_decimator
{
  friend DIGITAL_API digital_pn_correlator_cc_sptr
    digital_make_pn_correlator_cc(int degree, int mask, int seed);

  int        d_len;
  float      d_pn;
  gri_glfsr *d_reference;

 protected:
  digital_pn_correlator_cc(int degree, int mask, int seed);

 public:
  virtual int work(int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);

  ~digital_pn_correlator_cc();
};

#endif /* INCLUDED_GR_PN_CORRELATOR_CC_H */
