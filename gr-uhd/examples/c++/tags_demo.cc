/*
 * Copyright 2011,2019 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "tag_sink_demo.h"
#include "tag_source_demo.h"
#include <gnuradio/top_block.h>
#include <gnuradio/uhd/usrp_sink.h>
#include <gnuradio/uhd/usrp_source.h>
#include <uhd/utils/safe_main.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

namespace po = boost::program_options;
using namespace std::chrono_literals;

/***********************************************************************
 * Signal handlers
 **********************************************************************/
static bool stop_signal_called = false;
void sig_int_handler(int) { stop_signal_called = true; }

/***********************************************************************
 * Main w/ program options
 **********************************************************************/
int UHD_SAFE_MAIN(int argc, char* argv[])
{

    std::string device_addr, length_tag;
    double center_freq, samp_rate, burst_dur, idle_dur;

    // setup the program options
    po::options_description desc("Allowed options");
    desc.add_options()("help", "help message")(
        "addr",
        po::value<std::string>(&device_addr)->default_value(""),
        "the device address in string format")(
        "rate",
        po::value<double>(&samp_rate)->default_value(1e6),
        "the sample rate in samples per second")(
        "freq",
        po::value<double>(&center_freq)->default_value(10e6),
        "the center frequency in Hz")("burst",
                                      po::value<double>(&burst_dur)->default_value(0.1),
                                      "the duration of each burst in seconds")(
        "idle",
        po::value<double>(&idle_dur)->default_value(0.05),
        "idle time between bursts in seconds")(
        "length_tag",
        po::value<std::string>(&length_tag)->default_value(""),
        "the length tag key name");
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    // print the help message
    if (vm.count("help")) {
        std::cout << boost::format("UHD Tags Demo %s") % desc << std::endl
                  << "The tags sink demo block will print USRP source time stamps."
                  << std::endl
                  << "The tags source demo block will send bursts to the USRP sink."
                  << std::endl
                  << "Look at the USRP output on a scope to see the timed bursts."
                  << std::endl
                  << std::endl;
        return ~0;
    }

    //------------------------------------------------------------------
    //-- make a top block
    //------------------------------------------------------------------
    gr::top_block_sptr tb = gr::make_top_block("tags_demo");

    //------------------------------------------------------------------
    //-- make the usrp source test blocks
    //------------------------------------------------------------------
    gr::uhd::usrp_source::sptr usrp_source =
        gr::uhd::usrp_source::make(device_addr, uhd::stream_args_t("fc32"));
    usrp_source->set_samp_rate(samp_rate);
    usrp_source->set_center_freq(center_freq);

    std::shared_ptr<tag_sink_demo> tag_sink = std::make_shared<tag_sink_demo>();

    //------------------------------------------------------------------
    //-- connect the usrp source test blocks
    //------------------------------------------------------------------
    tb->connect(usrp_source, 0, tag_sink, 0);

    //------------------------------------------------------------------
    //-- make the usrp sink test blocks
    //------------------------------------------------------------------
    gr::uhd::usrp_sink::sptr usrp_sink =
        gr::uhd::usrp_sink::make(device_addr, uhd::stream_args_t("fc32"), length_tag);
    usrp_sink->set_samp_rate(samp_rate);
    usrp_sink->set_center_freq(center_freq);
    const uhd::time_spec_t time_now = usrp_sink->get_time_now();
    const double actual_samp_rate = usrp_sink->get_samp_rate();

    std::shared_ptr<tag_source_demo> tag_source =
        std::make_shared<tag_source_demo>(time_now.get_full_secs() + 1,
                                          time_now.get_frac_secs(), // time now + 1 second
                                          actual_samp_rate,
                                          idle_dur,
                                          burst_dur,
                                          length_tag);

    //------------------------------------------------------------------
    //-- connect the usrp sink test blocks
    //------------------------------------------------------------------
    tb->connect(tag_source, 0, usrp_sink, 0);

    //------------------------------------------------------------------
    //-- start flow graph execution
    //------------------------------------------------------------------
    std::cout << "starting flow graph" << std::endl;
    tb->start();

    //------------------------------------------------------------------
    //-- poll the exit signal while running
    //------------------------------------------------------------------
    std::signal(SIGINT, &sig_int_handler);
    std::cout << "press ctrl + c to exit" << std::endl;
    while (!stop_signal_called) {
        std::this_thread::sleep_for(100ms);
    }

    //------------------------------------------------------------------
    //-- stop flow graph execution
    //------------------------------------------------------------------
    std::cout << "stopping flow graph" << std::endl;
    tb->stop();
    tb->wait();

    std::cout << "done!" << std::endl;
    return 0;
}
