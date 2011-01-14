#include "qa_utils.h"
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>
//#include <boost/test/unit_test.hpp>
#include <iostream>
#include <vector>
#include <time.h>
//#include <math.h>
//#include <volk/volk_runtime.h>
#include <volk/volk_registry.h>
#include <volk/volk.h>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits.hpp>

float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

void random_floats (float *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = uniform ();
}

void load_random_data(void *data, std::string sig, unsigned int n) {
    if(sig == "32fc") {
        random_floats((float *)data, n*2);
    } else if(sig == "32f") {
        random_floats((float *)data, n);
    } else if(sig == "32u") {
        for(int i=0; i<n; i++) ((uint32_t *)data)[i] = (uint32_t) ((rand() - (RAND_MAX/2)) / (RAND_MAX/2));
    } else if(sig == "32s") {
        for(int i=0; i<n; i++) ((int32_t *)data)[i] = ((int32_t) (rand() - (RAND_MAX/2)));
    } else if(sig == "16u") {
        for(int i=0; i<n; i++) ((uint16_t *)data)[i] = (uint16_t) ((rand() - (RAND_MAX/2)) / (RAND_MAX/2));
    } else if(sig == "16s") {
        for(int i=0; i<n; i++) ((int16_t *)data)[i] = ((int16_t)((((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 32768.0));
    } else if(sig == "16sc") {
        for(int i=0; i<n*2; i++) ((int16_t *)data)[i] = ((int16_t)((((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * 32768.0));
    } else if(sig == "8u") {
        for(int i=0; i<n; i++) ((uint8_t *)data)[i] = ((uint8_t)(((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2)) * 256.0));
    } else if(sig == "8s") {
        for(int i=0; i<n; i++) ((int8_t *)data)[i] = ((int8_t)(((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2)) * 128.0));
    } else std::cout << "load_random_data(): Invalid sig: " << sig << std::endl;
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
        if     (sig=="32fc" || sig=="64f" || sig=="64u") buffs.push_back((void *) make_aligned_buffer<uint64_t>(vlen));
        else if(sig=="32f" || sig=="32u" || sig=="32s" || sig=="16sc") buffs.push_back((void *) make_aligned_buffer<uint32_t>(vlen));
        else if(sig=="16s" || sig=="16u" || sig=="8sc") buffs.push_back((void *) make_aligned_buffer<uint16_t>(vlen));
        else if(sig=="8s" || sig=="8u") buffs.push_back((void *) make_aligned_buffer<uint8_t>(vlen));
        else std::cout << "Invalid type: " << sig << std::endl;
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
    std::vector<std::string> valid_types = boost::assign::list_of("64f")("64u")("32fc")("32f")("32s")("32u")("16sc")("16s")("16u")("8s")("8sc")("8u");
    
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
        else pos++;
    }
    while(valid_type && pos < toked.size()) {
        if(is_valid_type(toked[pos])) outputsig.push_back(toked[pos]);
        else valid_type = false;
        pos++;
    }
    
    //if there's no output sig and only one input sig, assume there are 2 inputs
    //this handles conversion fn's (which have a specified output sig) and most of the rest
    if(outputsig.size() == 0 && inputsig.size() == 1) {
        outputsig.push_back(inputsig[0]);
        inputsig.push_back(inputsig[0]);
    }//if there's no explicit output sig then assume the output is the same as the first input
    else if(outputsig.size() == 0) outputsig.push_back(inputsig[0]);
    
    
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
    for(int i=0; i<inputsig.size(); i++) {
        load_random_data(inbuffs[i], inputsig[i], vlen);        
    }
    
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
        if(i != generic_offset) {
            if(outputsig[0] == "32fc") {
                for(int j=0; j<vlen*2; j++) {
                    if(fabs(((float *)(outbuffs[generic_offset]))[j] - ((float *)(outbuffs[i]))[j]) > tol) {
                        std::cout << "Generic: " << ((float *)(outbuffs[generic_offset]))[j] << " " << arch_list[i] << ": " << ((float *)(outbuffs[i]))[j] << std::endl;
                        return 1;
                    }
                }
            } else if(outputsig[0] == "32f") {
                for(int j=0; j<vlen; j++) {
                    if(fabs(((float *)(outbuffs[generic_offset]))[j] - ((float *)(outbuffs[i]))[j]) > tol) {
                        std::cout << "Generic: " << ((float *)(outbuffs[generic_offset]))[j] << " " << arch_list[i] << ": " << ((float *)(outbuffs[i]))[j] << std::endl;
                        return 1;
                    }
                }
            } else if(outputsig[0] == "32u" || outputsig[0] == "32s" || outputsig[0] == "16sc") {
                for(int j=0; j<vlen; j++) {
                    if(((uint32_t *)(outbuffs[generic_offset]))[j] != ((uint32_t *)(outbuffs[i]))[j]) {
                        std::cout << "Generic: " << ((uint32_t *)(outbuffs[generic_offset]))[j] << " " << arch_list[i] << ": " << ((uint32_t *)(outbuffs[i]))[j] << std::endl;
                        return 1;
                    }
                }
            } else if(outputsig[0] == "16u" || outputsig[0] == "16s" || outputsig[0] == "8sc") {
                for(int j=0; j<vlen; j++) {
                    if(((uint16_t *)(outbuffs[generic_offset]))[j] != ((uint16_t *)(outbuffs[i]))[j]) {
                        std::cout << "Generic: " << ((uint16_t *)(outbuffs[generic_offset]))[j] << " " << arch_list[i] << ": " << ((uint16_t *)(outbuffs[i]))[j] << std::endl;
                        return 1;
                    }
                }
            } else { 
                std::cout << "Error: invalid type " << outputsig[0] << std::endl;
                return 1;
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


