#ifndef INCLUDED_LDPC_ENCODER_H
#define INCLUDED_LDPC_ENCODER_H

#include <map>
#include <string>
#include <gnuradio/fec/encoder.h>
#include <vector>

#include <gnuradio/fec/cldpc.h>
#include <gnuradio/fec/alist.h>

namespace gr {
 namespace fec {

FEC_API generic_encoder::sptr
ldpc_make_encoder (std::string alist_file);


class FEC_API ldpc_encoder : public generic_encoder {
    //befriend the global, swigged make function
    friend generic_encoder::sptr
    ldpc_make_encoder (std::string alist_file);

    //private constructor
    ldpc_encoder (std::string alist_file);

    //plug into the generic fec api
    void generic_work(void *inBuffer, void *outbuffer);
    int get_output_size();
    int get_input_size();

    // memory allocated for processing
    int outputSize;
    int inputSize;
    alist d_list;
    cldpc d_code;   

    double rate() { return 1.0; }
    bool set_frame_size(unsigned int frame_size) { throw std::runtime_error("Not supported!!!"); }

 public:
    ~ldpc_encoder ();

};

}
}

#endif /* INCLUDED_LDPC_ENCODER_H */
