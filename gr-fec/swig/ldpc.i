/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
