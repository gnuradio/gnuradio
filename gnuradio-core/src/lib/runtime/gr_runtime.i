/* -*- c++ -*- */
/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

class gr_runtime;
typedef boost::shared_ptr<gr_runtime> gr_runtime_sptr;
%template(gr_runtime_sptr) boost::shared_ptr<gr_runtime>;

gr_runtime_sptr gr_make_runtime(gr_hier_block2_sptr top_block);

class gr_runtime
{
protected:
    gr_runtime(gr_hier_block2_sptr top_block);

public:
    void run() throw (std::runtime_error);
    void start() throw (std::runtime_error);
    void stop() throw (std::runtime_error);
    void wait() throw (std::runtime_error);
};
