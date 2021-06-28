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

namespace gr {
namespace fec {

generic_encoder::generic_encoder(std::string name)
{
    d_name = name;
    my_id = base_unique_id++;

    GR_LOG_ASSIGN_CONFIGURED_LOGPTR(d_logger, alias());
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
