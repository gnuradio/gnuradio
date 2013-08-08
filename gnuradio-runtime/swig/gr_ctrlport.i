/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifdef GR_CTRLPORT

typedef uint32_t DisplayType;

// DisplayType Plotting types
const uint32_t DISPNULL = 0x0000;
const uint32_t DISPTIME = 0x0001;
const uint32_t DISPXY   = 0x0002;
const uint32_t DISPPSD  = 0x0004;
const uint32_t DISPSPEC = 0x0008;
const uint32_t DISPRAST = 0x0010;

// DisplayType Options
const uint32_t DISPOPTCPLX    = 0x0100;
const uint32_t DISPOPTLOG     = 0x0200;
const uint32_t DISPOPTSTEM    = 0x0400;
const uint32_t DISPOPTSTRIP   = 0x0800;
const uint32_t DISPOPTSCATTER = 0x1000;

enum priv_lvl_t {
  RPC_PRIVLVL_ALL = 0,
  RPC_PRIVLVL_MIN = 9,
  RPC_PRIVLVL_NONE = 10
};

enum KnobType {
  KNOBBOOL,       KNOBCHAR,       KNOBINT,        KNOBFLOAT,
  KNOBDOUBLE,     KNOBSTRING,     KNOBLONG,       KNOBVECBOOL,
  KNOBCOMPLEX,    KNOBCOMPLEXD,
  KNOBVECCHAR,    KNOBVECINT,     KNOBVECFLOAT,   KNOBVECDOUBLE,
  KNOBVECSTRING,  KNOBVECLONG
};

%{
#include <gnuradio/rpcserver_booter_base.h>
#include <gnuradio/rpcserver_booter_aggregator.h>
#include <gnuradio/pycallback_object.h>
%}

%include <gnuradio/rpcserver_booter_base.h>
%include <gnuradio/rpcserver_booter_aggregator.h>
%include <gnuradio/pycallback_object.h>

// Declare this class here but without the nested templated class
// inside (replaces include of rpcmanager.h)
class GR_RUNTIME_API rpcmanager : public virtual rpcmanager_base
{
 public:
  rpcmanager();
  ~rpcmanager();

  static rpcserver_booter_base* get();

  static void register_booter(rpcserver_booter_base* booter);
};


// Attach a new python callback method to Python function
%extend pycallback_object {
  // Set a Python function object as a callback function
  // Note : PyObject *pyfunc is remapped with a typempap
  void activate(PyObject *pyfunc)
  {
    self->set_callback(pyfunc);
    Py_INCREF(pyfunc);
  }
}

%template(RPC_get_string)   pycallback_object<std::string>;
%template(RPC_get_int)      pycallback_object<int>;
%template(RPC_get_float)    pycallback_object<float>;
%template(RPC_get_double)   pycallback_object<double>;
%template(RPC_get_complex)  pycallback_object<gr_complex>;
%template(RPC_get_complexd)  pycallback_object<gr_complexd>;
%template(RPC_get_vector_float)    pycallback_object<std::vector<float> >;
%template(RPC_get_vector_gr_complex)    pycallback_object<std::vector<gr_complex> >;

#endif /* GR_CTRLPORT */
