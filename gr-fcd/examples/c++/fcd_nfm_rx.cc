/*
 * Copyright 2011 Free Software Foundation, Inc.
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

/*
 * Simple GNU Radio C++ example for using the Funcube Dongle.
 *
 * Construct a simple narrow band FM receiver using the gr-fcd
 * block as source.
 *
 * This example uses "hw:1" as FCD source device and the default
 * audio device for output.
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// Include header files for each block used in flowgraph
#include <gnuradio/top_block.h>
#include <gnuradio/filter/firdes.h>
#include <gnuradio/filter/fir_filter_ccf.h>
#include <gnuradio/analog/quadrature_demod_cf.h>
#include <gnuradio/audio/sink.h>
#include <gnuradio/fcd/source_c.h>

// other includes
#include <iostream>
#include <boost/program_options.hpp>

using namespace gr;
namespace po = boost::program_options;

int main(int argc, char **argv)
{
    int rate = 48000;		// Audio card sample rate
    float pi = 3.141592654;


    //variables to be set by po
    std::string device;
    int freq;
    float gain;

    //setup the program options
    po::options_description desc("Command line options");
    desc.add_options()
        ("help", "This help message")
        ("device", po::value<std::string>(&device)->default_value("hw:1"), "Audio input device")
        ("freq", po::value<int>(&freq)->default_value(145500), "RF frequency in kHz")
        ("gain", po::value<float>(&gain)->default_value(20.0), "LNA gain in dB")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    //print the help message
    if (vm.count("help")){
        std::cout << "Narrow band FM receiver example" << std::endl << desc << std::endl;
        return ~0;
    }


    // Construct a top block that will contain flowgraph blocks.
    top_block_sptr tb = make_top_block("fcd_nfm_rx");

    // FCD source
    gr::fcd::source_c::sptr fcd = gr::fcd::source_c::make(device);
    fcd->set_freq_khz(freq);
    fcd->set_lna_gain(gain);

    // Low pass filter
    std::vector<float> taps = filter::firdes::low_pass(1.0, 96000, 5000.0, 1000.0);
    filter::fir_filter_ccf::sptr filter = filter::fir_filter_ccf::make (2, taps);

    // FM demodulator
    // gain = sample_rate / (2*pi*max_dev)
    analog::quadrature_demod_cf::sptr demod = \
      analog::quadrature_demod_cf::make(rate/(2.0*pi*5000.0));

    // Audio sink
    audio::sink::sptr sink = audio::sink::make(rate);

    // Connect blocks
    tb->connect(fcd, 0, filter, 0);
    tb->connect(filter, 0, demod, 0);
    tb->connect(demod, 0, sink, 0);

    // Tell GNU Radio runtime to start flowgraph threads; the foreground thread
    // will block until either flowgraph exits (this example doesn't) or the
    // application receives SIGINT (e.g., user hits CTRL-C).
    //
    // Real applications may use tb->start() which returns, allowing the foreground
    // thread to proceed, then later use tb->stop(), followed by tb->wait(), to cleanup
    // GNU Radio before exiting.
    tb->run();

    // Exit normally.
    return 0;
}
