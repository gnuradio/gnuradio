/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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
#include "config.h"
#endif

#include <usrp1_sink_base.h>
#include <gr_io_signature.h>
#include <usrp_standard.h>
#include <assert.h>

static const int OUTPUT_MULTIPLE_SAMPLES = 128;		// DON'T CHANGE THIS VALUE!

usrp1_sink_base::usrp1_sink_base (const std::string &name,
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
  : gr_sync_block (name,
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
    throw std::runtime_error ("can't open usrp1");

  // All calls to d_usrp->write must be multiples of 512 bytes.

  set_output_multiple (OUTPUT_MULTIPLE_SAMPLES);
}

usrp1_sink_base::~usrp1_sink_base ()
{
  delete d_usrp;
}

bool 
usrp1_sink_base::start()
{
  return d_usrp->start();
}

bool 
usrp1_sink_base::stop()
{
  return d_usrp->stop();
}

int
usrp1_sink_base::work (int noutput_items,
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
	// fprintf (stderr, "usrp1_sink: underrun\n");
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
      // fprintf (stderr, "usrp1_sink: underrun\n");
      fputs ("uU", stderr);
    }
  }

  return noutput_items;
}

bool
usrp1_sink_base::set_interp_rate (unsigned int rate)
{
  return d_usrp->set_interp_rate (rate);
}

bool
usrp1_sink_base::set_nchannels (int nchan)
{
  return d_usrp->set_nchannels (nchan);
}

bool
usrp1_sink_base::set_mux (int mux)
{
  return d_usrp->set_mux (mux);
}

bool
usrp1_sink_base::set_tx_freq (int channel, double freq)
{
  return d_usrp->set_tx_freq (channel, freq);
}

long
usrp1_sink_base::fpga_master_clock_freq() const
{
  return d_usrp->fpga_master_clock_freq();
}

long
usrp1_sink_base::converter_rate () const
{
  return d_usrp->converter_rate ();
}

unsigned int
usrp1_sink_base::interp_rate () const
{
  return d_usrp->interp_rate ();
}

int
usrp1_sink_base::nchannels () const
{
  return d_usrp->nchannels ();
}

int
usrp1_sink_base::mux () const
{
  return d_usrp->mux ();
}


double
usrp1_sink_base::tx_freq (int channel) const
{
  return d_usrp->tx_freq (channel);
}

void
usrp1_sink_base::set_verbose (bool verbose)
{  
  d_usrp->set_verbose (verbose);
}

bool
usrp1_sink_base::write_aux_dac (int which_dboard, int which_dac, int value)
{
  return d_usrp->write_aux_dac (which_dboard, which_dac, value);
}

int
usrp1_sink_base::read_aux_adc (int which_dboard, int which_adc)
{
  return d_usrp->read_aux_adc (which_dboard, which_adc);
}

bool
usrp1_sink_base::write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf)
{
  return d_usrp->write_eeprom (i2c_addr, eeprom_offset, buf);
}

std::string
usrp1_sink_base::read_eeprom (int i2c_addr, int eeprom_offset, int len)
{
  return d_usrp->read_eeprom (i2c_addr, eeprom_offset, len);
}

bool
usrp1_sink_base::write_i2c (int i2c_addr, const std::string buf)
{
  return d_usrp->write_i2c (i2c_addr, buf);
}

std::string
usrp1_sink_base::read_i2c (int i2c_addr, int len)
{
  return d_usrp->read_i2c (i2c_addr, len);
}

bool
usrp1_sink_base::set_pga (int which, double gain)
{
  return d_usrp->set_pga (which, gain);
}

double
usrp1_sink_base::pga (int which) const
{
  return d_usrp->pga (which);
}

double
usrp1_sink_base::pga_min () const
{
  return d_usrp->pga_min ();
}

double
usrp1_sink_base::pga_max () const
{
  return d_usrp->pga_max ();
}

double
usrp1_sink_base::pga_db_per_step () const
{
  return d_usrp->pga_db_per_step ();
}

int
usrp1_sink_base::daughterboard_id (int which) const
{
  return d_usrp->daughterboard_id (which);
}

bool
usrp1_sink_base::set_adc_offset (int which, int offset)
{
  return d_usrp->set_adc_offset (which, offset);
}

bool
usrp1_sink_base::set_dac_offset (int which, int offset, int offset_pin)
{
  return d_usrp->set_dac_offset (which, offset, offset_pin);
}

bool
usrp1_sink_base::set_adc_buffer_bypass (int which, bool bypass)
{
  return d_usrp->set_adc_buffer_bypass (which, bypass);
}

std::string
usrp1_sink_base::serial_number()
{
  return d_usrp->serial_number();
}

bool
usrp1_sink_base::_write_oe (int which_dboard, int value, int mask)
{
  return d_usrp->_write_oe (which_dboard, value, mask);
}

bool
usrp1_sink_base::write_io (int which_dboard, int value, int mask)
{
  return d_usrp->write_io (which_dboard, value, mask);
}

int
usrp1_sink_base::read_io (int which_dboard)
{
  return d_usrp->read_io (which_dboard);
}

// internal routines...

bool
usrp1_sink_base::_write_fpga_reg (int regno, int value)
{
  return d_usrp->_write_fpga_reg (regno, value);
}

int
usrp1_sink_base::_read_fpga_reg (int regno)
{
  return d_usrp->_read_fpga_reg (regno);
}

bool
usrp1_sink_base::_write_9862 (int which_codec, int regno, unsigned char value)
{
  return d_usrp->_write_9862 (which_codec, regno, value);
}

int
usrp1_sink_base::_read_9862 (int which_codec, int regno) const
{
  return d_usrp->_read_9862 (which_codec, regno);
}

bool
usrp1_sink_base::_write_spi (int optional_header, int enables,
			     int format, std::string buf)
{
  return d_usrp->_write_spi (optional_header, enables, format, buf);
}

std::string
usrp1_sink_base::_read_spi (int optional_header, int enables, int format, int len)
{
  return d_usrp->_read_spi (optional_header, enables, format, len);
}
