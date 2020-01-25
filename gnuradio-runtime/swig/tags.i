/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%{
#include <gnuradio/tags.h>
%}

%import <pmt_swig.i> //for pmt support

%ignore gr::tag_t::operator=;
%include <gnuradio/tags.h>

//gives support for a vector of tags (get tags in range)
%include "std_vector.i"
%template(tags_vector_t) std::vector<gr::tag_t>;
