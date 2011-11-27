/* -*- c++ -*- */
/*
 * Copyright 2005,2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"				// the common stuff

//load generated python docstrings
%include "comedi_swig_doc.i"

%{
#include "comedi_sink_s.h"
#include "comedi_source_s.h"
%}

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(comedi,source_s)

comedi_source_s_sptr
comedi_make_source_s (int sampling_freq,
			const std::string dev = ""
			) throw (std::runtime_error);

class comedi_source_s : public gr_sync_block {

 protected:
  comedi_source_s (int sampling_freq,
		     const std::string device_name
		     ) throw (std::runtime_error);

 public:
  ~comedi_source_s ();

  bool start();
  bool stop();
};

// ----------------------------------------------------------------

GR_SWIG_BLOCK_MAGIC(comedi,sink_s)

comedi_sink_s_sptr
comedi_make_sink_s (int sampling_freq,
		      const std::string dev = ""
		      ) throw (std::runtime_error);

class comedi_sink_s : public gr_sync_block {

 protected:
  comedi_sink_s (int sampling_freq,
		   const std::string device_name
		   ) throw (std::runtime_error);

 public:
  ~comedi_sink_s ();
};

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-comedi" "scm_init_gnuradio_comedi_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
