/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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

