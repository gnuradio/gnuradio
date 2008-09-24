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

%feature("autodoc", "1");		// generate python docstrings

%include "exception.i"
%import "gnuradio.i"			// the common stuff

%{
#include <gnuradio_swig_bug_workaround.h>
#include "usrp2_source_16sc.h"
#include "usrp2_source_32fc.h"
#include "usrp2_sink_16sc.h"
#include "usrp2_sink_32fc.h"
%}

%include <usrp2/tune_result.h>

// ----------------------------------------------------------------

class usrp2_base : public gr_sync_block 
{
protected:
  usrp2_base() throw (std::runtime_error);

public:
  ~usrp2_base();

  std::string mac_addr() const;
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

// create a more pythonic interface
%pythoncode %{

def __set_center_freq(self, freq):
  tr = tune_result()
  r = self._real_set_center_freq(freq, tr)
  if r:
    return tr
  else:
    return None

usrp2_source_32fc_sptr.set_center_freq = __set_center_freq
usrp2_source_16sc_sptr.set_center_freq = __set_center_freq
usrp2_sink_32fc_sptr.set_center_freq = __set_center_freq
usrp2_sink_16sc_sptr.set_center_freq = __set_center_freq
%}
