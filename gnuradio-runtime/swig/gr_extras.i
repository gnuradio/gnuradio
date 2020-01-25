/*
 * Copyright 2011,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
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
