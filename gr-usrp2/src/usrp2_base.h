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

#ifndef INCLUDED_USRP2_BASE_H
#define INCLUDED_USRP2_BASE_H

#include <gr_sync_block.h>
#include <usrp2/usrp2.h>
#include <stdexcept>

// BIG ASS FIXME: get from lower layer MTU calculation
#define USRP2_MIN_RX_SAMPLES 371

/*!
 * Base class for all USRP2 blocks
 */
class usrp2_base : public gr_sync_block
{
protected:
  usrp2_base(const char *name,
	     gr_io_signature_sptr input_signature,
	     gr_io_signature_sptr output_signature,
	     const std::string &ifc,
	     const std::string &mac)
    throw (std::runtime_error);

  usrp2::usrp2::sptr d_u2;

public:
  ~usrp2_base();

  /*!
   * \brief Get USRP2 hardware MAC address
   */
  std::string mac_addr() const;
  
  /*!
   * \brief Get interface name used to communicat with USRP2
   */
  std::string interface_name() const;

  /*!
   * \brief Get USRP2 master clock rate
   */
  bool fpga_master_clock_freq(long *freq) const;

  /*!
   * \brief Set master time to 0 at next PPS rising edge
   */
  bool sync_to_pps();

  /*!
   * \brief Read memory from Wishbone bus as words
   */
  std::vector<uint32_t> peek32(uint32_t addr, uint32_t words);

  /*!
   * \brief Write memory to Wishbone bus as words
   */
  bool poke32(uint32_t addr, const std::vector<uint32_t> &data);

  /*!
   * \brief Called by scheduler when starting flowgraph
   */
  virtual bool start();
  
  /*!
   * \brief Called by scheduler when stopping flowgraph
   */
  virtual bool stop();

  /*!
   * \brief Derived class must override this
   */
  virtual int work(int noutput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items) = 0;
};

#endif /* INCLUDED_USRP2_BASE_H */
