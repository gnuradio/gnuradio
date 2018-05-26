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

#ifndef INCLUDED_TPC_ENCODER_H
#define INCLUDED_TPC_ENCODER_H

#include <map>
#include <string>
#include <gnuradio/fec/encoder.h>
#include <gnuradio/fec/generic_encoder.h>
#include <vector>


namespace gr { 
namespace fec { 

class FEC_API tpc_encoder : public generic_encoder {

    //private constructor
    tpc_encoder (std::vector<int> row_polys, std::vector<int> col_polys, int krow, int kcol, int bval, int qval);

    //plug into the generic fec api
    void generic_work(void *inBuffer, void *outbuffer);
    int get_output_size();
    int get_input_size();

    std::vector<int> d_rowpolys;
    std::vector<int> d_colpolys;
    
    unsigned int d_krow;
    unsigned int d_kcol;
    
    unsigned int d_bval;
    unsigned int d_qval;
        
    // store the state transitions & outputs
    int rowNumStates;
    std::vector< std::vector<int> > rowOutputs;
    std::vector< std::vector<int> > rowNextStates;
    int colNumStates;
    std::vector< std::vector<int> > colOutputs;
    std::vector< std::vector<int> > colNextStates;
    
    std::vector<int> rowTail;
    std::vector<int> colTail;
    
    int rowEncoder_K;
    size_t rowEncoder_n;
    int rowEncoder_m;
    int colEncoder_K;
    size_t colEncoder_n;
    int colEncoder_m;
    int outputSize;
    int inputSize;
    
    // memory allocated for processing
    int inputSizeWithPad;
    std::vector<unsigned char> inputWithPad;
    
    std::vector< std::vector<uint8_t> > rowEncodedBits;
    std::vector<unsigned char> rowToEncode;
    size_t numRowsToEncode;
    std::vector<uint8_t> rowEncoded_block;
    
    std::vector< std::vector<uint8_t> > colEncodedBits;
    std::vector<unsigned char> colToEncode;
    int numColsToEncode;
    std::vector<uint8_t> colEncoded_block;
    
    void block_conv_encode( std::vector<uint8_t> &output,
                               std::vector<uint8_t> input, 
                               std::vector< std::vector<int> > transOutputVec,
                               std::vector< std::vector<int> > transNextStateVec,
                               std::vector<int> tail,
                               size_t KK, 
                               size_t nn);

    FILE *fp;
    
 public:
    ~tpc_encoder ();
    static generic_encoder::sptr make(std::vector<int> row_poly, std::vector<int> col_poly, int krow, int kcol, int bval, int qval);
    double rate() { return (1.0*get_input_size() / get_output_size()); }
    bool set_frame_size( unsigned int ){ return false; }

};


}
}

#endif /* INCLUDED_TPC_ENCODER_H */
