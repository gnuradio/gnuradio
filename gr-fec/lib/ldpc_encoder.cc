#include <gnuradio/fec/ldpc_encoder.h>
//#include <debug_macros.h>

#include <math.h>
#include <boost/assign/list_of.hpp>
//#include <volk_typedefs.h>
#include <volk/volk.h>
#include <sstream>
#include <stdio.h>
#include <vector>
//#include <cc_common.h>

#include <algorithm>            // for std::reverse
#include <string.h>             // for memcpy

namespace gr {
  namespace fec {

generic_encoder::sptr
ldpc_make_encoder(std::string alist_file)
{
    return generic_encoder::sptr(new ldpc_encoder(alist_file));
}

ldpc_encoder::ldpc_encoder (std::string alist_file)
{
    d_list.read(alist_file.c_str());
    d_code.set_alist(d_list);
    inputSize = d_code.dimension();
    outputSize = d_code.get_N();
    printf("ENCODER: inputSize = %d, outputSize = %d\n",inputSize, outputSize);
}

int ldpc_encoder::get_output_size() {
    return outputSize;
}

int ldpc_encoder::get_input_size() {
    return inputSize;
}

void ldpc_encoder::generic_work(void *inBuffer, void *outBuffer) {
    const unsigned char *in = (const unsigned char *) inBuffer;
    float *out = (float *) outBuffer;
    std::vector<char> inbuf(inputSize);
    memcpy(&inbuf[0], in, inputSize);
    std::vector<char> coded(d_code.encode(inbuf));
    for(size_t i=0; i<coded.size();i++){ out[i] = coded[i]; }
}


ldpc_encoder::~ldpc_encoder()
{
}

}
}
