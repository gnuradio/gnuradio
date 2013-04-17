/* -*- c++ -*- */
/*
 * Copyright 2006,2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FEVAL_H
#define INCLUDED_GR_FEVAL_H

#include <gnuradio/api.h>
#include <gnuradio/gr_complex.h>
#include <pmt/pmt.h>

namespace gr {

  /*!
   * \brief base class for evaluating a function: double -> double
   * \ingroup misc
   *
   * This class is designed to be subclassed in Python or C++ and is
   * callable from both places. It uses SWIG's "director" feature to
   * implement the magic.
   *
   * It's slow. Don't use it in a performance critical path.
   *
   * Override eval to define the behavior.
   * Use calleval to invoke eval (this kludge is required to allow a
   * python specific "shim" to be inserted.
   */
  class GR_RUNTIME_API feval_dd
  {
  protected:
    /*!
     * \brief override this to define the function
     */
    virtual double eval(double x);

  public:
    feval_dd() {}
    virtual ~feval_dd();

    virtual double calleval(double x);	// invoke "eval"
  };

  /*!
   * \brief base class for evaluating a function: complex -> complex
   * \ingroup misc
   *
   * This class is designed to be subclassed in Python or C++ and is
   * callable from both places. It uses SWIG's "director" feature to
   * implement the magic.
   *
   * It's slow. Don't use it in a performance critical path.
   *
   * Override eval to define the behavior.
   * Use calleval to invoke eval (this kludge is required to allow a
   * python specific "shim" to be inserted.
   */
  class GR_RUNTIME_API feval_cc
  {
  protected:
    /*!
     * \brief override this to define the function
     */
    virtual gr_complex eval(gr_complex x);

  public:
    feval_cc() {}
    virtual ~feval_cc();

    virtual gr_complex calleval(gr_complex x);	// invoke "eval"
  };

  /*!
   * \brief base class for evaluating a function: long -> long
   * \ingroup misc
   *
   * This class is designed to be subclassed in Python or C++ and is
   * callable from both places. It uses SWIG's "director" feature to
   * implement the magic.
   *
   * It's slow. Don't use it in a performance critical path.
   *
   * Override eval to define the behavior.
   * Use calleval to invoke eval (this kludge is required to allow a
   * python specific "shim" to be inserted.
   */
  class GR_RUNTIME_API feval_ll
  {
  protected:
    /*!
     * \brief override this to define the function
     */
    virtual long eval(long x);

  public:
    feval_ll() {}
    virtual ~feval_ll();

    virtual long calleval(long x);	// invoke "eval"
  };

  /*!
   * \brief base class for evaluating a function: void -> void
   * \ingroup misc
   *
   * This class is designed to be subclassed in Python or C++ and is
   * callable from both places. It uses SWIG's "director" feature to
   * implement the magic.
   *
   * It's slow. Don't use it in a performance critical path.
   *
   * Override eval to define the behavior.
   * Use calleval to invoke eval (this kludge is required to allow a
   * python specific "shim" to be inserted.
   */
  class GR_RUNTIME_API feval
  {
  protected:
    /*!
     * \brief override this to define the function
     */
    virtual void eval();

  public:
    feval() {}
    virtual ~feval();

    virtual void calleval();	// invoke "eval"
  };

  /*!
   * \brief base class for evaluating a function: pmt -> void
   * \ingroup misc
   *
   * This class is designed to be subclassed in Python or C++ and is
   * callable from both places. It uses SWIG's "director" feature to
   * implement the magic.
   *
   * It's slow. Don't use it in a performance critical path.
   *
   * Override eval to define the behavior.
   * Use calleval to invoke eval (this kludge is required to allow a
   * python specific "shim" to be inserted.
   */
  class GR_RUNTIME_API feval_p
  {
  protected:
    /*!
     * \brief override this to define the function
     */
    virtual void eval(pmt::pmt_t x);

  public:
    feval_p() {}
    virtual ~feval_p();

    virtual void calleval(pmt::pmt_t x);	// invoke "eval"
  };

  /*!
   * \brief trivial examples / test cases showing C++ calling Python code
   */
  GR_RUNTIME_API double     feval_dd_example(feval_dd *f, double x);
  GR_RUNTIME_API gr_complex feval_cc_example(feval_cc *f, gr_complex x);
  GR_RUNTIME_API long       feval_ll_example(feval_ll *f, long x);
  GR_RUNTIME_API void       feval_example(feval *f);

} /* namespace gr */

#endif /* INCLUDED_GR_FEVAL_H */
