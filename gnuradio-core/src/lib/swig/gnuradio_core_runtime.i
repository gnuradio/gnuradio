/* -*- c++ -*- */
/*
 * Copyright 2009,2010 Free Software Foundation, Inc.
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

%include "runtime_swig_doc.i"

#ifndef SWIGIMPORTED
%module(directors="1") gnuradio_core_runtime
#endif

 //%feature("autodoc", "1");		// generate python docstrings

#define SW_RUNTIME
%include "gnuradio.i"				// the common stuff

%include "runtime.i"


#if SWIGGUILE
%scheme %{

;; Load our gsubr that loads libraries using the RTLD_GLOBAL option
(load-extension "libguile-gnuradio-dynl-global" "scm_init_gnuradio_dynl_global_module")

;; Define load-extension-global in module '(guile)
(module-define! (resolve-module '(guile)) 
		'load-extension-global
		(lambda (lib init)
		  (dynamic-call init (dynamic-link-global lib))))

;; Use load-extension-global to load our swig modules
(load-extension-global "libguile-gnuradio-gnuradio_core_runtime" "scm_init_gnuradio_gnuradio_core_runtime_module")
%}

%goops %{
(use-modules (gnuradio export-safely))
(re-export export-syms-if-not-imported-gf)
(re-export-syntax export-safely)
(re-export re-export-all)
%}
#endif
