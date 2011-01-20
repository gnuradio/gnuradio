#include "qa_utils.h"
#include <stdlib.h>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>
//#include <boost/test/unit_test.hpp>
#include <iostream>
#include <vector>
#include <time.h>
#include <math.h>
#include <boost/lexical_cast.hpp>
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

void load_random_data(void *data, volk_type_t type, unsigned int n) {
    if(type.is_complex) n *= 2;
    if(type.is_float) {
        assert(type.size == 4); //TODO: double support
        random_floats((float *)data, n);
    } else {
        float int_max = pow(2, type.size*8);
        if(type.is_signed) int_max /= 2.0;
        for(int i=0; i<n; i++) {
            float scaled_rand = (((float) (rand() - (RAND_MAX/2))) / static_cast<float>((RAND_MAX/2))) * int_max;
            //man i really don't know how to do this in a more clever way, you have to cast down at some point
            switch(type.size) {
            case 8:
                if(type.is_signed) ((int64_t *)data)[i] = (int64_t) scaled_rand;
                else ((uint64_t *)data)[i] = (uint64_t) scaled_rand;
            break;
            case 4:
                if(type.is_signed) ((int32_t *)data)[i] = (int32_t) scaled_rand;
                else ((uint32_t *)data)[i] = (uint32_t) scaled_rand;
            break;           
            case 2:
                if(type.is_signed) ((int16_t *)data)[i] = (int16_t) scaled_rand;
                else ((uint16_t *)data)[i] = (uint16_t) scaled_rand;
            break;
            case 1:
                if(type.is_signed) ((int8_t *)data)[i] = (int8_t) scaled_rand;
                else ((uint8_t *)data)[i] = (uint8_t) scaled_rand;
            break;
            default:
                throw; //no shenanigans here
            }
        }
    }
}

void *make_aligned_buffer(unsigned int len, unsigned int size) {
  void *buf;
  int ret;
  ret = posix_memalign((void**)&buf, 16, len * size);
  assert(ret == 0);
  return buf;
}

void make_buffer_for_signature(std::vector<void *> &buffs, std::vector<volk_type_t> inputsig, unsigned int vlen) {
    BOOST_FOREACH(volk_type_t sig, inputsig) {
        if(!sig.is_scalar) //we don't make buffers for scalars
          buffs.push_back(make_aligned_buffer(vlen, sig.size*(sig.is_complex ? 2 : 1)));
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

volk_type_t volk_type_from_string(std::string name) {
    volk_type_t type;
    type.is_float = false;
    type.is_scalar = false;
    type.is_complex = false;
    type.is_signed = false;
    type.size = 0;
    type.str = name;
    
    assert(name.size() > 1);
    
    //is it a scalar?
    if(name[0] == 's') { 
        type.is_scalar = true;
        name = name.substr(1, name.size()-1);
    }
    
    //get the data size
    int last_size_pos = name.find_last_of("0123456789");
    if(last_size_pos < 0) throw 0;
    //will throw if malformed
    int size = boost::lexical_cast<int>(name.substr(0, last_size_pos+1));

    assert(((size % 8) == 0) && (size <= 64) && (size != 0));
    type.size = size/8; //in bytes
    
    for(int i=last_size_pos+1; i < name.size(); i++) {
        switch (name[i]) {
        case 'f':
            type.is_float = true;
            break;
        case 'i':
            type.is_signed = true;
            break;
        case 'c':
            type.is_complex = true;
            break;
        case 'u':
            type.is_signed = false;
            break;
        default:
            throw;
        }
    }
    
    return type;
}

static void get_signatures_from_name(std::vector<volk_type_t> &inputsig, 
                                   std::vector<volk_type_t> &outputsig, 
                                   std::string name) {
    boost::char_separator<char> sep("_");
    boost::tokenizer<boost::char_separator<char> > tok(name, sep);
    std::vector<std::string> toked;
    tok.assign(name);
    toked.assign(tok.begin(), tok.end());
    
    assert(toked[0] == "volk");
    toked.erase(toked.begin());

    //ok. we're assuming a string in the form
    //(sig)_(multiplier-opt)_..._(name)_(sig)_(multiplier-opt)_..._(alignment)

    enum { SIDE_INPUT, SIDE_OUTPUT } side = SIDE_INPUT;
    std::string fn_name;
    volk_type_t type;
    BOOST_FOREACH(std::string token, toked) {
        try {
            type = volk_type_from_string(token);
            if(side == SIDE_INPUT) inputsig.push_back(type);
            else outputsig.push_back(type);
        } catch (...){
            if(token[0] == 'x') { //it's a multiplier
                if(side == SIDE_INPUT) assert(inputsig.size() > 0);
                else assert(outputsig.size() > 0);
                int multiplier = boost::lexical_cast<int>(token.substr(1, token.size()-1)); //will throw if invalid
                for(int i=1; i<multiplier; i++) {
                    if(side == SIDE_INPUT) inputsig.push_back(inputsig.back());
                    else outputsig.push_back(outputsig.back());
                }
            }
            else if(side == SIDE_INPUT) { //it's the function name, at least it better be
                side = SIDE_OUTPUT;
                fn_name = token;
            } else {
                if(token != toked.back()) throw; //the last token in the name is the alignment
            }
        }
    }
    //we don't need an output signature (some fn's operate on the input data, "in place"), but we do need at least one input!
    assert(inputsig.size() != 0);
}

inline void run_cast_test1(volk_fn_1arg func, void *buff, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buff, vlen, arch.c_str());
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

inline void run_cast_test1_s32f(volk_fn_1arg_s32f func, void *buff, float scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buff, scalar, vlen, arch.c_str());
}

inline void run_cast_test2_s32f(volk_fn_2arg_s32f func, void *outbuff, std::vector<void *> &inbuffs, float scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(outbuff, inbuffs[0], scalar, vlen, arch.c_str());
}

template <class t>
bool fcompare(t *in1, t *in2, unsigned int vlen, float tol) {
    for(int i=0; i<vlen; i++) {
        if(fabs(((t *)(in1))[i] - ((t *)(in2))[i]) > tol) return 1;
    }
    return 0;
}

template <class t>
bool icompare(t *in1, t *in2, unsigned int vlen) {
    for(int i=0; i<vlen; i++) {
        if(((t *)(in1))[i] != ((t *)(in2))[i]) return 1;
    }
    return 0;
}

bool run_volk_tests(const int archs[], void (*manual_func)(), std::string name, float tol, int vlen, int iter) {
    std::cout << "RUN_VOLK_TESTS: " << name << std::endl;
    
    //first let's get a list of available architectures for the test
    std::vector<std::string> arch_list = get_arch_list(archs);
    
    //now we have to get a function signature by parsing the name
    std::vector<volk_type_t> inputsig, outputsig;
    get_signatures_from_name(inputsig, outputsig, name);
    
    std::vector<volk_type_t> inputsc, outputsc;
    for(int i=0; i<inputsig.size(); i++) {
        if(inputsig[i].is_scalar) {
            inputsc.push_back(inputsig[i]);
            inputsig.erase(inputsig.begin() + i);
        }
    }
    for(int i=0; i<outputsig.size(); i++) {
        if(outputsig[i].is_scalar) {
            outputsc.push_back(outputsig[i]);
            outputsig.erase(outputsig.begin() + i);
        }
    }
    assert(outputsc.size() == 0); //we don't do output scalars yet

    //for(int i=0; i<inputsig.size(); i++) std::cout << "Input: " << inputsig[i].str << std::endl;
    //for(int i=0; i<outputsig.size(); i++) std::cout << "Output: " << outputsig[i].str << std::endl;
    std::vector<void *> inbuffs, outbuffs;
    
    if(outputsig.size() == 0) { //we're operating in place...
        //assert(inputsig.size() == 1); //we only support 0 output 1 input right now...
        make_buffer_for_signature(inbuffs, inputsig, vlen); //let's make an input buffer
        load_random_data(inbuffs[0], inputsig[0], vlen); //and load it with random data
        BOOST_FOREACH(std::string arch, arch_list) { //then copy the same random data to each output buffer
            make_buffer_for_signature(outbuffs, inputsig, vlen);
            memcpy(outbuffs.back(), inbuffs[0], vlen*inputsig[0].size*(inputsig[0].is_complex?2:1));
        }
    } else {
        make_buffer_for_signature(inbuffs, inputsig, vlen);
        BOOST_FOREACH(std::string arch, arch_list) {
            make_buffer_for_signature(outbuffs, outputsig, vlen);
        }
    
        //and set the input buffers to something random
        for(int i=0; i<inbuffs.size(); i++) {
            load_random_data(inbuffs[i], inputsig[i], vlen);        
        }
    }
    
    //now run the test
    clock_t start, end;
    for(int i = 0; i < arch_list.size(); i++) {
        start = clock();

        switch(inputsig.size() + outputsig.size()) {
            case 1:
                if(inputsc.size() == 0) {
                    run_cast_test1((volk_fn_1arg)(manual_func), outbuffs[i], vlen, iter, arch_list[i]); 
                } else if(inputsc.size() == 1 && inputsc[0].is_float) {
                    run_cast_test1_s32f((volk_fn_1arg_s32f)(manual_func), outbuffs[i], 1000.0, vlen, iter, arch_list[i]);
                } else throw "unsupported 1 arg function >1 scalars";
                break;
            case 2:
                if(inputsc.size() == 0) {
                    run_cast_test2((volk_fn_2arg)(manual_func), outbuffs[i], inbuffs, vlen, iter, arch_list[i]);
                } else if(inputsc.size() == 1 && inputsc[0].is_float) {
                    run_cast_test2_s32f((volk_fn_2arg_s32f)(manual_func), outbuffs[i], inbuffs, 1000.0, vlen, iter, arch_list[i]);
                } else throw "unsupported 2 arg function >1 scalars";
                break;
            case 3:
                run_cast_test3((volk_fn_3arg)(manual_func), outbuffs[i], inbuffs, vlen, iter, arch_list[i]);
                break;
            case 4:
                run_cast_test4((volk_fn_4arg)(manual_func), outbuffs[i], inbuffs, vlen, iter, arch_list[i]);
                break;
            default:
                throw "no function handler for this signature";
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
        
    //now compare
    if(outputsig.size() == 0) outputsig = inputsig; //a hack, i know

    bool fail = false;
    for(int i=0; i<arch_list.size(); i++) {
        if(i != generic_offset) {
            if(outputsig[0].str == "32fc") {
                fail = fcompare((float *) outbuffs[generic_offset], (float *) outbuffs[i], vlen*2, tol);
            } else if(outputsig[0].str == "32f") {
                fail = fcompare((float *) outbuffs[generic_offset], (float *) outbuffs[i], vlen, tol);
            } else if(outputsig[0].str == "32u" || outputsig[0].str == "32s" || outputsig[0].str == "16sc") {
                fail = icompare((uint32_t *) outbuffs[generic_offset], (uint32_t *) outbuffs[i], vlen);
            } else if(outputsig[0].size == 2) {
                fail = icompare((uint16_t *) outbuffs[generic_offset], (uint16_t *) outbuffs[i], vlen);
            } else if(outputsig[0].size == 1) {
                fail = icompare((uint8_t *) outbuffs[generic_offset], (uint8_t *) outbuffs[i], vlen);
            } else { 
                std::cout << "Error: invalid type " << outputsig[0].str << std::endl;
                fail = true;
            }
            if(fail) {
                std::cout << name << ": fail on arch " << arch_list[i] << std::endl;
            }
        }
    }

//    BOOST_FOREACH(void *buf, inbuffs) {
//        free(buf);
//    }
//    BOOST_FOREACH(void *buf, outbuffs) {
//        free(buf);
//    }
    return fail;
}


