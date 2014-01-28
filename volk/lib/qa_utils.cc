#include "qa_utils.h"
#include <cstring>
#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/tokenizer.hpp>
#include <boost/xpressive/xpressive.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <cmath>
#include <limits>
#include <boost/lexical_cast.hpp>
#include <volk/volk.h>
#include <volk/volk_cpu.h>
#include <volk/volk_common.h>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits.hpp>
#include <stdio.h>

float uniform() {
  return 2.0 * ((float) rand() / RAND_MAX - 0.5);	// uniformly (-1, 1)
}

template <class t>
void random_floats (t *buf, unsigned n)
{
  for (unsigned i = 0; i < n; i++)
    buf[i] = uniform ();
}

void load_random_data(void *data, volk_type_t type, unsigned int n) {
    if(type.is_complex) n *= 2;
    if(type.is_float) {
        if(type.size == 8) random_floats<double>((double *)data, n);
        else random_floats<float>((float *)data, n);
    } else {
        float int_max = float(uint64_t(2) << (type.size*8));
        if(type.is_signed) int_max /= 2.0;
        for(unsigned int i=0; i<n; i++) {
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
                throw "load_random_data: no support for data size > 8 or < 1"; //no shenanigans here
            }
        }
    }
}

static std::vector<std::string> get_arch_list(volk_func_desc_t desc) {
    std::vector<std::string> archlist;

    for(size_t i = 0; i < desc.n_impls; i++) {
        //if(!(archs[i+1] & volk_get_lvarch())) continue; //this arch isn't available on this pc
        archlist.push_back(std::string(desc.impl_names[i]));
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

    if(name.size() < 2) throw std::string("name too short to be a datatype");

    //is it a scalar?
    if(name[0] == 's') {
        type.is_scalar = true;
        name = name.substr(1, name.size()-1);
    }

    //get the data size
    size_t last_size_pos = name.find_last_of("0123456789");
    if(last_size_pos == std::string::npos)
      throw std::string("no size spec in type ").append(name);
    //will throw if malformed
    int size = boost::lexical_cast<int>(name.substr(0, last_size_pos+1));

    assert(((size % 8) == 0) && (size <= 64) && (size != 0));
    type.size = size/8; //in bytes

    for(size_t i=last_size_pos+1; i < name.size(); i++) {
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

    enum { SIDE_INPUT, SIDE_NAME, SIDE_OUTPUT } side = SIDE_INPUT;
    std::string fn_name;
    volk_type_t type;
    BOOST_FOREACH(std::string token, toked) {
        try {
            type = volk_type_from_string(token);
            if(side == SIDE_NAME) side = SIDE_OUTPUT; //if this is the first one after the name...

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
                side = SIDE_NAME;
                fn_name.append("_");
                fn_name.append(token);
            }
            else if(side == SIDE_OUTPUT) {
                if(token != toked.back()) throw; //the last token in the name is the alignment
            }
        }
    }
    //we don't need an output signature (some fn's operate on the input data, "in place"), but we do need at least one input!
    assert(inputsig.size() != 0);
    
}

inline void run_cast_test1(volk_fn_1arg func, std::vector<void *> &buffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], vlen, arch.c_str());
}

inline void run_cast_test2(volk_fn_2arg func, std::vector<void *> &buffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], vlen, arch.c_str());
}

inline void run_cast_test3(volk_fn_3arg func, std::vector<void *> &buffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], buffs[2], vlen, arch.c_str());
}

inline void run_cast_test4(volk_fn_4arg func, std::vector<void *> &buffs, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], buffs[2], buffs[3], vlen, arch.c_str());
}

inline void run_cast_test1_s32f(volk_fn_1arg_s32f func, std::vector<void *> &buffs, float scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], scalar, vlen, arch.c_str());
}

inline void run_cast_test2_s32f(volk_fn_2arg_s32f func, std::vector<void *> &buffs, float scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], scalar, vlen, arch.c_str());
}

inline void run_cast_test3_s32f(volk_fn_3arg_s32f func, std::vector<void *> &buffs, float scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], buffs[2], scalar, vlen, arch.c_str());
}

inline void run_cast_test1_s32fc(volk_fn_1arg_s32fc func, std::vector<void *> &buffs, lv_32fc_t scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], scalar, vlen, arch.c_str());
}

inline void run_cast_test2_s32fc(volk_fn_2arg_s32fc func, std::vector<void *> &buffs, lv_32fc_t scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], scalar, vlen, arch.c_str());
}

inline void run_cast_test3_s32fc(volk_fn_3arg_s32fc func, std::vector<void *> &buffs, lv_32fc_t scalar, unsigned int vlen, unsigned int iter, std::string arch) {
    while(iter--) func(buffs[0], buffs[1], buffs[2], scalar, vlen, arch.c_str());
}

// This function is a nop that helps resolve GNU Radio bugs 582 and 583.
// Without this the cast in run_volk_tests for tol_i = static_cast<int>(float tol)
// won't happen on armhf (reported on cortex A9 and A15).
void lv_force_cast_hf( int tol_i, float tol_f)
{
    int diff_i = 1;
    float diff_f = 1;
    if( diff_i > tol_i )
        std::cout << "" ;
    if( diff_f > tol_f )
        std::cout << "" ;
}

template <class t>
bool fcompare(t *in1, t *in2, unsigned int vlen, float tol) {
    bool fail = false;
    int print_max_errs = 10;
    for(unsigned int i=0; i<vlen; i++) {
        // for very small numbers we'll see round off errors due to limited 
        // precision. So a special test case... 
        if(fabs(((t *)(in1))[i]) < 1e-30) {
            if( fabs( ((t *)(in2))[i] ) > tol )
            {
                fail=true;
                if(print_max_errs-- > 0) {
                    std::cout << "offset " << i << " in1: " << t(((t *)(in1))[i]) << " in2: " << t(((t *)(in2))[i]) << std::endl;
                }
            }
        }
        // the primary test is the percent different greater than given tol
        else if(fabs(((t *)(in1))[i] - ((t *)(in2))[i])/(((t *)in1)[i]) > tol) {
            fail=true;
            if(print_max_errs-- > 0) {
                std::cout << "offset " << i << " in1: " << t(((t *)(in1))[i]) << " in2: " << t(((t *)(in2))[i]) << std::endl;
            }
        }
    }

    return fail;
}

template <class t>
bool ccompare(t *in1, t *in2, unsigned int vlen, float tol) {
    bool fail = false;
    int print_max_errs = 10;
    for(unsigned int i=0; i<2*vlen; i+=2) {
        t diff[2] = { in1[i] - in2[i], in1[i+1] - in2[i+1] };
        t err  = std::sqrt(diff[0] * diff[0] + diff[1] * diff[1]);
        t norm = std::sqrt(in1[i] * in1[i] + in1[i+1] * in1[i+1]);

        // for very small numbers we'll see round off errors due to limited 
        // precision. So a special test case... 
        if (norm < 1e-30) {
            if (err > tol)
            {
                fail=true;
                if(print_max_errs-- > 0) {
                    std::cout << "offset " << i/2 << " in1: " << in1[i] << " + " << in1[i+1] << "j  in2: " << in2[i] << " + " << in2[i+1] << "j" << std::endl;
                }
            }
        }
        // the primary test is the percent different greater than given tol
        else if((err / norm) > tol) {
            fail=true;
            if(print_max_errs-- > 0) {
                std::cout << "offset " << i/2 << " in1: " << in1[i] << " + " << in1[i+1] << "j  in2: " << in2[i] << " + " << in2[i+1] << "j" << std::endl;
            }
        }
    }

    return fail;
}

template <class t>
bool icompare(t *in1, t *in2, unsigned int vlen, unsigned int tol) {
    bool fail = false;
    int print_max_errs = 10;
    for(unsigned int i=0; i<vlen; i++) {
        if(abs(int(((t *)(in1))[i]) - int(((t *)(in2))[i])) > tol) {
            fail=true;
            if(print_max_errs-- > 0) {
                std::cout << "offset " << i << " in1: " << static_cast<int>(t(((t *)(in1))[i])) << " in2: " << static_cast<int>(t(((t *)(in2))[i])) << std::endl;
            }
        }
    }

    return fail;
}

class volk_qa_aligned_mem_pool{
public:
    void *get_new(size_t size){
        size_t alignment = volk_get_alignment();
        _mems.push_back(std::vector<char>(size + alignment-1, 0));
        size_t ptr = size_t(&_mems.back().front());
        return (void *)((ptr + alignment-1) & ~(alignment-1));
    }
private: std::list<std::vector<char> > _mems;
};

bool run_volk_tests(volk_func_desc_t desc,
                    void (*manual_func)(),
                    std::string name,
                    float tol,
                    lv_32fc_t scalar,
                    int vlen,
                    int iter,
                    std::vector<std::string> *best_arch_vector = 0,
                    std::string puppet_master_name = "NULL",
                    bool benchmark_mode,
                    std::string kernel_regex
                   ) {
    boost::xpressive::sregex kernel_expression = boost::xpressive::sregex::compile(kernel_regex);
    if( !boost::xpressive::regex_search(name, kernel_expression) ) {
        // in this case we have a regex and are only looking to test one kernel
        return false;
    }
    std::cout << "RUN_VOLK_TESTS: " << name << "(" << vlen << "," << iter << ")" << std::endl;

    // The multiply and lv_force_cast_hf are work arounds for GNU Radio bugs 582 and 583
    // The bug is the casting/assignment below do not happen, which results in false
    // positives when testing for errors in fcompare and icompare.
    // Since this only happens on armhf (reported for Cortex A9 and A15) combined with
    // the following fixes it is suspected to be a compiler bug. 
    // Bug 1272024 on launchpad has been filed with Linaro GCC.
    const float tol_f = tol*1.0000001;
    const unsigned int tol_i = static_cast<const unsigned int>(tol);
    lv_force_cast_hf( tol_i, tol_f );

    //first let's get a list of available architectures for the test
    std::vector<std::string> arch_list = get_arch_list(desc);

    if((!benchmark_mode) && (arch_list.size() < 2)) {
        std::cout << "no architectures to test" << std::endl;
        return false;
    }

    //something that can hang onto memory and cleanup when this function exits
    volk_qa_aligned_mem_pool mem_pool;

    //now we have to get a function signature by parsing the name
    std::vector<volk_type_t> inputsig, outputsig;
    get_signatures_from_name(inputsig, outputsig, name);
    
    //pull the input scalars into their own vector
    std::vector<volk_type_t> inputsc;
    for(size_t i=0; i<inputsig.size(); i++) {
        if(inputsig[i].is_scalar) {
            inputsc.push_back(inputsig[i]);
            inputsig.erase(inputsig.begin() + i);
            i -= 1;
        }
    }
    //for(int i=0; i<inputsig.size(); i++) std::cout << "Input: " << inputsig[i].str << std::endl;
    //for(int i=0; i<outputsig.size(); i++) std::cout << "Output: " << outputsig[i].str << std::endl;
    std::vector<void *> inbuffs;
    BOOST_FOREACH(volk_type_t sig, inputsig) {
        if(!sig.is_scalar) //we don't make buffers for scalars
          inbuffs.push_back(mem_pool.get_new(vlen*sig.size*(sig.is_complex ? 2 : 1)));
    }
    for(size_t i=0; i<inbuffs.size(); i++) {
        load_random_data(inbuffs[i], inputsig[i], vlen);
    }

    //ok let's make a vector of vector of void buffers, which holds the input/output vectors for each arch
    std::vector<std::vector<void *> > test_data;
    for(size_t i=0; i<arch_list.size(); i++) {
        std::vector<void *> arch_buffs;
        for(size_t j=0; j<outputsig.size(); j++) {
            arch_buffs.push_back(mem_pool.get_new(vlen*outputsig[j].size*(outputsig[j].is_complex ? 2 : 1)));
        }
        for(size_t j=0; j<inputsig.size(); j++) {
            arch_buffs.push_back(inbuffs[j]);
        }
        test_data.push_back(arch_buffs);
    }

    std::vector<volk_type_t> both_sigs;
    both_sigs.insert(both_sigs.end(), outputsig.begin(), outputsig.end());
    both_sigs.insert(both_sigs.end(), inputsig.begin(), inputsig.end());

    //now run the test
    clock_t start, end;
    std::vector<double> profile_times;
    for(size_t i = 0; i < arch_list.size(); i++) {
        start = clock();

        switch(both_sigs.size()) {
            case 1:
                if(inputsc.size() == 0) {
                    run_cast_test1((volk_fn_1arg)(manual_func), test_data[i], vlen, iter, arch_list[i]);
                } else if(inputsc.size() == 1 && inputsc[0].is_float) {
                    if(inputsc[0].is_complex) {
                        run_cast_test1_s32fc((volk_fn_1arg_s32fc)(manual_func), test_data[i], scalar, vlen, iter, arch_list[i]);
                    } else {
                        run_cast_test1_s32f((volk_fn_1arg_s32f)(manual_func), test_data[i], scalar.real(), vlen, iter, arch_list[i]);
                    }
                } else throw "unsupported 1 arg function >1 scalars";
                break;
            case 2:
                if(inputsc.size() == 0) {
                    run_cast_test2((volk_fn_2arg)(manual_func), test_data[i], vlen, iter, arch_list[i]);
                } else if(inputsc.size() == 1 && inputsc[0].is_float) {
                    if(inputsc[0].is_complex) {
                        run_cast_test2_s32fc((volk_fn_2arg_s32fc)(manual_func), test_data[i], scalar, vlen, iter, arch_list[i]);
                    } else {
                        run_cast_test2_s32f((volk_fn_2arg_s32f)(manual_func), test_data[i], scalar.real(), vlen, iter, arch_list[i]);
                    }
                } else throw "unsupported 2 arg function >1 scalars";
                break;
            case 3:
                if(inputsc.size() == 0) {
                    run_cast_test3((volk_fn_3arg)(manual_func), test_data[i], vlen, iter, arch_list[i]);
                } else if(inputsc.size() == 1 && inputsc[0].is_float) {
                    if(inputsc[0].is_complex) {
                        run_cast_test3_s32fc((volk_fn_3arg_s32fc)(manual_func), test_data[i], scalar, vlen, iter, arch_list[i]);
                    } else {
                        run_cast_test3_s32f((volk_fn_3arg_s32f)(manual_func), test_data[i], scalar.real(), vlen, iter, arch_list[i]);
                    }
                } else throw "unsupported 3 arg function >1 scalars";
                break;
            case 4:
                run_cast_test4((volk_fn_4arg)(manual_func), test_data[i], vlen, iter, arch_list[i]);
                break;
            default:
                throw "no function handler for this signature";
                break;
        }

        end = clock();
        double arch_time = 1000.0 * (double)(end-start)/(double)CLOCKS_PER_SEC;
        std::cout << arch_list[i] << " completed in " << arch_time << "ms" << std::endl;

        profile_times.push_back(arch_time);
    }

    //and now compare each output to the generic output
    //first we have to know which output is the generic one, they aren't in order...
    size_t generic_offset=0;
    for(size_t i=0; i<arch_list.size(); i++)
        if(arch_list[i] == "generic") generic_offset=i;

    //now compare
    //if(outputsig.size() == 0) outputsig = inputsig; //a hack, i know

    bool fail = false;
    bool fail_global = false;
    std::vector<bool> arch_results;
    for(size_t i=0; i<arch_list.size(); i++) {
        fail = false;
        if(i != generic_offset) {
            for(size_t j=0; j<both_sigs.size(); j++) {
                if(both_sigs[j].is_float) {
                    if(both_sigs[j].size == 8) {
                        if (both_sigs[j].is_complex) {
                            fail = ccompare((double *) test_data[generic_offset][j], (double *) test_data[i][j], vlen, tol_f);

                        } else {
                            fail = fcompare((double *) test_data[generic_offset][j], (double *) test_data[i][j], vlen, tol_f);
                        }
                    } else {
                        if (both_sigs[j].is_complex) {
                            fail = ccompare((float *) test_data[generic_offset][j], (float *) test_data[i][j], vlen, tol_f);
                        } else {
                            fail = fcompare((float *) test_data[generic_offset][j], (float *) test_data[i][j], vlen, tol_f);
                        }
                    }
                } else {
                    //i could replace this whole switch statement with a memcmp if i wasn't interested in printing the outputs where they differ
                    switch(both_sigs[j].size) {
                    case 8:
                        if(both_sigs[j].is_signed) {
                            fail = icompare((int64_t *) test_data[generic_offset][j], (int64_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        } else {
                            fail = icompare((uint64_t *) test_data[generic_offset][j], (uint64_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        }
                        break;
                    case 4:
                        if(both_sigs[j].is_signed) {
                            fail = icompare((int32_t *) test_data[generic_offset][j], (int32_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        } else {
                            fail = icompare((uint32_t *) test_data[generic_offset][j], (uint32_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        }
                        break;
                    case 2:
                        if(both_sigs[j].is_signed) {
                            fail = icompare((int16_t *) test_data[generic_offset][j], (int16_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        } else {
                            fail = icompare((uint16_t *) test_data[generic_offset][j], (uint16_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        }
                        break;
                    case 1:
                        if(both_sigs[j].is_signed) {
                            fail = icompare((int8_t *) test_data[generic_offset][j], (int8_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        } else {
                            fail = icompare((uint8_t *) test_data[generic_offset][j], (uint8_t *) test_data[i][j], vlen*(both_sigs[j].is_complex ? 2 : 1), tol_i);
                        }
                        break;
                    default:
                        fail=1;
                    }
                }
                if(fail) {
                    fail_global = true;
                    std::cout << name << ": fail on arch " << arch_list[i] << std::endl;
                }
                //fail = memcmp(outbuffs[generic_offset], outbuffs[i], outputsig[0].size * vlen * (outputsig[0].is_complex ? 2:1));
            }
        }
        arch_results.push_back(!fail);
    }

    double best_time_a = std::numeric_limits<double>::max();
    double best_time_u = std::numeric_limits<double>::max();
    std::string best_arch_a = "generic";
    std::string best_arch_u = "generic";
    for(size_t i=0; i < arch_list.size(); i++)
    {
        if((profile_times[i] < best_time_u) && arch_results[i] && desc.impl_alignment[i] == 0)
        {
            best_time_u = profile_times[i];
            best_arch_u = arch_list[i];
        }
        if((profile_times[i] < best_time_a) && arch_results[i])
        {
            best_time_a = profile_times[i];
            best_arch_a = arch_list[i];
        }
    }

    std::cout << "Best aligned arch: " << best_arch_a << std::endl;
    std::cout << "Best unaligned arch: " << best_arch_u << std::endl;
    if(best_arch_vector) {
        if(puppet_master_name == "NULL") {
            best_arch_vector->push_back(name + " " + best_arch_a + " " + best_arch_u);
        }
        else {
            best_arch_vector->push_back(puppet_master_name + " " + best_arch_a + " " + best_arch_u);
        }
    }

    return fail_global;
}


