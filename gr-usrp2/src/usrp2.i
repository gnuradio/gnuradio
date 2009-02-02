/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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

%feature("autodoc", "1");		// generate python docstrings

%import(module="gnuradio_swig_py_runtime") "gnuradio.i"	// the common stuff
%import <stdint.i>     

%{
#include <gnuradio_swig_bug_workaround.h>
#include "usrp2_source_16sc.h"
#include "usrp2_source_32fc.h"
#include "usrp2_sink_16sc.h"
#include "usrp2_sink_32fc.h"
%}

%include <usrp2/tune_result.h>

%template(uint32_t_vector) std::vector<uint32_t>;

// ----------------------------------------------------------------

class usrp2_base : public gr_sync_block 
{
protected:
  usrp2_base() throw (std::runtime_error);

public:
  ~usrp2_base();

  std::string mac_addr() const;
  %rename(_real_fpga_master_clock_freq) fpga_master_clock_freq;
  bool fpga_master_clock_freq(long *freq);
  bool sync_to_pps();
  std::vector<uint32_t> peek32(uint32_t addr, uint32_t words);
  bool poke32(uint32_t addr, const std::vector<uint32_t> &data);
};

// ----------------------------------------------------------------

class usrp2_source_base : public usrp2_base 
{
protected:
  usrp2_source_base() throw (std::runtime_error);

public:
  ~usrp2_source_base();

  bool set_gain(double gain);
  %rename(_real_set_center_freq) set_center_freq;
  bool set_center_freq(double frequency, usrp2::tune_result *r);
  bool set_decim(int decimation_factor);
  bool set_scale_iq(int scale_i, int scale_q);
  int decim();
  %rename(_real_adc_rate) adc_rate;
  bool adc_rate(long *rate);
  double gain_min();
  double gain_max();
  double gain_db_per_step();
  double freq_min();
  double freq_max();
  %rename(_real_daughterboard_id) daughterboard_id;
  bool daughterboard_id(int *dbid);
  unsigned int overruns();
  unsigned int missing();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(usrp2,source_32fc)

usrp2_source_32fc_sptr
usrp2_make_source_32fc(const std::string ifc="eth0", 
                       const std::string mac="") 
  throw (std::runtime_error);

class usrp2_source_32fc : public usrp2_source_base 
{
protected:
  usrp2_source_32fc(const std::string &ifc, const std::string &mac);

public:
  ~usrp2_source_32fc();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(usrp2,source_16sc)

usrp2_source_16sc_sptr
usrp2_make_source_16sc(const std::string ifc="eth0", 
		       const std::string mac="") 
  throw (std::runtime_error);

class usrp2_source_16sc : public usrp2_source_base 
{
protected:
  usrp2_source_16sc(const std::string &ifc, const std::string &mac);

public:
  ~usrp2_source_16sc();
};

// ----------------------------------------------------------------

class usrp2_sink_base : public usrp2_base 
{
protected:
  usrp2_sink_base() throw (std::runtime_error);

public:
  ~usrp2_sink_base();

  bool set_gain(double gain);
  %rename(_real_set_center_freq) set_center_freq;
  bool set_center_freq(double frequency, usrp2::tune_result *r);
  bool set_interp(int interp_factor);
  bool set_scale_iq(int scale_i, int scale_q);
  int interp();
  %rename(_real_default_tx_scale_iq) default_scale_iq;
  void default_scale_iq(int interp, int *scale_i, int *scale_q);
  %rename(_real_dac_rate) dac_rate;
  bool dac_rate(long *rate);
  double gain_min();
  double gain_max();
  double gain_db_per_step();
  double freq_min();
  double freq_max();
  %rename(_real_daughterboard_id) daughterboard_id;
  bool daughterboard_id(int *dbid);
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(usrp2,sink_32fc)

usrp2_sink_32fc_sptr
usrp2_make_sink_32fc(const std::string ifc="eth0", 
		     const std::string mac="") 
  throw (std::runtime_error);

class usrp2_sink_32fc : public usrp2_sink_base 
{
protected:
  usrp2_sink_32fc(const std::string &ifc, const std::string &mac);

public:
  ~usrp2_sink_32fc();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(usrp2,sink_16sc)

usrp2_sink_16sc_sptr
usrp2_make_sink_16sc(const std::string ifc="eth0", 
		     const std::string mac="") 
  throw (std::runtime_error);

class usrp2_sink_16sc : public usrp2_sink_base 
{
protected:
  usrp2_sink_16sc(const std::string &ifc, const std::string &mac);

public:
  ~usrp2_sink_16sc();
};

// ----------------------------------------------------------------

// some utility functions to allow Python to deal with pointers
%{
  long *make_long_ptr() { return (long *)malloc(sizeof(long)); }
  long deref_long_ptr(long *l) { return *l; }
  void free_long_ptr(long *l) { free(l); }
  int *make_int_ptr() { return (int *)malloc(sizeof(int)); }
  int deref_int_ptr(int *l) { return *l; }
  void free_int_ptr(int *l) { free(l); }
%}

long *make_long_ptr();
long deref_long_ptr(long *l);
void free_long_ptr(long *l);
int *make_int_ptr();
int deref_int_ptr(int *l);
void free_int_ptr(int *l);

// create a more pythonic interface
%pythoncode %{

def __set_center_freq(self, freq):
  tr = tune_result()
  r = self._real_set_center_freq(freq, tr)
  if r:
    return tr
  else:
    return None

def __fpga_master_clock_freq(self):
  f = make_long_ptr();
  r = self._real_fpga_master_clock_freq(f)
  if r:
    result = deref_long_ptr(f)
  else:
    result = None
  free_long_ptr(f)
  return result

def __adc_rate(self):
  rate = make_long_ptr();
  r = self._real_adc_rate(rate)
  if r:
    result = deref_long_ptr(rate)
  else:
    result = None
  free_long_ptr(rate)
  return result

def __dac_rate(self):
  rate = make_long_ptr();
  r = self._real_dac_rate(rate)
  if r:
    result = deref_long_ptr(rate)
  else:
    result = None
  free_long_ptr(rate)
  return result

def __gain_range(self):
  return [self.gain_min(),
          self.gain_max(),
          self.gain_db_per_step()]

# NOTE: USRP1 uses a length three tuple here (3rd value is 'freq step'),
#       but it's not really useful.  We let an index error happen here
#       to identify code using it.
def __freq_range(self):
  return [self.freq_min(),
          self.freq_max()]

def __daughterboard_id(self):
  dbid = make_int_ptr()
  r = self._real_daughterboard_id(dbid)
  if r:
    result = deref_int_ptr(dbid)
  else:
    result = None
  free_int_ptr(dbid)
  return result

def __default_tx_scale_iq(self, interp):
  scale_i = make_int_ptr()
  scale_q = make_int_ptr()
  self._real_default_tx_scale_iq(interp, scale_i, scale_q)
  return (deref_int_ptr(scale_i), deref_int_ptr(scale_q))

usrp2_source_32fc_sptr.set_center_freq = __set_center_freq
usrp2_source_16sc_sptr.set_center_freq = __set_center_freq
usrp2_sink_32fc_sptr.set_center_freq = __set_center_freq
usrp2_sink_16sc_sptr.set_center_freq = __set_center_freq

usrp2_source_32fc_sptr.fpga_master_clock_freq = __fpga_master_clock_freq
usrp2_source_16sc_sptr.fpga_master_clock_freq = __fpga_master_clock_freq
usrp2_sink_32fc_sptr.fpga_master_clock_freq = __fpga_master_clock_freq
usrp2_sink_16sc_sptr.fpga_master_clock_freq = __fpga_master_clock_freq

usrp2_source_32fc_sptr.adc_rate = __adc_rate
usrp2_source_16sc_sptr.adc_rate = __adc_rate
usrp2_sink_32fc_sptr.dac_rate = __dac_rate
usrp2_sink_16sc_sptr.dac_rate = __dac_rate

usrp2_source_32fc_sptr.gain_range = __gain_range
usrp2_source_16sc_sptr.gain_range = __gain_range
usrp2_sink_32fc_sptr.gain_range = __gain_range
usrp2_sink_16sc_sptr.gain_range = __gain_range

usrp2_source_32fc_sptr.freq_range = __freq_range
usrp2_source_16sc_sptr.freq_range = __freq_range
usrp2_sink_32fc_sptr.freq_range = __freq_range
usrp2_sink_16sc_sptr.freq_range = __freq_range

usrp2_source_32fc_sptr.daughterboard_id = __daughterboard_id
usrp2_source_16sc_sptr.daughterboard_id = __daughterboard_id
usrp2_sink_32fc_sptr.daughterboard_id = __daughterboard_id
usrp2_sink_16sc_sptr.daughterboard_id = __daughterboard_id

usrp2_sink_32fc_sptr.default_scale_iq = __default_tx_scale_iq
usrp2_sink_16sc_sptr.default_scale_iq = __default_tx_scale_iq

%}
