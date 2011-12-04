/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2010 Free Software Foundation, Inc.
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

%include "general_swig_doc.i"

#define GR_CORE_API

#ifndef SWIGIMPORTED
%module(directors="1") gnuradio_core_general
#endif

 //%feature("autodoc", "1");		// generate python docstrings

%include "gnuradio.i"				// the common stuff

%include "general.i"

 // Simple test case for complex input and output
%inline
%{
  std::complex<float> complexf_add_2j(std::complex<float> x)
  {
    return std::complex<float>(x.real(), x.imag() + 2);
  }

  std::complex<double> complexd_add_2j(std::complex<double> x)
  {
    return std::complex<double>(x.real(), x.imag() + 2);
  }

  std::complex<float> complexf_add_x_2j(float x, std::complex<float> y)
  {
    return std::complex<float>(x + y.real(), y.imag() + 2);
  }

%}

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-gnuradio_core_general" "scm_init_gnuradio_gnuradio_core_general_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
