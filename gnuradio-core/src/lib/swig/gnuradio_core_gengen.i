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

%include "gengen_swig_doc.i"

#ifndef SWIGIMPORTED
%module(directors="1") gnuradio_core_gengen
#endif

 //%feature("autodoc", "1");		// generate python docstrings

%include "gnuradio.i"				// the common stuff

%include "gengen.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-gnuradio_core_gengen" "scm_init_gnuradio_gnuradio_core_gengen_module")
%}

%goops %{
  (use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
