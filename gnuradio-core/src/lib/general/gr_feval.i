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

// Enable SWIG directors for these classes
%feature("director") gr_py_feval_dd;
%feature("director") gr_py_feval_cc;
%feature("director") gr_py_feval_ll;
%feature("director") gr_py_feval;

%feature("nodirector") gr_py_feval_dd::calleval;
%feature("nodirector") gr_py_feval_cc::calleval;
%feature("nodirector") gr_py_feval_ll::calleval;
%feature("nodirector") gr_py_feval::calleval;


%rename(feval_dd) gr_py_feval_dd;
%rename(feval_cc) gr_py_feval_cc;
%rename(feval_ll) gr_py_feval_ll;
%rename(feval)    gr_py_feval;

//%exception {
//  try { $action }
//  catch (Swig::DirectorException &e) { std::cerr << e.getMessage();  SWIG_fail; }
//}

%{

// class that ensures we acquire and release the Python GIL

class ensure_py_gil_state {
  PyGILState_STATE	d_gstate;
public:
  ensure_py_gil_state()  { d_gstate = PyGILState_Ensure(); }
  ~ensure_py_gil_state() { PyGILState_Release(d_gstate); }
};

%}

/*
 * These are the real C++ base classes, however we don't want these exposed.
 */
%ignore gr_feval_dd;
class gr_feval_dd
{
protected:
  virtual double eval(double x);

public:
  gr_feval_dd() {}
  virtual ~gr_feval_dd();

  virtual double calleval(double x);
};

%ignore gr_feval_cc;
class gr_feval_cc
{
protected:
  virtual gr_complex eval(gr_complex x);

public:
  gr_feval_cc() {}
  virtual ~gr_feval_cc();

  virtual gr_complex calleval(gr_complex x);
};

%ignore gr_feval_ll;
class gr_feval_ll
{
protected:
  virtual long eval(long x);
  
public:
  gr_feval_ll() {}
  virtual ~gr_feval_ll();

  virtual long calleval(long x);
};

%ignore gr_feval;
class gr_feval
{
protected:
  virtual void eval();
  
public:
  gr_feval() {}
  virtual ~gr_feval();

  virtual void calleval();
};

/*
 * These are the ones to derive from in Python.  They have the magic shim
 * that ensures that we're holding the Python GIL when we enter Python land...
 */

%inline %{

class gr_py_feval_dd : public gr_feval_dd
{
 public:
  double calleval(double x)
  {
    ensure_py_gil_state _lock;
    return eval(x);
  }
};

class gr_py_feval_cc : public gr_feval_cc
{
 public:
  gr_complex calleval(gr_complex x)
  {
    ensure_py_gil_state _lock;
    return eval(x);
  }
};

class gr_py_feval_ll : public gr_feval_ll
{
 public:
  long calleval(long x)
  {
    ensure_py_gil_state _lock;
    return eval(x);
  }
};

class gr_py_feval : public gr_feval
{
 public:
  void calleval()
  {
    ensure_py_gil_state _lock;
    eval();
  }
};

%}



// examples / test cases

%rename(feval_dd_example) gr_feval_dd_example;
double gr_feval_dd_example(gr_feval_dd *f, double x);

%rename(feval_cc_example) gr_feval_cc_example;
gr_complex gr_feval_cc_example(gr_feval_cc *f, gr_complex x);

%rename(feval_ll_example) gr_feval_ll_example;
long gr_feval_ll_example(gr_feval_ll *f, long x);

%rename(feval_example) gr_feval_example;
void gr_feval_example(gr_feval *f);

#endif // SWIGPYTHON
