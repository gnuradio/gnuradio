/* -*- c++ -*- */
//
// Copyright 2008 Free Software Foundation, Inc.
// 
// This file is part of GNU Radio
// 
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either asversion 3, or (at your option)
// any later version.
// 
// GNU Radio is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING.  If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//

#ifndef INCLUDED_DB_BASE_H
#define INCLUDED_DB_BASE_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <iosfwd>

class db_base;
typedef boost::shared_ptr<db_base> db_base_sptr;

class usrp_basic;
typedef boost::shared_ptr<usrp_basic> usrp_basic_sptr;

struct freq_result_t
{
  bool  ok;
  double baseband_freq;
};

/******************************************************************************/

/*!
 * \brief Abstract base class for all USRP daughterboards
 * \ingroup usrp
 */
class db_base
{
 protected:
  bool				d_is_shutdown;
  usrp_basic		       *d_raw_usrp;
  int 				d_which;
  double			d_lo_offset;

  void _enable_refclk(bool enable);
  virtual double _refclk_freq();
  virtual int _refclk_divisor();

  usrp_basic *usrp(){
    return d_raw_usrp;
  }

 public:
  db_base(boost::shared_ptr<usrp_basic> usrp, int which);
  virtual ~db_base();

  int dbid();
  std::string name();
  std::string side_and_name();
  int which() { return d_which; }

  bool bypass_adc_buffers(bool bypass);
  bool set_atr_mask(int v);
  bool set_atr_txval(int v);
  bool set_atr_rxval(int v);
  bool set_atr_tx_delay(int v);
  bool set_atr_rx_delay(int v);
  bool set_lo_offset(double offset);
  double lo_offset() { return d_lo_offset; }


  ////////////////////////////////////////////////////////
  // derived classes should override the following methods

protected:
  friend class usrp_basic;

  /*!
   * Called to shutdown daughterboard.  Called from dtor and usrp_basic dtor.
   *
   * N.B., any class that overrides shutdown MUST call shutdown in its destructor.
   */
  virtual void shutdown();


public:
  virtual float gain_min() = 0;
  virtual float gain_max() = 0;
  virtual float gain_db_per_step() = 0;
  virtual double freq_min() = 0;
  virtual double freq_max() = 0;
  virtual struct freq_result_t set_freq(double target_freq) = 0;
  virtual bool set_gain(float gain) = 0;
  virtual bool is_quadrature() = 0;
  virtual bool i_and_q_swapped();
  virtual bool spectrum_inverted();
  virtual bool set_enable(bool on);
  virtual bool set_auto_tr(bool on);
  virtual bool select_rx_antenna(int which_antenna);
  virtual bool select_rx_antenna(const std::string &which_antenna);
  virtual bool set_bw(float bw);
};


std::ostream & operator<<(std::ostream &os, db_base &x);

#endif /* INCLUDED_DB_BASE_H */
