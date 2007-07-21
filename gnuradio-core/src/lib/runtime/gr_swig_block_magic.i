/* -*- c++ -*- */
/*
 * Copyright 2004 Free Software Foundation, Inc.
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

%define GR_SWIG_BLOCK_MAGIC(PKG, BASE_NAME)
_GR_SWIG_BLOCK_MAGIC_HELPER(PKG, PKG ## _ ## BASE_NAME, BASE_NAME)
%enddef

%define _GR_SWIG_BLOCK_MAGIC_HELPER(PKG, NAME, BASE_NAME)
class NAME;
typedef boost::shared_ptr<NAME> NAME ## _sptr;
%template(NAME ## _sptr) boost::shared_ptr<NAME>;
%rename(BASE_NAME) PKG ## _make_ ## BASE_NAME;
%inline {
  gr_block_sptr NAME ## _block (NAME ## _sptr r)
  {
    return gr_block_sptr (r);
  }
}

%pythoncode %{
NAME ## _sptr.block = lambda self: NAME ## _block (self)
NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id ())
%}

%ignore NAME;
%enddef
