/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

%template(ofdm_equalizer_base_sptr) boost::shared_ptr<gr::digital::ofdm_equalizer_base>;
%template(ofdm_equalizer_1d_pilots_sptr) boost::shared_ptr<gr::digital::ofdm_equalizer_1d_pilots>;
%pythoncode %{
ofdm_equalizer_1d_pilots_sptr.__repr__ = lambda self: "<OFDM equalizer 1D base class>"
%}

%template(ofdm_equalizer_simpledfe_sptr) boost::shared_ptr<gr::digital::ofdm_equalizer_simpledfe>;
%pythoncode %{
ofdm_equalizer_simpledfe_sptr.__repr__ = lambda self: "<OFDM equalizer simpledfe>"
ofdm_equalizer_simpledfe = ofdm_equalizer_simpledfe.make;
%}
//%rename(ofdm_equalizer_simpledfe) make_ofdm_equalizer_simpledfe;
//%ignore ofdm_equalizer_simpledfe;

%template(ofdm_equalizer_static_sptr) boost::shared_ptr<gr::digital::ofdm_equalizer_static>;
%pythoncode %{
ofdm_equalizer_static_sptr.__repr__ = lambda self: "<OFDM equalizer static>"
ofdm_equalizer_static = ofdm_equalizer_static.make;
%}
//%rename(ofdm_equalizer_static) make_ofdm_equalizer_static;
//%ignore ofdm_equalizer_static;

