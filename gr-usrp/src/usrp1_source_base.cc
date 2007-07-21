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

#include <usrp1_source_base.h>
#include <gr_io_signature.h>
#include <usrp_standard.h>
#include <assert.h>

static const int OUTPUT_MULTIPLE_BYTES = 4 * 1024;

usrp1_source_base::usrp1_source_base (const std::string &name,
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
  : gr_sync_block (name,
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
    throw std::runtime_error ("can't open usrp1");

  // All calls to d_usrp->read must be multiples of 512 bytes.
  // We jack this up to 4k to reduce overhead.

  set_output_multiple (OUTPUT_MULTIPLE_BYTES / output_signature->sizeof_stream_item (0));
}

usrp1_source_base::~usrp1_source_base ()
{
  delete d_usrp;
}

unsigned int
usrp1_source_base::sizeof_basic_sample() const
{
  return usrp_standard_rx::format_width(d_usrp->format()) / 8;
}

bool
usrp1_source_base::start()
{
  return d_usrp->start();
}

bool
usrp1_source_base::stop()
{
  return d_usrp->stop();
}

int
usrp1_source_base::work (int noutput_items,
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
      // fprintf (stderr, "usrp1_source: overrun\n");
      fputs ("uO", stderr);
      d_noverruns++;
    }

    if (result_nbytes < 0)	// We've got a problem.  Usually board unplugged or powered down.
      return -1;		// Indicate we're done.

    if (result_nbytes != nbytes){	// not really an error, but unexpected
      fprintf (stderr, "usrp1_source: short read.  Expected %d, got %d\n",
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
usrp1_source_base::set_decim_rate (unsigned int rate)
{
  return d_usrp->set_decim_rate (rate);
}

bool
usrp1_source_base::set_nchannels (int nchan)
{
  return d_usrp->set_nchannels (nchan);
}

bool
usrp1_source_base::set_mux (int mux)
{
  return d_usrp->set_mux (mux);
}

bool
usrp1_source_base::set_rx_freq (int channel, double freq)
{
  return d_usrp->set_rx_freq (channel, freq);
}

long
usrp1_source_base::fpga_master_clock_freq() const
{
  return d_usrp->fpga_master_clock_freq();
}

long
usrp1_source_base::converter_rate() const
{
  return d_usrp->converter_rate();
}

unsigned int
usrp1_source_base::decim_rate () const
{
  return d_usrp->decim_rate ();
}

int
usrp1_source_base::nchannels () const
{
  return d_usrp->nchannels ();
}

int
usrp1_source_base::mux () const
{
  return d_usrp->mux ();
}

double
usrp1_source_base::rx_freq (int channel) const
{
  return d_usrp->rx_freq (channel);
}

bool
usrp1_source_base::set_fpga_mode (int mode)
{
  return d_usrp->set_fpga_mode (mode);
}

bool
usrp1_source_base::set_ddc_phase (int channel, int phase)
{
  return d_usrp->set_ddc_phase(channel, phase);
}

bool
usrp1_source_base::set_dc_offset_cl_enable(int bits, int mask)
{
  return d_usrp->set_dc_offset_cl_enable(bits, mask);
}

void
usrp1_source_base::set_verbose (bool verbose)
{  
  d_usrp->set_verbose (verbose);
}

bool
usrp1_source_base::write_aux_dac (int which_dboard, int which_dac, int value)
{
  return d_usrp->write_aux_dac (which_dboard, which_dac, value);
}

int
usrp1_source_base::read_aux_adc (int which_dboard, int which_adc)
{
  return d_usrp->read_aux_adc (which_dboard, which_adc);
}

bool
usrp1_source_base::write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf)
{
  return d_usrp->write_eeprom (i2c_addr, eeprom_offset, buf);
}

std::string
usrp1_source_base::read_eeprom (int i2c_addr, int eeprom_offset, int len)
{
  return d_usrp->read_eeprom (i2c_addr, eeprom_offset, len);
}

bool
usrp1_source_base::write_i2c (int i2c_addr, const std::string buf)
{
  return d_usrp->write_i2c (i2c_addr, buf);
}

std::string
usrp1_source_base::read_i2c (int i2c_addr, int len)
{
  return d_usrp->read_i2c (i2c_addr, len);
}

bool
usrp1_source_base::set_pga (int which, double gain)
{
  return d_usrp->set_pga (which, gain);
}

double
usrp1_source_base::pga (int which) const
{
  return d_usrp->pga (which);
}

double
usrp1_source_base::pga_min () const
{
  return d_usrp->pga_min ();
}

double
usrp1_source_base::pga_max () const
{
  return d_usrp->pga_max ();
}

double
usrp1_source_base::pga_db_per_step () const
{
  return d_usrp->pga_db_per_step ();
}

int
usrp1_source_base::daughterboard_id (int which) const
{
  return d_usrp->daughterboard_id (which);
}


bool
usrp1_source_base::set_adc_offset (int which, int offset)
{
  return d_usrp->set_adc_offset (which, offset);
}

bool
usrp1_source_base::set_dac_offset (int which, int offset, int offset_pin)
{
  return d_usrp->set_dac_offset (which, offset, offset_pin);
}

bool
usrp1_source_base::set_adc_buffer_bypass (int which, bool bypass)
{
  return d_usrp->set_adc_buffer_bypass (which, bypass);
}

std::string
usrp1_source_base::serial_number()
{
  return d_usrp->serial_number();
}

bool
usrp1_source_base::_write_oe (int which_dboard, int value, int mask)
{
  return d_usrp->_write_oe (which_dboard, value, mask);
}

bool
usrp1_source_base::write_io (int which_dboard, int value, int mask)
{
  return d_usrp->write_io (which_dboard, value, mask);
}

int
usrp1_source_base::read_io (int which_dboard)
{
  return d_usrp->read_io (which_dboard);
}




// internal routines...

bool
usrp1_source_base::_write_fpga_reg (int regno, int value)
{
  return d_usrp->_write_fpga_reg (regno, value);
}

bool
usrp1_source_base::_write_fpga_reg_masked (int regno, int value, int mask)
{
  return d_usrp->_write_fpga_reg_masked (regno, value, mask);
}

int
usrp1_source_base::_read_fpga_reg (int regno)
{
  return d_usrp->_read_fpga_reg (regno);
}

bool
usrp1_source_base::_write_9862 (int which_codec, int regno, unsigned char value)
{
  return d_usrp->_write_9862 (which_codec, regno, value);
}

int
usrp1_source_base::_read_9862 (int which_codec, int regno) const
{
  return d_usrp->_read_9862 (which_codec, regno);
}

bool
usrp1_source_base::_write_spi (int optional_header, int enables,
			       int format, std::string buf)
{
  return d_usrp->_write_spi (optional_header, enables, format, buf);
}

std::string
usrp1_source_base::_read_spi (int optional_header, int enables, int format, int len)
{
  return d_usrp->_read_spi (optional_header, enables, format, len);
}

bool
usrp1_source_base::set_format(unsigned int format)
{
  return d_usrp->set_format(format);
}

unsigned int
usrp1_source_base::format() const
{
  return d_usrp->format();
}

unsigned int
usrp1_source_base::make_format(int width, int shift, bool want_q, bool bypass_halfband)
{
  return usrp_standard_rx::make_format(width, shift, want_q, bypass_halfband);
}

int
usrp1_source_base::format_width(unsigned int format)
{
  return usrp_standard_rx::format_width(format);
}

int
usrp1_source_base::format_shift(unsigned int format)
{
  return usrp_standard_rx::format_shift(format);
}

bool
usrp1_source_base::format_want_q(unsigned int format)
{
  return usrp_standard_rx::format_want_q(format);
}

bool
usrp1_source_base::format_bypass_halfband(unsigned int format)
{
  return usrp_standard_rx::format_bypass_halfband(format);
}
