/* -*- c++ -*- */
/*
 * Copyright 2006,2010 Free Software Foundation, Inc.
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

class gr_basic_block;
typedef boost::shared_ptr<gr_basic_block> gr_basic_block_sptr;
%template(gr_basic_block_sptr) boost::shared_ptr<gr_basic_block>;

// support vectors of these...
namespace std {
  %template(x_vector_gr_basic_block_sptr) vector<gr_basic_block_sptr>;
};

class gr_basic_block
{
protected:
    gr_basic_block();

public:
    virtual ~gr_basic_block();
    std::string name() const;
    gr_io_signature_sptr input_signature() const;
    gr_io_signature_sptr output_signature() const;
    long unique_id() const;
    gr_basic_block_sptr to_basic_block();
    bool check_topology (int ninputs, int noutputs);
};

%rename(block_ncurrently_allocated) gr_basic_block_ncurrently_allocated;
long gr_basic_block_ncurrently_allocated();

#ifdef SWIGPYTHON
%pythoncode %{
gr_basic_block_sptr.__repr__ = lambda self: "<gr_basic_block %s (%d)>" % (self.name(), self.unique_id ())
%}
#endif
