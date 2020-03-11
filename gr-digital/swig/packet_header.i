/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%template(packet_header_default_sptr) boost::shared_ptr<gr::digital::packet_header_default>;
%pythoncode %{
packet_header_default_sptr.__repr__ = lambda self: "<packet_header_default>"
packet_header_default = packet_header_default .make;
%}

%template(packet_header_ofdm_sptr) boost::shared_ptr<gr::digital::packet_header_ofdm>;
%pythoncode %{
packet_header_ofdm_sptr.__repr__ = lambda self: "<packet_header_ofdm>"
packet_header_ofdm = packet_header_ofdm .make;
%}

%template(header_format_base_sptr) boost::shared_ptr<gr::digital::header_format_base>;

%template(header_format_default_sptr) boost::shared_ptr<gr::digital::header_format_default>;
%pythoncode %{
header_format_default_sptr.__repr__ = lambda self: "<header_format_default>"
header_format_default = header_format_default .make;
%}


%template(header_format_counter_sptr) boost::shared_ptr<gr::digital::header_format_counter>;
%pythoncode %{
header_format_counter_sptr.__repr__ = lambda self: "<header_format_counter>"
header_format_counter = header_format_counter .make;
%}

%template(header_format_crc_sptr) boost::shared_ptr<gr::digital::header_format_crc>;
%pythoncode %{
header_format_crc_sptr.__repr__ = lambda self: "<header_format_crc>"
header_format_crc = header_format_crc .make;
%}

%template(header_format_ofdm_sptr) boost::shared_ptr<gr::digital::header_format_ofdm>;
%pythoncode %{
header_format_ofdm_sptr.__repr__ = lambda self: "<header_format_ofdm>"
header_format_ofdm = header_format_ofdm .make;
%}
