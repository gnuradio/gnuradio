#include "qa_utils.h"
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <vector>
#include <time.h>
//#include <math.h>
//#include <volk/volk_runtime.h>
#include <volk/volk_registry.h>
#include <volk/volk.h>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits.hpp>
//#include <boost/test/unit_test.hpp>

float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

void
random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = uniform ();
}

template <class t>
t *make_aligned_buffer(unsigned int len) {
  t *buf;
  int ret;
  ret = posix_memalign((void**)&buf, 16, len * sizeof(t));
  assert(ret == 0);
  return buf;
}

void make_buffer_for_signature(std::vector<void *> &buffs, std::vector<std::string> inputsig, unsigned int vlen) {
    BOOST_FOREACH(std::string sig, inputsig) {
        if     (sig=="32fc" || sig=="64f") buffs.push_back((void *) make_aligned_buffer<lv_32fc_t>(vlen));
        else if(sig=="32f" || sig=="32u" || sig=="32s" || sig=="16sc") buffs.push_back((void *) make_aligned_buffer<float>(vlen));
        else if(sig=="16s" || sig=="16u") buffs.push_back((void *) make_aligned_buffer<int16_t>(vlen));
        else if(sig=="8s" || sig=="8u") buffs.push_back((void *) make_aligned_buffer<int8_t>(vlen));
        else std::cout << "Invalid type!" << std::endl;
    }
}

static std::vector<std::string> get_arch_list(const int archs[]) {
    std::vector<std::string> archlist;
    int num_archs = archs[0];
    
    //there has got to be a way to query these arches
    for(int i = 0; i < num_archs; i++) {
        switch(archs[i+1]) {
        case (1<<LV_GENERIC):
            archlist.push_back("generic");
            break;
        case (1<<LV_ORC):
            archlist.push_back("orc");
            break;
        case (1<<LV_SSE):
            archlist.push_back("sse");
            break;
        case (1<<LV_SSE2):
            archlist.push_back("sse2");
            break;
        case (1<<LV_SSSE3):
            archlist.push_back("ssse3");
            break;
        case (1<<LV_SSE4_1):
            archlist.push_back("sse4_1");
            break;
        case (1<<LV_SSE4_2):
            archlist.push_back("sse4_2");
            break;
        case (1<<LV_SSE4_A):
            archlist.push_back("sse4_a");
            break;
        case (1<<LV_MMX):
            archlist.push_back("mmx");
            break;
        case (1<<LV_AVX):
            archlist.push_back("avx");
            break;
        default:
            break;
        }
    }
    return archlist;
}

static bool is_valid_type(std::string type) {
    std::vector<std::string> valid_types = boost::assign::list_of("32fc")("32f")("32s")("32u")("16sc")("16s")("16u")("8s")("8u");
    
    BOOST_FOREACH(std::string this_type, valid_types) {
        if(type == this_type) return true;
    }
    return false;
}
    

static void get_function_signature(std::vector<std::string> &inputsig, 
                                   std::vector<std::string> &outputsig, 
                                   std::string name) {
    boost::char_separator<char> sep("_");
    boost::tokenizer<boost::char_separator<char> > tok(name, sep);
    std::vector<std::string> toked;
    tok.assign(name);
    toked.assign(tok.begin(), tok.end());
    
    assert(toked[0] == "volk");
    
    inputsig.push_back(toked[1]); //mandatory
    int pos = 2;
    bool valid_type = true;
    while(valid_type && pos < toked.size()) {
        if(is_valid_type(toked[pos])) inputsig.push_back(toked[pos]);
        else valid_type = false;
        pos++;
    }
    while(!valid_type && pos < toked.size()) {
        if(is_valid_type(toked[pos])) valid_type = true;
        pos++;
    }
    while(valid_type && pos < toked.size()) {
        if(is_valid_type(toked[pos])) outputsig.push_back(toked[pos]);
        else valid_type = false;
        pos++;
    }
        
    //if there's no explicit output sig then assume the output is the same as the first input
    if(outputsig.size() == 0) outputsig.push_back(inputsig[0]);
    assert(inputsig.size() != 0);
    assert(outputsig.size() != 0);
}

inline void run_cast_test2(volk_fn_2arg func, void *outbuff, std::vector<void *> &inbuffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(outbuff, inbuffs[0], vlen, arch.c_str());
}

inline void run_cast_test3(volk_fn_3arg func, void *outbuff, std::vector<void *> &inbuffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(outbuff, inbuffs[0], inbuffs[1], vlen, arch.c_str());
}

inline void run_cast_test4(volk_fn_4arg func, void *outbuff, std::vector<void *> &inbuffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(outbuff, inbuffs[0], inbuffs[1], inbuffs[2], vlen, arch.c_str());
}

bool run_volk_tests(const int archs[], void (*manual_func)(), std::string name, float tol, int vlen, int iter) {
    std::cout << "RUN_VOLK_TESTS: " << name << std::endl;
    
    //first let's get a list of available architectures for the test
    std::vector<std::string> arch_list = get_arch_list(archs);
    
    BOOST_FOREACH(std::string arch, arch_list) {
        std::cout << "Found an arch: " << arch << std::endl;
    }
    
    //now we have to get a function signature by parsing the name
    std::vector<std::string> inputsig, outputsig;
    get_function_signature(inputsig, outputsig, name);

    for(int i=0; i<inputsig.size(); i++) std::cout << "Input: " << inputsig[i] << std::endl;
    for(int i=0; i<outputsig.size(); i++) std::cout << "Output: " << outputsig[i] << std::endl;
    
    //now that we have that, we'll set up input and output buffers based on the function signature
    std::vector<void *> inbuffs;
    make_buffer_for_signature(inbuffs, inputsig, vlen);
    
    //and set the input buffers to something random
    //TODO
    
    //allocate output buffers -- one for each output for each arch
    std::vector<void *> outbuffs;
    BOOST_FOREACH(std::string arch, arch_list) {
        make_buffer_for_signature(outbuffs, outputsig, vlen);
    }
    
    //now run the test
    clock_t start, end;
    for(int i = 0; i < arch_list.size(); i++) {
        start = clock();
        switch(outputsig.size()+inputsig.size()) {
            case 2:
                run_cast_test2((volk_fn_2arg)(manual_func), outbuffs[i], inbuffs, vlen, iter, arch_list[i]);
                break;
            case 3:
                run_cast_test3((volk_fn_3arg)(manual_func), outbuffs[i], inbuffs, vlen, iter, arch_list[i]);
                break;
            case 4:
                run_cast_test4((volk_fn_4arg)(manual_func), outbuffs[i], inbuffs, vlen, iter, arch_list[i]);
                break;
            default:
                break;
        }
        end = clock();
        std::cout << arch_list[i] << " completed in " << (double)(end-start)/(double)CLOCKS_PER_SEC << "s" << std::endl;
    }

    //and now compare each output to the generic output
    //first we have to know which output is the generic one, they aren't in order...
    int generic_offset;
    for(int i=0; i<arch_list.size(); i++) 
        if(arch_list[i] == "generic") generic_offset=i;
    
    for(int i=0; i<arch_list.size(); i++) {
        if(arch_list[i] != "generic") {
            for(int j=0; i<vlen; j++) {
                BOOST_CHECK_CLOSE(((float *)(outbuffs[generic_offset]))[j], ((float *)(outbuffs[i]))[j], tol);
            }
        }
    }

    BOOST_FOREACH(void *buf, inbuffs) {
        free(buf);
    }
    BOOST_FOREACH(void *buf, outbuffs) {
        free(buf);
    }
    return 0;
}


