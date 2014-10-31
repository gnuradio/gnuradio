/* -*- c++ -*- */
/*
 * Copyright 2012-2014 Free Software Foundation, Inc.
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

#include "qa_utils.h"

#include <volk/volk.h>
#include <volk/volk_prefs.h>

#include <ciso646>
#include <vector>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>

namespace fs = boost::filesystem;

void write_json(std::ofstream &json_file, std::vector<volk_test_results_t> results) {
    json_file << "{" << std::endl;
    json_file << " \"volk_tests\": [" << std::endl;
    size_t len = results.size();
    size_t i = 0;
    BOOST_FOREACH(volk_test_results_t &result, results) {
        json_file << "  {" << std::endl;
        json_file << "   \"name\": \"" << result.name << "\"," << std::endl;
        json_file << "   \"vlen\": " << result.vlen << "," << std::endl;
        json_file << "   \"iter\": " << result.iter << "," << std::endl;
        json_file << "   \"best_arch_a\": \"" << result.best_arch_a
            << "\"," << std::endl;
        json_file << "   \"best_arch_u\": \"" << result.best_arch_u
            << "\"," << std::endl;
        json_file << "   \"results\": {" << std::endl;
        size_t results_len = result.results.size();
        size_t ri = 0;
        typedef std::pair<std::string, volk_test_time_t> tpair;
        BOOST_FOREACH(tpair pair, result.results) {
            volk_test_time_t time = pair.second;
            json_file << "    \"" << time.name << "\": {" << std::endl;
            json_file << "     \"name\": \"" << time.name << "\"," << std::endl;
            json_file << "     \"time\": " << time.time << "," << std::endl;
            json_file << "     \"units\": \"" << time.units << "\"" << std::endl;
            json_file << "    }" ;
            if(ri+1 != results_len) {
                json_file << ",";
            }
            json_file << std::endl;
            ri++;
        }
        json_file << "   }" << std::endl;
        json_file << "  }";
        if(i+1 != len) {
            json_file << ",";
        }
        json_file << std::endl;
        i++;
    }
    json_file << " ]" << std::endl;
    json_file << "}" << std::endl;
}

int main(int argc, char *argv[]) {
    // Adding program options
    boost::program_options::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help messages")
      ("benchmark,b",
            boost::program_options::value<bool>()->default_value( false )
                                                ->implicit_value( true ),
            "Run all kernels (benchmark mode)")
      ("tests-regex,R",
            boost::program_options::value<std::string>(),
            "Run tests matching regular expression.")
      ("json,j",
            boost::program_options::value<std::string>(),
            "JSON output file")
      ;

    // Handle the options that were given
    boost::program_options::variables_map vm;
    bool benchmark_mode;
    std::string kernel_regex;
    bool store_results = true;
    std::ofstream json_file;

    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
        benchmark_mode = vm.count("benchmark")?vm["benchmark"].as<bool>():false;
        if ( vm.count("tests-regex" ) ) {
            kernel_regex = vm["tests-regex"].as<std::string>();
            store_results = false;
            std::cout << "Warning: using a regexp will not save results to a config" << std::endl;
        }
        else {
            kernel_regex = ".*";
            store_results = true;
        }
    } catch (boost::program_options::error& error) {
        std::cerr << "Error: " << error.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }
    /** --help option
*/
    if ( vm.count("help") )
    {
      std::cout << "The VOLK profiler." << std::endl
                << desc << std::endl;
      return 0;
    }

    if ( vm.count("json") )
    {
        json_file.open( vm["json"].as<std::string>().c_str() );
    }


    // Run tests
    std::vector<volk_test_results_t> results;

    //VOLK_PROFILE(volk_16i_x5_add_quad_16i_x4, 1e-4, 2046, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_16i_branch_4_state_8, 1e-4, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_8u_conv_k7_r2puppet_8u, volk_8u_x4_conv_k7_r2_8u, 0, 0, 2060, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_32fc_s32fc_rotatorpuppet_32fc, volk_32fc_s32fc_x2_rotator_32fc, 1e-2, (lv_32fc_t)lv_cmake(0.953939201, 0.3), 20462, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_s32f_deinterleave_real_32f, 1e-5, 32768.0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_deinterleave_real_8i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_deinterleave_16i_x2, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_s32f_deinterleave_32f_x2, 1e-4, 32768.0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_deinterleave_real_16i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_magnitude_16i, 1, 0, 204602, 100, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16ic_s32f_magnitude_32f, 1e-5, 32768.0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16i_s32f_convert_32f, 1e-4, 32768.0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16i_convert_8i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_16i_max_star_16i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_16i_max_star_horizontal_16i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_16i_permute_and_scalar_add, 1e-4, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_16i_x4_quad_max_star_16i, 1e-4, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_16u_byteswappuppet_16u, volk_16u_byteswap, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16i_32fc_dot_prod_32fc, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_accumulator_s32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_add_32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_32f_multiply_32fc, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_log2_32f, 1.5e-1, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_expfast_32f, 1e-1, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_pow_32f, 1e-2, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_sin_32f, 1e-6, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_cos_32f, 1e-6, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_tan_32f, 1e-6, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_atan_32f, 1e-3, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_asin_32f, 1e-3, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_acos_32f, 1e-3, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_power_32fc, 1e-4, 0, 204602, 50, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_calc_spectral_noise_floor_32f, 1e-4, 20.0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_atan2_32f, 1e-4, 10.0, 204602, 100, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 204602, 100, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_deinterleave_32f_x2, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_deinterleave_64f_x2, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_deinterleave_real_16i, 0, 32768, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_deinterleave_imag_32f, 1e-4, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_deinterleave_real_32f, 1e-4, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_deinterleave_real_64f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_dot_prod_32fc, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_32f_dot_prod_32fc, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_index_max_16u, 3, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_magnitude_16i, 1, 32768, 204602, 100, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_magnitude_32f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_magnitude_squared_32f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_multiply_32fc, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_multiply_conjugate_32fc, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_conjugate_32fc, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_convert_16i, 1, 32768, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_convert_32i, 1, 1<<31, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_convert_64f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_convert_8i, 1, 128, 204602, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_32fc_s32f_x2_power_spectral_density_32f, 1e-4, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_power_spectrum_32f, 1e-4, 0, 20462, 100, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_square_dist_32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_s32f_square_dist_scalar_mult_32f, 1e-4, 10, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_divide_32f, 1e-4, 0, 204602, 2000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_dot_prod_32f, 1e-4, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_dot_prod_16i, 1e-4, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_32f_s32f_32f_fm_detect_32f, 1e-4, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_index_max_16u, 3, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_s32f_interleave_16ic, 1, 32768, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_interleave_32fc, 0, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_max_32f, 1e-4, 0, 204602, 2000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_min_32f, 1e-4, 0, 204602, 2000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_multiply_32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_normalize, 1e-4, 100, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_power_32f, 1e-4, 4, 204602, 100, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_sqrt_32f, 1e-4, 0, 204602, 100, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_stddev_32f, 1e-4, 100, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_stddev_and_mean_32f_x2, 1e-4, 0, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_subtract_32f, 1e-4, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x3_sum_of_poly_32f, 1e-2, 0, 204602, 5000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32i_x2_and_32i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32i_s32f_convert_32f, 1e-4, 100, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32i_x2_or_32i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_32u_byteswappuppet_32u, volk_32u_byteswap, 0, 0, 204602, 2000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_32u_popcntpuppet_32u, volk32u_popcnt_32u,  0, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64f_convert_32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64f_x2_max_64f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64f_x2_min_64f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_64u_byteswappuppet_64u, volk_64u_byteswap, 0, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PUPPET_PROFILE(volk_64u_popcntpuppet_64u, volk_64u_popcnt, 0, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_deinterleave_16i_x2, 0, 0, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_s32f_deinterleave_32f_x2, 1e-4, 100, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_deinterleave_real_16i, 0, 256, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_s32f_deinterleave_real_32f, 1e-4, 100, 204602, 3000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_deinterleave_real_8i, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_x2_multiply_conjugate_16ic, 0, 0, 204602, 400, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8ic_x2_s32f_multiply_conjugate_32fc, 1e-4, 100, 204602, 400, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8i_convert_16i, 0, 0, 204602, 20000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_8i_s32f_convert_32f, 1e-4, 100, 204602, 2000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_32fc_s32fc_multiply_32fc, 1e-4, lv_32fc_t(1.0, 0.5), 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32fc_multiply_32fc, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_multiply_32f, 1e-4, 1.0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_binary_slicer_32i, 0, 1.0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_binary_slicer_8i, 0, 1.0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_tanh_32f, 1e-6, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);

    // Until we can update the config on a kernel by kernel basis
    // do not overwrite volk_config when using a regex.
    if(store_results) {
        char path[1024];
        volk_get_config_path(path);

        const fs::path config_path(path);

        if (not fs::exists(config_path.branch_path()))
        {
            std::cout << "Creating " << config_path.branch_path() << "..." << std::endl;
            fs::create_directories(config_path.branch_path());
        }

        std::cout << "Writing " << config_path << "..." << std::endl;
        std::ofstream config(config_path.string().c_str());
        if(!config.is_open()) { //either we don't have write access or we don't have the dir yet
            std::cout << "Error opening file " << config_path << std::endl;
        }

        config << "\
#this file is generated by volk_profile.\n\
#the function name is followed by the preferred architecture.\n\
";

        BOOST_FOREACH(volk_test_results_t result, results) {
            config << result.config_name << " "
                << result.best_arch_a << " "
                << result.best_arch_u << std::endl;
        }
        config.close();
    }
    else {
        std::cout << "Warning: config not generated" << std::endl;
    }
}
