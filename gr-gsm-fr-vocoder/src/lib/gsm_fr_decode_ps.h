/* -*- c++ -*- */
/*
 * Copyright 2005 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GSM_FR_DECODE_PS_H
#define INCLUDED_GSM_FR_DECODE_PS_H

#include <gr_sync_interpolator.h>

class gsm_fr_decode_ps;
typedef boost::shared_ptr<gsm_fr_decode_ps> gsm_fr_decode_ps_sptr;

gsm_fr_decode_ps_sptr gsm_fr_make_decode_ps ();

/*!
 * \brief GSM 06.10 Full Rate Vocoder Decoder
 */
class gsm_fr_decode_ps : public gr_sync_interpolator {
  struct gsm_state	*d_gsm;

  friend gsm_fr_decode_ps_sptr gsm_fr_make_decode_ps ();
  gsm_fr_decode_ps ();

public:
  ~gsm_fr_decode_ps ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);
};

#endif /* INCLUDED_GSM_FR_DECODE_PS_H */
