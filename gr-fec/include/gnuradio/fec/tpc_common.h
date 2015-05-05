/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
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

#ifndef INCLUDED_TPC_COMMON_H
#define INCLUDED_TPC_COMMON_H

#include <vector>

namespace gr {
namespace fec { 

class tpc_common {
public:
    static int parity_counter( int symbol, int length );
    static void rsc_enc_bit(int input, int state_in, std::vector<int> g, int KK, int nn, 
                    std::vector< std::vector<int> > &output, std::vector< std::vector<int> > &nextStates);
    static void precomputeStateTransitionMatrix_RSCPoly(
        int numStates,
        std::vector<int> g, 
        int KK, 
        int nn, 
        std::vector< std::vector<int> > &output, 
        std::vector< std::vector<int> > &nextStates);
    static void rsc_tail( std::vector<int> &tail_p, std::vector<int> g, int max_states, int mm );
    static void itob(std::vector<int> &binVec, int symbol, int length);
};

}
}

#endif  /* INCLUDED_TPC_COMMON_H */
