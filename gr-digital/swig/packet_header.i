/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
