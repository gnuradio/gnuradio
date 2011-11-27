/* -*- c++ -*- */
/*
 * Copyright 2009 Free Software Foundation, Inc.
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

%include "gnuradio.i"

//load generated python docstrings
%include "noaa_swig_doc.i"

%{
#include <noaa_hrpt_decoder.h>
#include <noaa_hrpt_deframer.h>
#include <noaa_hrpt_pll_cf.h>
%}

%include "noaa_hrpt_decoder.i"
%include "noaa_hrpt_deframer.i"
%include "noaa_hrpt_pll_cf.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-noaa_swig" "scm_init_gnuradio_noaa_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
