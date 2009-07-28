/* -*- c++ -*- */
/*
 * Copyright 2004,2008,2009 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp_source_base.h>
#include <gr_io_signature.h>
#include <usrp/usrp_standard.h>
#include <assert.h>
#include <cstdio>

static const int OUTPUT_MULTIPLE_BYTES = 4 * 1024;

usrp_source_base::usrp_source_base (const std::string &name,
				      gr_io_signature_sptr output_signature,
				      int which_board,
				      unsigned int decim_rate,
				      int nchan,
				      int mux,
				      int mode,
				      int fusb_block_size,
				      int fusb_nblocks,
				      const std::string fpga_filename,
				      const std::string firmware_filename
				      ) throw (std::runtime_error)
  : usrp_base(name,
	      gr_make_io_signature (0, 0, 0),
	      output_signature),
    d_noverruns (0)
{
  d_usrp = usrp_standard_rx::make (which_board, decim_rate,
				   nchan, mux, mode,
				   fusb_block_size,
				   fusb_nblocks,
				   fpga_filename,
				   firmware_filename);
  if (d_usrp == 0)
    throw std::runtime_error ("can't open usrp");

  set_usrp_basic(d_usrp);

  // All calls to d_usrp->read must be multiples of 512 bytes.
  // We jack this up to 4k to reduce overhead.

  set_output_multiple (OUTPUT_MULTIPLE_BYTES / output_signature->sizeof_stream_item (0));
}

usrp_source_base::~usrp_source_base ()
{
}

unsigned int
usrp_source_base::sizeof_basic_sample() const
{
  return usrp_standard_rx::format_width(d_usrp->format()) / 8;
}

int
usrp_source_base::work (int noutput_items,
			 gr_vector_const_void_star &input_items,
			 gr_vector_void_star &output_items)
{
  static const int BUFSIZE = 4 * OUTPUT_MULTIPLE_BYTES;
  unsigned char buf[BUFSIZE];
  int output_index = 0;
  int output_items_produced;
  int bytes_read;
  bool overrun;

  while (output_index < noutput_items){
    int nbytes = ninput_bytes_reqd_for_noutput_items (noutput_items - output_index);
    nbytes = std::min (nbytes, BUFSIZE);

    int result_nbytes = d_usrp->read (buf, nbytes, &overrun);
    if (overrun){
      // fprintf (stderr, "usrp_source: overrun\n");
      fputs ("uO", stderr);
      d_noverruns++;
    }

    if (result_nbytes < 0)	// We've got a problem.  Usually board unplugged or powered down.
      return -1;		// Indicate we're done.

    if (result_nbytes != nbytes){	// not really an error, but unexpected
      fprintf (stderr, "usrp_source: short read.  Expected %d, got %d\n",
	       nbytes, result_nbytes);
    }

    copy_from_usrp_buffer (output_items,
			   output_index,
			   noutput_items - output_index,   // output_items_available
			   output_items_produced,	   // [out]
			   buf,				   // usrp_buffer
			   result_nbytes,		   // usrp_buffer_length
			   bytes_read);			   // [out]

    assert (output_index + output_items_produced <= noutput_items);
    assert (bytes_read == result_nbytes);

    output_index += output_items_produced;
  }

  return noutput_items;
}


bool
usrp_source_base::set_decim_rate (unsigned int rate)
{
  return d_usrp->set_decim_rate (rate);
}

bool
usrp_source_base::set_nchannels (int nchan)
{
  return d_usrp->set_nchannels (nchan);
}

bool
usrp_source_base::set_mux (int mux)
{
  return d_usrp->set_mux (mux);
}

int
usrp_source_base::determine_rx_mux_value(usrp_subdev_spec ss)
{
  return d_usrp->determine_rx_mux_value(ss);
}

int
usrp_source_base::determine_rx_mux_value(usrp_subdev_spec ss_a, usrp_subdev_spec ss_b)
{
  return d_usrp->determine_rx_mux_value(ss_a, ss_b);
}

bool
usrp_source_base::set_rx_freq (int channel, double freq)
{
  return d_usrp->set_rx_freq (channel, freq);
}

unsigned int
usrp_source_base::decim_rate () const
{
  return d_usrp->decim_rate ();
}

int
usrp_source_base::nchannels () const
{
  return d_usrp->nchannels ();
}

int
usrp_source_base::mux () const
{
  return d_usrp->mux ();
}

double
usrp_source_base::rx_freq (int channel) const
{
  return d_usrp->rx_freq (channel);
}

bool
usrp_source_base::set_fpga_mode (int mode)
{
  return d_usrp->set_fpga_mode (mode);
}

bool
usrp_source_base::set_ddc_phase (int channel, int phase)
{
  return d_usrp->set_ddc_phase(channel, phase);
}


bool
usrp_source_base::set_format(unsigned int format)
{
  return d_usrp->set_format(format);
}

unsigned int
usrp_source_base::format() const
{
  return d_usrp->format();
}

unsigned int
usrp_source_base::make_format(int width, int shift, bool want_q, bool bypass_halfband)
{
  return usrp_standard_rx::make_format(width, shift, want_q, bypass_halfband);
}

int
usrp_source_base::format_width(unsigned int format)
{
  return usrp_standard_rx::format_width(format);
}

int
usrp_source_base::format_shift(unsigned int format)
{
  return usrp_standard_rx::format_shift(format);
}

bool
usrp_source_base::format_want_q(unsigned int format)
{
  return usrp_standard_rx::format_want_q(format);
}

bool
usrp_source_base::format_bypass_halfband(unsigned int format)
{
  return usrp_standard_rx::format_bypass_halfband(format);
}

bool
usrp_source_base::has_rx_halfband()
{
  return d_usrp->has_rx_halfband();
}
 
bool
usrp_source_base::has_tx_halfband()
{
  return d_usrp->has_tx_halfband();
}

int
usrp_source_base::nddcs()
{
  return d_usrp->nddcs();
}

int
usrp_source_base::nducs()
{
  return d_usrp->nducs();
}

bool
usrp_source_base::start()
{
  return d_usrp->start();
}

bool
usrp_source_base::stop()
{
  return d_usrp->stop();
}

bool
usrp_source_base::tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result)
{
  return d_usrp->tune(chan, db, target_freq, result);
}

usrp_subdev_spec
usrp_source_base::pick_rx_subdevice()
{
  int dbids[] = {
    USRP_DBID_FLEX_400_RX,
    USRP_DBID_FLEX_900_RX,
    USRP_DBID_FLEX_1200_RX,
    USRP_DBID_FLEX_2400_RX,
    USRP_DBID_TV_RX,
    USRP_DBID_TV_RX_REV_2,
    USRP_DBID_TV_RX_REV_3,
    USRP_DBID_TV_RX_MIMO,
    USRP_DBID_TV_RX_REV_2_MIMO,
    USRP_DBID_TV_RX_REV_3_MIMO,
    USRP_DBID_DBS_RX,
    USRP_DBID_BASIC_RX
  };

  std::vector<int> candidates(dbids, dbids+(sizeof(dbids)/sizeof(int)));
  return pick_subdev(candidates);
}
