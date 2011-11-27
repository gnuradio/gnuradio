/*
 * Copyright 2005,2006,2009 Free Software Foundation, Inc.
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
%include "pager_swig_doc.i"

%{
#include "pager_flex_frame.h"
#include "pager_slicer_fb.h"
#include "pager_flex_sync.h"
#include "pager_flex_deinterleave.h"
#include "pager_flex_parse.h"
%}

%include "pager_flex_frame.i"
%include "pager_slicer_fb.i"
%include "pager_flex_sync.i"
%include "pager_flex_deinterleave.i"
%include "pager_flex_parse.i"

#if SWIGGUILE
%scheme %{
(load-extension-global "libguile-gnuradio-pager_swig" "scm_init_gnuradio_pager_swig_module")
%}

%goops %{
(use-modules (gnuradio gnuradio_core_runtime))
%}
#endif
