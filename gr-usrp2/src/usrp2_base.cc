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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <usrp2_base.h>
#include <gr_io_signature.h>
#include <iostream>

usrp2_base::usrp2_base(const char *name,
		       gr_io_signature_sptr input_signature,
		       gr_io_signature_sptr output_signature,
		       const std::string &ifc,
		       const std::string &mac) 
  throw (std::runtime_error)
  : gr_sync_block(name,
		  input_signature,
		  output_signature),
    d_u2(usrp2::usrp2::sptr())
{
  d_u2 = usrp2::usrp2::make(ifc, mac);
  if (!d_u2)
    throw std::runtime_error("Unable to initialize USRP2!");
}

usrp2_base::~usrp2_base ()
{
  // NOP
}

std::string
usrp2_base::mac_addr() const
{
  return d_u2->mac_addr();
}

std::string
usrp2_base::ifc_name() const
{
  return d_u2->ifc_name();
}

bool
usrp2_base::fpga_master_clock_freq(long *freq) const
{
  return d_u2->fpga_master_clock_freq(freq);
}

bool
usrp2_base::sync_to_pps()
{
  return d_u2->sync_to_pps();
}

std::vector<uint32_t>
usrp2_base::peek32(uint32_t addr, uint32_t words)
{
  return d_u2->peek32(addr, words);
}

bool
usrp2_base::poke32(uint32_t addr, const std::vector<uint32_t> &data)
{
  return d_u2->poke32(addr, data);
}

bool
usrp2_base::start()
{
  // Default implementation is NOP
  return true;
}

bool
usrp2_base::stop()
{
  // Default implementation is NOP
  return true;
}
