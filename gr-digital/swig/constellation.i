/* -*- c++ -*- */
/*
 * Copyright 2010-2012 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%template(constellation_sptr) std::shared_ptr<gr::digital::constellation>;

%template(constellation_calcdist_sptr) std::shared_ptr<gr::digital::constellation_calcdist>;
%pythoncode %{
constellation_calcdist_sptr.__repr__ = lambda self: "<constellation calcdist (m=%d)>" % (len(self.points()))
constellation_calcdist = constellation_calcdist.make;
%}

%template(constellation_rect_sptr) std::shared_ptr<gr::digital::constellation_rect>;
%pythoncode %{
constellation_rect_sptr.__repr__ = lambda self: "<constellation rect (m=%d)>" % (len(self.points()))
constellation_rect = constellation_rect.make;
%}

%template(constellation_expl_rect_sptr) std::shared_ptr<gr::digital::constellation_expl_rect>;
%pythoncode %{
constellation_expl_rect_sptr.__repr__ = lambda self: "<constellation expl rect (m=%d)>" % (len(self.points()))
constellation_expl_rect = constellation_expl_rect.make;
%}

%template(constellation_psk_sptr) std::shared_ptr<gr::digital::constellation_psk>;
%pythoncode %{
constellation_psk_sptr.__repr__ = lambda self: "<constellation PSK (m=%d)>" % (len(self.points()))
constellation_psk = constellation_psk.make;
%}

%template(constellation_bpsk_sptr) std::shared_ptr<gr::digital::constellation_bpsk>;
%pythoncode %{
constellation_bpsk_sptr.__repr__ = lambda self: "<constellation BPSK>"
constellation_bpsk = constellation_bpsk.make;
%}

%template(constellation_qpsk_sptr) std::shared_ptr<gr::digital::constellation_qpsk>;
%pythoncode %{
constellation_qpsk_sptr.__repr__ = lambda self: "<constellation QPSK>"
constellation_qpsk = constellation_qpsk.make;
%}

%template(constellation_dqpsk_sptr) std::shared_ptr<gr::digital::constellation_dqpsk>;
%pythoncode %{
constellation_dqpsk_sptr.__repr__ = lambda self: "<constellation DQPSK>"
constellation_dqpsk = constellation_dqpsk.make;
%}

%template(constellation_8psk_sptr) std::shared_ptr<gr::digital::constellation_8psk>;
%pythoncode %{
constellation_8psk_sptr.__repr__ = lambda self: "<constellation 8PSK>"
constellation_8psk = constellation_8psk.make;
%}

%template(constellation_8psk_natural_sptr) std::shared_ptr<gr::digital::constellation_8psk_natural>;
%pythoncode %{
constellation_8psk_natural_sptr.__repr__ = lambda self: "<constellation 8PSK_natural>"
constellation_8psk_natural = constellation_8psk_natural.make;
%}

%template(constellation_16qam_sptr) std::shared_ptr<gr::digital::constellation_16qam>;
%pythoncode %{
constellation_16qam_sptr.__repr__ = lambda self: "<constellation 16qam>"
constellation_16qam = constellation_16qam.make;
%}
