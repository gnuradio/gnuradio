/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/fec/generic_encoder.h>
#include <gnuradio/prefs.h>
#include <cstdio>

namespace gr {
namespace fec {

generic_encoder::generic_encoder(std::string name)
{
    d_name = name;
    my_id = base_unique_id++;

    prefs* p = prefs::singleton();
    std::string config_file = p->get_string("LOG", "log_config", "");
    std::string log_level = p->get_string("LOG", "log_level", "off");
    std::string log_file = p->get_string("LOG", "log_file", "");

    GR_CONFIG_LOGGER(config_file);

    GR_LOG_GETLOGGER(LOG, "gr_log." + alias());
    GR_LOG_SET_LEVEL(LOG, log_level);
    if (!log_file.empty()) {
        if (log_file == "stdout") {
            GR_LOG_SET_CONSOLE_APPENDER(LOG, "stdout", "gr::log :%p: %c{1} - %m%n");
        } else if (log_file == "stderr") {
            GR_LOG_SET_CONSOLE_APPENDER(LOG, "stderr", "gr::log :%p: %c{1} - %m%n");
        } else {
            GR_LOG_SET_FILE_APPENDER(LOG, log_file, true, "%r :%p: %c{1} - %m%n");
        }
    }

    d_logger = LOG;
}

generic_encoder::~generic_encoder() {}

const char* generic_encoder::get_input_conversion() { return "none"; }

const char* generic_encoder::get_output_conversion() { return "none"; }

int generic_encoder::base_unique_id = 1;
int generic_encoder::unique_id() { return my_id; }

/*******************************************************
 * Static functions
 ******************************************************/
int get_encoder_output_size(generic_encoder::sptr my_encoder)
{
    return my_encoder->get_output_size();
}

int get_encoder_input_size(generic_encoder::sptr my_encoder)
{
    return my_encoder->get_input_size();
}

const char* get_encoder_input_conversion(generic_encoder::sptr my_encoder)
{
    return my_encoder->get_input_conversion();
}

const char* get_encoder_output_conversion(generic_encoder::sptr my_encoder)
{
    return my_encoder->get_output_conversion();
}

} /* namespace fec */
} /* namespace gr */
