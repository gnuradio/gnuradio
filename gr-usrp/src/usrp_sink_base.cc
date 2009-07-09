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

#include <usrp_sink_base.h>
#include <gr_io_signature.h>
#include <usrp/usrp_standard.h>
#include <assert.h>
#include <cstdio>

static const int OUTPUT_MULTIPLE_SAMPLES = 128;		// DON'T CHANGE THIS VALUE!

usrp_sink_base::usrp_sink_base (const std::string &name,
				  gr_io_signature_sptr input_signature,
				  int which_board,
				  unsigned int interp_rate,
				  int nchan,
				  int mux,
				  int fusb_block_size,
				  int fusb_nblocks,
				  const std::string fpga_filename,
				  const std::string firmware_filename
				  ) throw (std::runtime_error)
  : usrp_base(name,
	      input_signature,
	      gr_make_io_signature (0, 0, 0)),
    d_nunderruns (0)
{
  d_usrp = usrp_standard_tx::make (which_board,
				   interp_rate,
				   nchan, mux,
				   fusb_block_size,
				   fusb_nblocks,
				   fpga_filename,
				   firmware_filename
				   );
  if (d_usrp == 0)
    throw std::runtime_error ("can't open usrp");

  set_usrp_basic(d_usrp);

  // All calls to d_usrp->write must be multiples of 512 bytes.

  set_output_multiple (OUTPUT_MULTIPLE_SAMPLES);
}

usrp_sink_base::~usrp_sink_base ()
{
}

int
usrp_sink_base::work (int noutput_items,
		       gr_vector_const_void_star &input_items,
		       gr_vector_void_star &output_items)
{
  static const int BUFSIZE = 16 * (1L << 10);	// 16kB
  unsigned char outbuf[BUFSIZE];
  int		obi = 0;
  int		input_index = 0;
  int		input_items_consumed;
  int		bytes_written;
  bool		underrun;
  

  while (input_index < noutput_items){
  
    copy_to_usrp_buffer (input_items,
			 input_index,
			 noutput_items - input_index,	// input_items_available
			 input_items_consumed,		// [out]
			 &outbuf[obi],			// [out] usrp_buffer
			 BUFSIZE - obi,			// usrp_buffer_length
			 bytes_written);		// [out]

    assert (input_index + input_items_consumed <= noutput_items);
    assert (obi + bytes_written <= BUFSIZE);
    
    input_index += input_items_consumed;
    obi += bytes_written;

    if (obi >= BUFSIZE){	// flush
      if (d_usrp->write (outbuf, obi, &underrun) != obi)
	return -1;		// indicate we're done

      if (underrun){
	d_nunderruns++;
	// fprintf (stderr, "usrp_sink: underrun\n");
	fputs ("uU", stderr);
      }
      obi = 0;
    }
  }

  if (obi != 0){
    assert (obi % 512 == 0);
    if (d_usrp->write (outbuf, obi, &underrun) != obi)
      return -1;	// indicate we're done

    if (underrun){
      d_nunderruns++;
      // fprintf (stderr, "usrp_sink: underrun\n");
      fputs ("uU", stderr);
    }
  }

  return noutput_items;
}

bool
usrp_sink_base::set_interp_rate (unsigned int rate)
{
  return d_usrp->set_interp_rate (rate);
}

bool
usrp_sink_base::set_nchannels (int nchan)
{
  return d_usrp->set_nchannels (nchan);
}

bool
usrp_sink_base::set_mux (int mux)
{
  return d_usrp->set_mux (mux);
}

int
usrp_sink_base::determine_tx_mux_value(usrp_subdev_spec ss)
{
  return d_usrp->determine_tx_mux_value(ss);
}

int
usrp_sink_base::determine_tx_mux_value(usrp_subdev_spec ss_a, usrp_subdev_spec ss_b)
{
  return d_usrp->determine_tx_mux_value(ss_a, ss_b);
}

bool
usrp_sink_base::set_tx_freq (int channel, double freq)
{
  return d_usrp->set_tx_freq (channel, freq);
}

unsigned int
usrp_sink_base::interp_rate () const
{
  return d_usrp->interp_rate ();
}

int
usrp_sink_base::nchannels () const
{
  return d_usrp->nchannels ();
}

int
usrp_sink_base::mux () const
{
  return d_usrp->mux ();
}


double
usrp_sink_base::tx_freq (int channel) const
{
  return d_usrp->tx_freq (channel);
}

bool
usrp_sink_base::has_rx_halfband()
{
  return d_usrp->has_rx_halfband();
}
 
bool
usrp_sink_base::has_tx_halfband()
{
  return d_usrp->has_tx_halfband();
}

int
usrp_sink_base::nddcs()
{
  return d_usrp->nddcs();
}

int
usrp_sink_base::nducs()
{
  return d_usrp->nducs();
}

bool
usrp_sink_base::start()
{
  return d_usrp->start();
}

bool
usrp_sink_base::stop()
{
  return d_usrp->stop();
}


bool
usrp_sink_base::tune(int chan, db_base_sptr db, double target_freq, usrp_tune_result *result)
{
  return d_usrp->tune(chan, db, target_freq, result);
}

usrp_subdev_spec
usrp_sink_base::pick_tx_subdevice()
{
  int dbids[] = {
    USRP_DBID_FLEX_400_TX,
    USRP_DBID_FLEX_900_TX,
    USRP_DBID_FLEX_1200_TX,
    USRP_DBID_FLEX_2400_TX,
    USRP_DBID_BASIC_TX
  };

  std::vector<int> candidates(dbids, dbids+(sizeof(dbids)/sizeof(int)));
  return pick_subdev(candidates);
}
