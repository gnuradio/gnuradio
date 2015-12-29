/* -*- c++ -*- */
/*
 * Copyright 2002, 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DTV_ATSC_SINGLE_VITERBI_H
#define INCLUDED_DTV_ATSC_SINGLE_VITERBI_H

namespace gr {
  namespace dtv {

    class atsc_single_viterbi
    {
    public:
      atsc_single_viterbi();

      static const unsigned int TB_LEN = 32;

      /*!
       * \p INPUT ideally takes on the values +/- 1,3,5,7
       * return is decoded dibit in the range [0, 3]
       */
      char decode(float input);

      void reset ();

      //! internal delay of decoder
      static int delay () { return TB_LEN - 1; }

      float best_state_metric() const;

    protected:
      static const int d_transition_table[4][4];
      static const int d_was_sent[4][4];

      float d_path_metrics [2][4];
      unsigned long long d_traceback [2][4];
      unsigned char d_phase;
      int d_post_coder_state;
      float d_best_state_metric;
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_SINGLE_VITERBI_H */
