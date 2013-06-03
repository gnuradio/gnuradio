/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2013 Free Software Foundation, Inc.
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


/*
 * N.B., this is a _very_ non-standard SWIG .i file
 *
 * It contains a bunch of magic that is required to ensure that when
 * these classes are used as base classes for python code,
 * everything works when calling back from C++ into Python.
 *
 * The gist of the problem is that our C++ code is usually not holding
 * the Python Global Interpreter Lock (GIL).  Thus if we invoke a
 * "director" method from C++, we'll end up in Python not holding the
 * GIL.  Disaster (SIGSEGV) will result.  To avoid this we insert a
 * "shim" that grabs and releases the GIL.
 *
 * If you don't understand SWIG "directors" or the Python GIL,
 * don't bother trying to understand what's going on in here.
 *
 * [We could eliminate a bunch of this hair by requiring SWIG 1.3.29
 * or later and some additional magic declarations, but many systems
 * aren't shipping that version yet.  Thus we kludge...]
 */

// Directors are only supported in Python, Java and C#
#ifdef SWIGPYTHON

%import "pmt_swig.i"

 // Enable SWIG directors for these classes
%feature("director") gr::py_feval_dd;
%feature("director") gr::py_feval_cc;
%feature("director") gr::py_feval_ll;
%feature("director") gr::py_feval;
%feature("director") gr::py_feval_p;

%feature("nodirector") gr::py_feval_dd::calleval;
%feature("nodirector") gr::py_feval_cc::calleval;
%feature("nodirector") gr::py_feval_ll::calleval;
%feature("nodirector") gr::py_feval::calleval;
%feature("nodirector") gr::py_feval_p::calleval;

//%exception {
//  try { $action }
//  catch (Swig::DirectorException &e) { std::cerr << e.getMessage();  SWIG_fail; }
//}

%ignore gr::feval_dd;
%ignore gr::feval_cc;
%ignore gr::feval_ll;
%ignore gr::feval;
%ignore gr::feval_p;

%include <gnuradio/feval.h>

/*
 * These are the ones to derive from in Python.  They have the magic shim
 * that ensures that we're holding the Python GIL when we enter Python land...
 */
namespace gr {
  %rename(feval_dd) py_feval_dd;
  %rename(feval_cc) py_feval_cc;
  %rename(feval_ll) py_feval_ll;
  %rename(feval)    py_feval;
  %rename(feval_p)  py_feval_p;
}

%include <gnuradio/py_feval.h>

namespace gr {
  // examples / test cases
  %rename(feval_dd_example) gr::feval_dd_example;
  %rename(feval_cc_example) gr::feval_cc_example;
  %rename(feval_ll_example) gr::feval_ll_example;
  %rename(feval_example) gr::feval_example;
}

#endif // SWIGPYTHON
