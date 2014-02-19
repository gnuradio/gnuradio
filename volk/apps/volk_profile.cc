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

int main(int argc, char *argv[]) {
    boost::program_options::options_description desc("Options");
    desc.add_options()
      ("help,h", "Print help messages")
      ("benchmark,b",
            boost::program_options::value<bool>()->default_value( false ),
            "Run all kernels (benchmark mode)")
      ("tests-regex,R",
            boost::program_options::value<std::string>(),
            "Run tests matching regular expression.")
      ;
    boost::program_options::variables_map vm;
    bool benchmark_mode;
    std::string kernel_regex;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
        boost::program_options::notify(vm);
        benchmark_mode, kernel_regex = vm.count("benchmark")?vm["benchmark"].as<bool>():false;
        if ( vm.count("tests-regex" ) ) {
            kernel_regex = vm["tests-regex"].as<std::string>();
        }
        else {
            kernel_regex = ".*";
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


    std::vector<std::string> results;

    //VOLK_PROFILE(volk_16i_x5_add_quad_16i_x4, 1e-4, 2046, 10000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_16i_branch_4_state_8, 1e-4, 2046, 10000, &results, benchmark_mode, kernel_regex);
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
    VOLK_PROFILE(volk_16u_byteswap, 0, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_16i_32fc_dot_prod_32fc, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_accumulator_s32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_x2_add_32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_32f_multiply_32fc, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_power_32fc, 1e-4, 0, 204602, 50, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32f_s32f_calc_spectral_noise_floor_32f, 1e-4, 20.0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_s32f_atan2_32f, 1e-4, 10.0, 204602, 100, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_32fc_x2_conjugate_dot_prod_32fc, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
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
    VOLK_PROFILE(volk_32u_byteswap, 0, 0, 204602, 2000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_32u_popcnt, 0, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64f_convert_32f, 1e-4, 0, 204602, 10000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64f_x2_max_64f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64f_x2_min_64f, 1e-4, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    VOLK_PROFILE(volk_64u_byteswap, 0, 0, 204602, 1000, &results, benchmark_mode, kernel_regex);
    //VOLK_PROFILE(volk_64u_popcnt, 0, 0, 2046, 10000, &results, benchmark_mode, kernel_regex);
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

    BOOST_FOREACH(std::string result, results) {
        config << result << std::endl;
    }
    config.close();
}
