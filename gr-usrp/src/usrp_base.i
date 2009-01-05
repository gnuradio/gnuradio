/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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

%{
#include "usrp_base.h"
%}

class usrp_base : public gr_sync_block 
{
protected:
  usrp_base(const std::string &name,
	    gr_io_signature_sptr input_signature,
	    gr_io_signature_sptr output_signature)
    : gr_sync_block(name, input_signature, output_signature) {}
    
public:
  std::vector<std::vector<db_base_sptr> > db();
  std::vector<db_base_sptr> db(int which_side);
  db_base_sptr db(int which_side, int which_dev);
  %rename (_real_selected_subdev) selected_subdev;
  db_base_sptr selected_subdev(usrp_subdev_spec ss);
  long fpga_master_clock_freq() const;
  void set_fpga_master_clock_freq(long master_clock);
  void set_verbose (bool on);
  static const int READ_FAILED = -99999;
  bool write_eeprom (int i2c_addr, int eeprom_offset, const std::string buf);
  std::string read_eeprom (int i2c_addr, int eeprom_offset, int len);
  bool write_i2c (int i2c_addr, const std::string buf);
  std::string read_i2c (int i2c_addr, int len);
  bool set_adc_offset (int which_adc, int offset);
  bool set_dac_offset (int which_dac, int offset, int offset_pin);
  bool set_adc_buffer_bypass (int which_adc, bool bypass);
  bool set_dc_offset_cl_enable(int bits, int mask);
  std::string serial_number();
  virtual int daughterboard_id (int which_side) const;
  bool write_atr_tx_delay(int value);
  bool write_atr_rx_delay(int value);
  bool set_pga (int which_amp, double gain_in_db);
  double pga (int which_amp) const;
  double pga_min () const;
  double pga_max () const;
  double pga_db_per_step () const;
  bool _write_oe (int which_side, int value, int mask);
  bool write_io (int which_side, int value, int mask);
  int read_io (int which_side);
  //bool write_refclk(int which_side, int value);
  bool write_atr_mask(int which_side, int value);
  bool write_atr_txval(int which_side, int value);
  bool write_atr_rxval(int which_side, int value);
  bool write_aux_dac (int which_side, int which_dac, int value);
  int read_aux_adc (int which_side, int which_adc);
  long converter_rate() const;
  bool _set_led (int which_led, bool on);
  bool _write_fpga_reg (int regno, int value);
  //bool _read_fpga_reg (int regno, int *value);
  int  _read_fpga_reg (int regno);
  bool _write_fpga_reg_masked (int regno, int value, int mask);
  bool _write_9862 (int which_codec, int regno, unsigned char value);
  int  _read_9862 (int which_codec, int regno) const;
  bool _write_spi (int optional_header, int enables, int format, std::string buf);
  std::string _read_spi (int optional_header, int enables, int format, int len);
  %rename(_real_pick_subdev) pick_subdev;
  usrp_subdev_spec pick_subdev(std::vector<int> candidates=std::vector<int>(0))
    throw (std::runtime_error);
};
