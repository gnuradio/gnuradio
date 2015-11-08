/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_H
#define INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_H

#include <gnuradio/analog/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace analog {

    /*!
     * \brief quadrature demodulator: complex in, float out
     * \ingroup modulators_blk
     *
     * \details
     * This can be used to demod FM, FSK, GMSK, etc.  The input is complex
     * baseband, output is the signal frequency in relation to the sample
     * rated, multiplied with the gain.
     *
     * Mathematically, this block calculates the product of the one-sample
     * delayed input and the conjugate undelayed signal, and then calculates
     * the argument of the resulting complex number:
     *
     * \f$y[n] = \mathrm{arg}\left(x[n] \, \bar x [n-1]\right)\f$.
     * 
     * Let \f$x\f$ be a complex sinusoid with amplitude \f$A>0\f$, (absolute)
     * frequency \f$f\in\mathbb R\f$ and phase \f$\phi_0\in[0;2\pi]\f$ sampled at
     * \f$f_s>0\f$ so, without loss of generality,
     *
     * \f$x[n]= A e^{j2\pi( \frac f{f_s} n + \phi_0)}\f$
     *
     * then
     *
     * \f{align*}{ y[n] &= \mathrm{arg}\left(A e^{j2\pi\left( \frac f{f_s} n + \phi_0\right)} \overline{A e^{j2\pi( \frac f{f_s} (n-1) + \phi_0)}}\right)\\
     *  & = \mathrm{arg}\left(A^2 e^{j2\pi\left( \frac f{f_s} n + \phi_0\right)} e^{-j2\pi( \frac f{f_s} (n-1) + \phi_0)}\right)\\
     *  & = \mathrm{arg}\left( A^2 e^{j2\pi\left( \frac f{f_s} n + \phi_0 - \frac f{f_s} (n-1) - \phi_0\right)}\right)\\
     *  & = \mathrm{arg}\left( A^2 e^{j2\pi\left( \frac f{f_s} n - \frac f{f_s} (n-1)\right)}\right)\\
     *  & = \mathrm{arg}\left( A^2 e^{j2\pi\left( \frac f{f_s} \left(n-(n-1)\right)\right)}\right)\\
     *  & = \mathrm{arg}\left( A^2 e^{j2\pi \frac f{f_s}}\right) \intertext{$A$ is real, so is $A^2$ and hence only \textit{scales}, therefore $\mathrm{arg}(\cdot)$ is invariant:} &= \mathrm{arg}\left(e^{j2\pi \frac f{f_s}}\right)\\
     *  &= \frac f{f_s}\\
     *  &&\blacksquare 
     * \f}
     */
    class ANALOG_API quadrature_demod_cf : virtual public sync_block
    {
    public:
      // gr::analog::quadrature_demod_cf::sptr
      typedef boost::shared_ptr<quadrature_demod_cf> sptr;

      /* \brief Make a quadrature demodulator block.
       *
       * \param gain Gain setting to adjust the output amplitude. Set
       *             based on converting the phase difference between
       *             samples to a nominal output value.
       */
      static sptr make(float gain);

      virtual void set_gain(float gain) = 0;
      virtual float gain() const = 0;
    };

  } /* namespace analog */
} /* namespace gr */

#endif /* INCLUDED_ANALOG_QUADRATURE_DEMOD_CF_H */
