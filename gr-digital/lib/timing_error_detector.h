/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
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
 * along with this file; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_DIGITAL_TIMING_ERROR_DETECTOR_H
#define INCLUDED_DIGITAL_TIMING_ERROR_DETECTOR_H

#include <gnuradio/gr_complex.h>
#include <gnuradio/digital/timing_error_detector_type.h>
#include <gnuradio/digital/constellation.h>
#include <deque>

namespace gr {
  namespace digital {

    /*!
     * \brief Base class for the composite symbol clock timing error
     * detector object used by the symbol_synchronizer_xx blocks.
     * \ingroup internal
     *
     * \details
     * This is the base class for the symbol clock timing error detector
     * object used by the symbol_synchronizer_xx blocks to provide a
     * user selectable timing error detector type.
     *
     * This base class provides the enumeration type for the available
     * types of timing error detectors.
     *
     * This base class also provides most of the methods update, manage, and
     * store the data points required to compute the symbol clock timing error,
     * and the timing error result itself.
     *
     * The derived classes only have to provide the simple functions
     * to actually compute the timing error.
     */
    class timing_error_detector
    {
      public:

        /*!
         * \brief Create a timing error detector object of the specified
         * type
         * \param type The type/algorithm to use for the timing error detector
         * \param constellation A constellation to be used as a decision slicer
         *                      for decision directed timing error detector
         *                      algorithms
         */
        static timing_error_detector *make(enum ted_type type,
                                           constellation_sptr constellation =
                                                          constellation_sptr());

        virtual ~timing_error_detector() {};

        /*!
         * \brief Return the number of input samples per symbol this timing
         * error detector algorithm requires.
         */
        virtual int inputs_per_symbol() { return d_inputs_per_symbol; }

        /*!
         * \brief Provide a complex input sample to the TED algorithm
         * \param x the input sample
         * \param dx the derivative at the input sample, if required by the
         *           timinig error detector algorithm
         */
        virtual void input(const gr_complex &x,
                           const gr_complex &dx = gr_complex(0.0f, 0.0f));

        /*!
         * \brief Provide a float input sample to the TED algorithm
         * \param x the input sample
         * \param dx the derivative at the input sample, if required by the
         *           timinig error detector algorithm
         */
        virtual void input(float x,
                           float dx = 0.0f);

        /*!
         * \brief Return if this timing error detector algorithm requires a
         * look-ahead input sample to be input to produce a new error estimate
         * for the symbol sampling instant.
         */
        virtual bool needs_lookahead() { return d_needs_lookahead; }

        /*!
         * \brief Provide a complex input lookahead sample to the TED algorithm
         * \param x the input lookahead sample
         * \param dx the derivative at the input lookahead sample, if required
         *           by the timinig error detector algorithm
         */
        virtual void input_lookahead(const gr_complex &x,
                                     const gr_complex &dx =
                                                        gr_complex(0.0f, 0.0f));

        /*!
         * \brief Provide a float input lookahead sample to the TED algorithm
         * \param x the input lookahead sample
         * \param dx the derivative at the input lookahead sample, if required
         *           by the timinig error detector algorithm
         */
        virtual void input_lookahead(float x,
                                     float dx = 0.0f);

        /*!
         * \brief Return if this timing error detector algorithm requires
         * derivative input samples in addition to normal input samples.
         */
        virtual bool needs_derivative() { return d_needs_derivative; }

        /*!
         * \brief Return the current symbol timing error estimate
         */
        virtual float error() { return d_error; }

        /*!
         * \brief Revert the timing error detector processing state back one
         * step.
         * \param preserve_error If true, don't revert the error estimate.
         */
        virtual void revert(bool preserve_error = false);

        /*!
         * \brief Reset the timing error detector
         */
        virtual void sync_reset();

      private:
        enum ted_type d_type;

      protected:
        /*!
         * \brief Create a timing error detector abstract base class object
         * object
         * \param type The type/algorithm to use for the timing error detector
         * \param inputs_per_symbol The input samples per symbol this TED
         *                          algorithm requires
         * \param error_computation_depth The number of input samples this TED
         *                                algorithm needs to compute the error
         * \param needs_lookahead True if this TED algorithm needs a lookahead
         *                        input sample to compute the error for the
         *                        symbol sampling instant
         * \param needs_derivative True if this TED algorithm needs a derivative
         *                         input sample in addition to the normal input
         *                         sample
         * \param constellation A constellation to be used as a decision slicer
         *                      for decision directed timing error detector
         *                      algorithms
         */
        timing_error_detector(enum ted_type type,
                              int inputs_per_symbol,
                              int error_computation_depth,
                              bool needs_lookahead = false,
                              bool needs_derivative = false,
                              constellation_sptr constellation =
                                                          constellation_sptr());

        /*!
         * \brief Advance the TED input clock, so the input() methods will
         * compute the TED error term at the proper symbol sampling instant.
         */
        void advance_input_clock() {
            d_input_clock = (d_input_clock + 1) % d_inputs_per_symbol;
        }

        /*!
         * \brief Revert the TED input clock one step
         */
        void revert_input_clock()
        {
            if (d_input_clock == 0)
                d_input_clock = d_inputs_per_symbol - 1;
            else
                d_input_clock--;
        }

        /*!
         * \brief Reset the TED input clock, so the next input clock advance
         * corresponds to a symbol sampling instant.
         */
        void sync_reset_input_clock() {
            d_input_clock = d_inputs_per_symbol - 1;
        }

        /*!
         * \brief Convert the soft symbol sample into a hard symbol decision.
         * \param x the soft input symbol sample
         */
        gr_complex slice(const gr_complex &x);

        /*!
         * \brief Compute the TED error term for complex input samples
         */
        virtual float compute_error_cf() = 0;

        /*!
         * \brief Compute the TED error term for float input samples
         */
        virtual float compute_error_ff() = 0;

        constellation_sptr d_constellation;
        float d_error;
        float d_prev_error;
        int d_inputs_per_symbol;
        int d_input_clock;
        int d_error_depth;
        std::deque<gr_complex> d_input;
        std::deque<gr_complex> d_decision;
        std::deque<gr_complex> d_input_derivative;
        bool d_needs_lookahead;
        bool d_needs_derivative;
    };

    /*!
     * \brief Mueller and Müller (M&M) timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Mueller and Müller (M&M) timing error detector algorithm
     * class.  It is a decision directed timing error detector, and requires
     * an input slicer constellation to make decisions.
     *
     * See equation (49) of:
     * Mueller, Kurt H., Müller, Markus, "Timing Recovery in Digital
     * Synchronous Data Receivers", _IEEE_Transactions_on_Communications_,
     * Vol. COM-24, No. 5, May 1976, pp. 516-531
     */
    class ted_mueller_and_muller : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Mueller and Müller (M&M) timing error detector
         * \param constellation A constellation to be used as a decision slicer
         */
        ted_mueller_and_muller(constellation_sptr constellation)
          : timing_error_detector(TED_MUELLER_AND_MULLER,
                                  1, 2, false, false, constellation)
        {}
        ~ted_mueller_and_muller() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Modified Mueller and Müller (M&M) timing error detector algorithm
     * class
     * \ingroup internal
     *
     * \details
     * This is the modified Mueller and Müller (M&M) timing error detector
     * algorithm class.  It is a decision directed timing error detector, and
     * requires an input slicer constellation to make decisions.
     *
     * The modification of the standard M&M TED is done to remove
     * self-noise that is present in the standard M&M TED.
     *
     * See:
     * G. R. Danesfahani, T.G. Jeans, "Optimisation of modified Mueller
     * and Muller algorithm," Electronics Letters, Vol. 31, no. 13, 22
     * June 1995, pp. 1032 - 1033.
     */
    class ted_mod_mueller_and_muller : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a modified Mueller and Müller (M&M) timing error
         * detector
         * \param constellation A constellation to be used as a decision slicer
         */
        ted_mod_mueller_and_muller(constellation_sptr constellation)
          : timing_error_detector(TED_MOD_MUELLER_AND_MULLER,
                                  1, 3, false, false, constellation)
        {}
        ~ted_mod_mueller_and_muller() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Zero Crossing timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Zero Crossing timing error detector algorithm class.
     * It is a decision directed timing error detector, and
     * requires an input slicer constellation to make decisions.
     */
    class ted_zero_crossing : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Zero Crossing timing error detector
         * \param constellation A constellation to be used as a decision slicer
         */
        ted_zero_crossing(constellation_sptr constellation)
          : timing_error_detector(TED_ZERO_CROSSING,
                                  2, 3, false, false, constellation)
        {}
        ~ted_zero_crossing() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Gardner timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Gardner timing error detector algorithm class.
     *
     * See:
     * F.M. Gardner, “A BPSK/QPSK Timing Error Detector for Sampled Receivers”, 
     * IEEE Trans., COM-34, (5), 1988, pp. 423 – 429.
     */
    class ted_gardner : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Gardner timing error detector
         */
        ted_gardner()
          : timing_error_detector(TED_GARDNER,
                                  2, 3, false, false,
                                  constellation_sptr())
        {}
        ~ted_gardner() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Early-Late timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Early-Late timing error detector algorithm class.
     * It requires a lookahead sample to compute the symbol timing error
     * at the symbol sampling instant.
     */
    class ted_early_late : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Early-Late timing error detector
         */
        ted_early_late()
          : timing_error_detector(TED_EARLY_LATE,
                                  2, 2, true, false,
                                  constellation_sptr())
        {}
        ~ted_early_late() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Generalized MSK timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Generalized MSK timing error detector algorithm class.
     * It operates on the CPM MSK signal directly and not the FM pulses.
     *
     * See:
     * A.N. D'Andrea, U. Mengali, R. Reggiannini, "A digital approach to clock
     * recovery in generalized minimum shift keying", IEEE Transactions on
     * Vehicular Technology, Vol. 39, Issue 3, August 1990, pp. 227-234
     */
    class ted_generalized_msk : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Generalized MSK timing error detector
         */
        ted_generalized_msk()
          : timing_error_detector(TED_DANDREA_AND_MENGALI_GEN_MSK,
                                  2, 4, false, false, constellation_sptr())
        {}
        ~ted_generalized_msk() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Gaussian MSK timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Gaussian MSK timing error detector algorithm class.
     * It operates on the CPM MSK signal directly and not the FM pulses.
     *
     * See:
     * U. Mengali, A.N. D'Andrea, _Synchronization_Techniques_for_Digital_
     * Receviers_, New York, Plenum Press 1997
     */
    class ted_gaussian_msk : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Generalized MSK timing error detector
         */
        ted_gaussian_msk()
          : timing_error_detector(TED_MENGALI_AND_DANDREA_GMSK,
                                  2, 4, false, false, constellation_sptr())
        {}
        ~ted_gaussian_msk() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Signal times Slope Maximum Likelihood solution approximation
     * timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Signal times Slope Maximum Likelihood solution approximation
     * timing error detector algorithm class.  This approximation is good for
     * small signals (< 1.0) and/or situations with low SNR.
     *
     * See equation (5) and the 2 following paragraphs of:
     * Fredric J. Harris, Michael Rice, "Multirate Digital Filters for
     * Symbol Timing Synchronization in Software Defined Radios",
     * _IEEE_Journal_on_Selected_Areas_in_Communications_, Vol. 19, No. 12,
     * December 2001, pp. 2346 - 2357
     */
    class ted_signal_times_slope_ml : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Signal times Slope Maximum Likelihood solution
         * approximation timing error detector
         */
        ted_signal_times_slope_ml()
          : timing_error_detector(TED_SIGNAL_TIMES_SLOPE_ML,
                                  1, 1, false, true, constellation_sptr())
        {}
        ~ted_signal_times_slope_ml() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

    /*!
     * \brief Signum times Slope Maximum Likelihood solution approximation
     * timing error detector algorithm class
     * \ingroup internal
     *
     * \details
     * This is the Signum times Slope Maximum Likelihood solution approximation
     * timing error detector algorithm class.  This approximation is good for
     * large signals (> 1.0) and/or situations with high SNR.
     *
     * See equation (5) and the 2 following paragraphs of:
     * Fredric J. Harris, Michael Rice, "Multirate Digital Filters for
     * Symbol Timing Synchronization in Software Defined Radios",
     * _IEEE_Journal_on_Selected_Areas_in_Communications_, Vol. 19, No. 12,
     * December 2001, pp. 2346 - 2357
     */
    class ted_signum_times_slope_ml : public timing_error_detector
    {
      public:
        /*!
         * \brief Create a Signum times Slope Maximum Likelihood solution
         * approximation timing error detector
         */
        ted_signum_times_slope_ml()
          : timing_error_detector(TED_SIGNUM_TIMES_SLOPE_ML,
                                  1, 1, false, true, constellation_sptr())
        {}
        ~ted_signum_times_slope_ml() {};

      private:
        float compute_error_cf();
        float compute_error_ff();
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_TIMING_ERROR_DETECTOR_H */
