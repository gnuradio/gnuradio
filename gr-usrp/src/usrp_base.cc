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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <usrp_base.h>
#include <usrp/usrp_basic.h>

class truth_table_element_t 
{
public:
  truth_table_element_t(int side, unsigned int uses, bool swap_iq, unsigned int mux_val);
  bool operator==(const truth_table_element_t &in);
  bool operator!=(const truth_table_element_t &in);

  unsigned int mux_val() { return d_mux_val; }

private:
  int          d_side;
  unsigned int d_uses;
  bool         d_swap_iq;
  unsigned int d_mux_val;
};


usrp_base::~usrp_base()
{
}

void
usrp_base::set_usrp_basic(boost::shared_ptr<usrp_basic> u)
{
  d_usrp_basic = u;
}

std::vector<std::vector<db_base_sptr> >
usrp_base::db()
{
  return d_usrp_basic->db();
}

std::vector<db_base_sptr> 
usrp_base::db(int which_side)
{
  return d_usrp_basic->db(which_side);
}

db_base_sptr
usrp_base::db(int which_side, int which_dev)
{
  return d_usrp_basic->selected_subdev(usrp_subdev_spec(which_side, which_dev));
}

db_base_sptr
usrp_base::selected_subdev(usrp_subdev_spec ss)
{
  return d_usrp_basic->selected_subdev(ss);
}

long
usrp_base::fpga_master_clock_freq() const
{
  return d_usrp_basic->fpga_master_clock_freq();
}

void
usrp_base::set_fpga_master_clock_freq(long master_clock)
{
  d_usrp_basic->set_fpga_master_clock_freq(master_clock);
}

void
usrp_base::set_verbose (bool verbose)
{  
  d_usrp_basic->set_verbose (verbose);
}

bool
usrp_base::write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf)
{
  return d_usrp_basic->write_eeprom (i2c_addr, eeprom_offset, buf);
}

std::string
usrp_base::read_eeprom (int i2c_addr, int eeprom_offset, int len)
{
  return d_usrp_basic->read_eeprom (i2c_addr, eeprom_offset, len);
}

bool
usrp_base::write_i2c (int i2c_addr, const std::string buf)
{
  return d_usrp_basic->write_i2c (i2c_addr, buf);
}

std::string
usrp_base::read_i2c (int i2c_addr, int len)
{
  return d_usrp_basic->read_i2c (i2c_addr, len);
}

bool
usrp_base::set_adc_offset (int which, int offset)
{
  return d_usrp_basic->set_adc_offset (which, offset);
}

bool
usrp_base::set_dac_offset (int which, int offset, int offset_pin)
{
  return d_usrp_basic->set_dac_offset (which, offset, offset_pin);
}

bool
usrp_base::set_adc_buffer_bypass (int which, bool bypass)
{
  return d_usrp_basic->set_adc_buffer_bypass (which, bypass);
}

bool
usrp_base::set_dc_offset_cl_enable(int bits, int mask)
{
  return d_usrp_basic->set_dc_offset_cl_enable(bits, mask);
}

std::string
usrp_base::serial_number()
{
  return d_usrp_basic->serial_number();
}

int
usrp_base::daughterboard_id (int which) const
{
  return d_usrp_basic->daughterboard_id (which);
}

bool
usrp_base::write_atr_tx_delay(int value)
{
  return d_usrp_basic->write_atr_tx_delay(value);
}

bool
usrp_base::write_atr_rx_delay(int value)
{
  return d_usrp_basic->write_atr_rx_delay(value);
}

bool
usrp_base::set_pga (int which, double gain)
{
  return d_usrp_basic->set_pga (which, gain);
}

double
usrp_base::pga (int which) const
{
  return d_usrp_basic->pga (which);
}

double
usrp_base::pga_min () const
{
  return d_usrp_basic->pga_min ();
}

double
usrp_base::pga_max () const
{
  return d_usrp_basic->pga_max ();
}

double
usrp_base::pga_db_per_step () const
{
  return d_usrp_basic->pga_db_per_step ();
}

bool
usrp_base::_write_oe (int which_dboard, int value, int mask)
{
  return d_usrp_basic->_write_oe (which_dboard, value, mask);
}

bool
usrp_base::write_io (int which_dboard, int value, int mask)
{
  return d_usrp_basic->write_io (which_dboard, value, mask);
}

int
usrp_base::read_io (int which_dboard)
{
  return d_usrp_basic->read_io (which_dboard);
}

bool
usrp_base::write_atr_mask(int which_side, int value)
{
  return d_usrp_basic->write_atr_mask(which_side, value);
}

bool
usrp_base::write_atr_txval(int which_side, int value)
{
  return d_usrp_basic->write_atr_txval(which_side, value);
}

bool
usrp_base::write_atr_rxval(int which_side, int value)
{
  return d_usrp_basic->write_atr_rxval(which_side, value);
}

bool
usrp_base::write_aux_dac (int which_dboard, int which_dac, int value)
{
  return d_usrp_basic->write_aux_dac (which_dboard, which_dac, value);
}

int
usrp_base::read_aux_adc (int which_dboard, int which_adc)
{
  return d_usrp_basic->read_aux_adc (which_dboard, which_adc);
}

long
usrp_base::converter_rate() const
{
  return d_usrp_basic->converter_rate();
}

bool
usrp_base::_set_led(int which_led, bool on)
{
  return d_usrp_basic->_set_led(which_led, on);
}

bool
usrp_base::_write_fpga_reg (int regno, int value)
{
  return d_usrp_basic->_write_fpga_reg (regno, value);
}

bool
usrp_base::_write_fpga_reg_masked (int regno, int value, int mask)
{
  return d_usrp_basic->_write_fpga_reg_masked (regno, value, mask);
}

int
usrp_base::_read_fpga_reg (int regno)
{
  return d_usrp_basic->_read_fpga_reg (regno);
}

bool
usrp_base::_write_9862 (int which_codec, int regno, unsigned char value)
{
  return d_usrp_basic->_write_9862 (which_codec, regno, value);
}

int
usrp_base::_read_9862 (int which_codec, int regno) const
{
  return d_usrp_basic->_read_9862 (which_codec, regno);
}

bool
usrp_base::_write_spi (int optional_header, int enables,
			       int format, std::string buf)
{
  return d_usrp_basic->_write_spi (optional_header, enables, format, buf);
}

std::string
usrp_base::_read_spi (int optional_header, int enables, int format, int len)
{
  return d_usrp_basic->_read_spi (optional_header, enables, format, len);
}

usrp_subdev_spec
usrp_base::pick_subdev(std::vector<int> candidates)
{
  int dbid0 = db(0, 0)->dbid();
  int dbid1 = db(1, 0)->dbid();

  for (int i = 0; i < candidates.size(); i++) {
    int dbid = candidates[i];
    if (dbid0 == dbid)
      return usrp_subdev_spec(0, 0);
    if (dbid1 == dbid)
      return usrp_subdev_spec(1, 0);
  }

  if (dbid0 >= 0)
    return usrp_subdev_spec(0, 0);
  if (dbid1 >= 0)
    return usrp_subdev_spec(1, 0);

  throw std::runtime_error("No suitable daughterboard found!");
}
