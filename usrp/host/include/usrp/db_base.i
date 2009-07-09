/* -*- c++ -*- */
//
// Copyright 2008,2009 Free Software Foundation, Inc.
// 
// This file is part of GNU Radio
// 
// GNU Radio is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
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


%{
#include <usrp/db_base.h>
%}

%include <gr_shared_ptr.i>

class usrp_tune_result
{
public:  
  usrp_tune_result(double baseband=0, double dxc=0, 
		   double residual=0, bool inv=0);
  ~usrp_tune_result();

  double baseband_freq;
  double dxc_freq;
  double residual_freq;
  bool  inverted;
};

struct freq_result_t
{
  bool  ok;
  double baseband_freq;
};

class db_base
{
 private:
  db_base(boost::shared_ptr<usrp_basic> usrp, int which);

 public:
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
  virtual bool select_rx_antenna(const std::string &antenna);
  virtual bool set_bw(float bw);
};

// Create templates for db's, vectors of db's, and vector of vectors of db's
typedef boost::shared_ptr<db_base> db_base_sptr;
%template(db_base_sptr) boost::shared_ptr<db_base>;
%template(db_base_sptr_vector) std::vector<db_base_sptr>;
%template(db_base_sptr_vector_vector) std::vector<std::vector<db_base_sptr> >;

// Set better class name in Python
// Enable freq_range and gain_range from public methods of class not implemented in C++
// And create a dummy wrapper for backwards compatability with some of the example code
%pythoncode %{
  db_base_sptr.__repr__ = lambda self: "<db_base::%s>" % (self.name(),)
  db_base_sptr.freq_range = lambda self: (self.freq_min(), self.freq_max(), 1)
  db_base_sptr.gain_range = lambda self: (self.gain_min(), self.gain_max(), self.gain_db_per_step())

%}
