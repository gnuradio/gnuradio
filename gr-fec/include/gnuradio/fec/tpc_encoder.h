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

    unsigned int d_krow;
    unsigned int d_kcol;
    
    std::vector<int> d_rowpolys;
    std::vector<int> d_colpolys;
    
    int d_bval;
    int d_qval;
        
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
    int rowEncoder_n;
    int rowEncoder_m;
    int colEncoder_K;
    int colEncoder_n;
    int colEncoder_m;
    int outputSize;
    int inputSize;
    
    // memory allocated for processing
    int inputSizeWithPad;
    std::vector<unsigned char> inputWithPad;
    
    std::vector< std::vector<float> > rowEncodedBits;
    std::vector<unsigned char> rowToEncode;
    int numRowsToEncode;
    std::vector<float> rowEncoded_block;
    
    std::vector< std::vector<float> > colEncodedBits;
    std::vector<unsigned char> colToEncode;
    int numColsToEncode;
    std::vector<float> colEncoded_block;
    
    void block_conv_encode( std::vector<float> &output,
                               std::vector<unsigned char> input, 
                               std::vector< std::vector<int> > transOutputVec,
                               std::vector< std::vector<int> > transNextStateVec,
                               std::vector<int> tail,
                               int KK, 
                               int nn);

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
