/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
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

#ifndef GR_EXTRAS_I
#define GR_EXTRAS_I

////////////////////////////////////////////////////////////////////////
// Language independent exception handler
////////////////////////////////////////////////////////////////////////
%include exception.i

%exception {
    try {
        $action
    }
    catch(std::exception &e) {
        SWIG_exception(SWIG_RuntimeError, e.what());
    }
    catch(...) {
        SWIG_exception(SWIG_RuntimeError, "Unknown exception");
    }

}

////////////////////////////////////////////////////////////////////////
// Wrapper for python calls that may block
////////////////////////////////////////////////////////////////////////

/*!
 * Use GR_PYTHON_BLOCKING_CODE when calling code that blocks.
 *
 * The try/catch is to save us from boost::thread::interrupt:
 * If a thread from the scheduler (or any other boost thread)
 * is blocking the routine and throws an interrupt exception.
 */
%{

#define GR_PYTHON_BLOCKING_CODE(code) {                                 \
    PyThreadState *_save;                                               \
    _save = PyEval_SaveThread();                                        \
    try{code}                                                           \
    catch(...){PyEval_RestoreThread(_save); throw;}                     \
    PyEval_RestoreThread(_save);                                        \
}

%}

#endif /*GR_EXTRAS_I*/
