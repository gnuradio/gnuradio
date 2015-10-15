/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

%template(matrix_sptr) boost::shared_ptr<gr::fec::code::matrix>;
%template(fec_mtrx_sptr) boost::shared_ptr<gr::fec::code::fec_mtrx>;

%template(ldpc_H_matrix_sptr) boost::shared_ptr<gr::fec::code::ldpc_H_matrix>;
%pythoncode %{
  ldpc_H_matrix = ldpc_H_matrix.make;
%}

%template(ldpc_G_matrix_sptr) boost::shared_ptr<gr::fec::code::ldpc_G_matrix>;
%pythoncode %{
  ldpc_G_matrix = ldpc_G_matrix.make;
%}
