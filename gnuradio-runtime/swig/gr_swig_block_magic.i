/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012,2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

%define GR_SWIG_BLOCK_MAGIC(PKG, BASE_NAME)
_GR_SWIG_BLOCK_MAGIC_HELPER(PKG, BASE_NAME, PKG ## _ ## BASE_NAME)
%enddef

%define _GR_SWIG_BLOCK_MAGIC_HELPER_COMMON(PKG, BASE_NAME, FULL_NAME)
class FULL_NAME;
typedef boost::shared_ptr<FULL_NAME> FULL_NAME ## _sptr;
%template(FULL_NAME ## _sptr) boost::shared_ptr<FULL_NAME>;
%rename(BASE_NAME) PKG ## _make_ ## BASE_NAME;
%ignore FULL_NAME;
%enddef

#ifdef SWIGPYTHON
%define _GR_SWIG_BLOCK_MAGIC_HELPER(PKG, BASE_NAME, FULL_NAME)
_GR_SWIG_BLOCK_MAGIC_HELPER_COMMON(PKG, BASE_NAME, FULL_NAME)
%pythoncode %{
FULL_NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id ())
%}
%enddef
#endif

%define GR_SWIG_BLOCK_MAGIC2(PKG, BASE_NAME)
%template(BASE_NAME ## _sptr) boost::shared_ptr<gr:: ## PKG ## :: ## BASE_NAME>;
%pythoncode %{
BASE_NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id())
BASE_NAME = BASE_NAME.make;
%}
%enddef

%define GR_SWIG_BLOCK_MAGIC2_TMPL(PKG, BASE_NAME, TARGET_NAME...)
%template(BASE_NAME) gr:: ## PKG ## :: ## TARGET_NAME;
%template(BASE_NAME ## _sptr) boost::shared_ptr<gr:: ## PKG ## :: ## TARGET_NAME ## >;
%pythoncode %{
BASE_NAME ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id())
BASE_NAME = BASE_NAME.make
%}
%enddef


%define GR_SWIG_BLOCK_MAGIC_FACTORY(PKG, BASE_NAME, FACTORY)
%template(FACTORY ## _sptr) boost::shared_ptr<gr:: ## PKG ## :: ## BASE_NAME>;
%pythoncode %{
FACTORY ## _sptr.__repr__ = lambda self: "<gr_block %s (%d)>" % (self.name(), self.unique_id())
FACTORY = BASE_NAME ## _make_ ## FACTORY;
%}
%enddef
