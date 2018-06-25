/* -*- c++ -*- */
/*
 * Copyright 2002,2011-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TRELLIS_FSM_H
#define INCLUDED_TRELLIS_FSM_H

#include <gnuradio/trellis/api.h>
#include <vector>
#include <iosfwd>

namespace gr {
  namespace trellis {

    /*!
     * \brief  Finite State Machine Specification class.
     * \ingroup trellis_coding_blk
     *
     * \details
     * An instance of this class represents a finite state machine
     * specification (FSMS) rather than the FSM itself.  It particular
     * the state of the FSM is not stored within an instance of this
     * class.
     */
    class TRELLIS_API fsm
    {
    private:
      // Input alphabet cardinality.
      int d_I;

      // Number of states.
      int d_S;

      // Output alphabet cardinality.
      int d_O;

      // NS means Next State.
      // next_state = d_NS[current_state * d_I + input_symbol]
      std::vector<int> d_NS;

      // OS means Output Symbol.
      // output_symbol = d_OS[current_state * d_I + input_symbol]
      std::vector<int> d_OS;

      // PS means Previous State.
      std::vector< std::vector<int> > d_PS;

      // PI means Previous Input Symbol.
      // d_PS[current_state][k] and d_PI[current_state][k], is a pair of the form
      // (previous_state, previous_input_symbol) that could have produced the
      // current state.
      std::vector< std::vector<int> > d_PI;

      // TM means Termination matrix.
      // d_TMl[s*d_S+es] is the shortest number of steps to get from state s to
      // state es.
      std::vector<int> d_TMl;

      // d_TMi[s*d_S+es] is the input symbol required to set off on the shortest
      // path from state s to es.
      std::vector<int> d_TMi;
      void generate_PS_PI ();
      void generate_TM ();
      bool find_es(int es);

    public:
      /*!
       * \brief Constructor to create an uninitialized FSMS.
       */
      fsm();

      /*!
       * \brief Constructor to copy an FSMS.
       */
      fsm(const fsm &FSM);

      /*!
       * \brief Constructor to to create an FSMS.
       *
       * \param I	        The number of possible input symbols.
       * \param S           The number of possible FSM states.
       * \param O           The number of possible output symbols.
       * \param NS          A mapping from (current state, input symbol) to next state.
       *                    next_state = NS[current_state * I + input_symbol]
       * \param OS          A mapping from (current state, input symbol) to output symbol.
       *                    output_symbol = OS[current_state * I + input_symbol]
       *
       */
      fsm(int I, int S, int O, const std::vector<int> &NS, const std::vector<int> &OS);

      /*!
       * \brief Constructor to create an FSMS from file contents.
       *
       * \param name        filename
       *
       */
      fsm(const char *name);

      /*!
       * \brief Creates an FSMS from the generator matrix of a (n, k) binary convolutional code.
       *
       * \param k      ???
       * \param n      ???
       * \param G      ???
       *
       */
      fsm(int k, int n, const std::vector<int> &G);

      /*!
       * \brief Creates an FSMS describing ISI.
       *
       * \param mod_size    modulation size
       * \param ch_length   channel length
       *
       */
      fsm(int mod_size, int ch_length);

      /*!
       * \brief Creates an FSMS describing the trellis for a CPM.
       *
       * \param P    ???? h=K/P (relatively prime)
       * \param M    alphabet size
       * \param L    pulse duration
       *
       * This FSM is based on the paper by B. Rimoldi
       * "A decomposition approach to CPM", IEEE Trans. Info Theory, March 1988
       * See also my own notes at http://www.eecs.umich.edu/~anastas/docs/cpm.pdf
       */
      fsm(int P, int M, int L);

      /*!
       * \brief Creates an FSMS describing the joint trellis of two FSMs.
       *
       * \param FSM1  first FSMS
       * \param FSM2  second FSMS
       */
      fsm(const fsm &FSM1, const fsm &FSM2);
     
       
      /*!
       * \brief Creates an FSMS describing the trellis of two serially concatenated FSMs.
       *
       * \param FSMo  outer FSMS
       * \param FSMi  inner FSMS
       * \param serial set it to true to distinguish from the previous constructor
       */
      fsm(const fsm &FSMo, const fsm &FSMi, bool serial);

      /*!
       * \brief Creates an FSMS representing n stages through the original FSM (AKA radix-n FSM).
       *
       * \param FSM      Original FSMs
       * \param n        Number of stages.
       */
      fsm(const fsm &FSM, int n);
      int I() const { return d_I; }
      int S() const { return d_S; }
      int O() const { return d_O; }
      const std::vector<int> & NS() const { return d_NS; }
      const std::vector<int> & OS() const { return d_OS; }
      const std::vector< std::vector<int> > & PS() const { return d_PS; }
      const std::vector< std::vector<int> > & PI() const { return d_PI; }
      const std::vector<int> & TMi() const { return d_TMi; }
      const std::vector<int> & TMl() const { return d_TMl; }

      /*!
       * \brief Creates an svg image of the trellis representation.
       *
       * \param filename         filename
       * \param number_stages    ????
       */
      void write_trellis_svg(std::string filename ,int number_stages);

      /*!
       * \brief Write the FSMS to a file.
       *
       * \param filename   filename
       */
      void write_fsm_txt(std::string filename);
    };

  } /* namespace trellis */
} /* namespace gr */

#endif /* INCLUDED_TRELLIS_FSM_H */
