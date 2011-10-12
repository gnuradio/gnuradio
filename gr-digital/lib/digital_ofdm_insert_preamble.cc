/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2011 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <digital_ofdm_insert_preamble.h>
#include <gr_io_signature.h>
#include <stdexcept>
#include <iostream>
#include <string.h>

digital_ofdm_insert_preamble_sptr
digital_make_ofdm_insert_preamble(int fft_length,
				  const std::vector<std::vector<gr_complex> > &preamble)
{
  return gnuradio::get_initial_sptr(new digital_ofdm_insert_preamble(fft_length,
								     preamble));
}

digital_ofdm_insert_preamble::digital_ofdm_insert_preamble
       (int fft_length,
	const std::vector<std::vector<gr_complex> > &preamble)
  : gr_block("ofdm_insert_preamble",
	     gr_make_io_signature2(2, 2,
				   sizeof(gr_complex)*fft_length,
				   sizeof(char)),
	     gr_make_io_signature2(1, 2,
				   sizeof(gr_complex)*fft_length,
				   sizeof(char))),
    d_fft_length(fft_length),
    d_preamble(preamble),
    d_state(ST_IDLE),
    d_nsymbols_output(0),
    d_pending_flag(0)
{
  // sanity check preamble symbols
  for (size_t i = 0; i < d_preamble.size(); i++){
    if (d_preamble[i].size() != (size_t) d_fft_length)
      throw std::invalid_argument("digital_ofdm_insert_preamble: invalid length for preamble symbol");
  }

  enter_idle();
}


digital_ofdm_insert_preamble::~digital_ofdm_insert_preamble()
{
}

int
digital_ofdm_insert_preamble::general_work (int noutput_items,
					    gr_vector_int &ninput_items_v,
					    gr_vector_const_void_star &input_items,
					    gr_vector_void_star &output_items)
{
  int ninput_items = std::min(ninput_items_v[0], ninput_items_v[1]);
  const gr_complex *in_sym = (const gr_complex *) input_items[0];
  const unsigned char *in_flag = (const unsigned char *) input_items[1];

  gr_complex *out_sym = (gr_complex *) output_items[0];
  unsigned char *out_flag = 0;
  if (output_items.size() == 2)
    out_flag = (unsigned char *) output_items[1];


  int no = 0;	// number items output
  int ni = 0;	// number items read from input


#define write_out_flag() 			\
  do { if (out_flag) 				\
          out_flag[no] = d_pending_flag; 	\
       d_pending_flag = 0; 			\
  } while(0)


  while (no < noutput_items && ni < ninput_items){
    switch(d_state){
    case ST_IDLE:
      if (in_flag[ni] & 0x1)	// this is first symbol of new payload
	enter_preamble();
      else
	ni++;			// eat one input symbol
      break;
      
    case ST_PREAMBLE:
      assert(in_flag[ni] & 0x1);
      if (d_nsymbols_output >= (int) d_preamble.size()){
	// we've output all the preamble
	enter_first_payload();
      }
      else {
	memcpy(&out_sym[no * d_fft_length],
	       &d_preamble[d_nsymbols_output][0],
	       d_fft_length*sizeof(gr_complex));

	write_out_flag();
	no++;
	d_nsymbols_output++;
      }
      break;
      
    case ST_FIRST_PAYLOAD:
      // copy first payload symbol from input to output
      memcpy(&out_sym[no * d_fft_length],
	     &in_sym[ni * d_fft_length],
	     d_fft_length * sizeof(gr_complex));

      write_out_flag();
      no++;
      ni++;
      enter_payload();
      break;
      
    case ST_PAYLOAD:
      if (in_flag[ni] & 0x1){	// this is first symbol of a new payload
	enter_preamble();
	break;
      }

      // copy a symbol from input to output
      memcpy(&out_sym[no * d_fft_length],
	     &in_sym[ni * d_fft_length],
	     d_fft_length * sizeof(gr_complex));

      write_out_flag();
      no++;
      ni++;
      break;

    default:
      std::cerr << "digital_ofdm_insert_preamble: (can't happen) invalid state, resetting\n";
      enter_idle();
    }
  }

  consume_each(ni);
  return no;
}

void
digital_ofdm_insert_preamble::enter_idle()
{
  d_state = ST_IDLE;
  d_nsymbols_output = 0;
  d_pending_flag = 0;
}

void
digital_ofdm_insert_preamble::enter_preamble()
{
  d_state = ST_PREAMBLE;
  d_nsymbols_output = 0;
  d_pending_flag = 1;
}

void
digital_ofdm_insert_preamble::enter_first_payload()
{
  d_state = ST_FIRST_PAYLOAD;
}

void
digital_ofdm_insert_preamble::enter_payload()
{
  d_state = ST_PAYLOAD;
}
