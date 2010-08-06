/* -*- c++ -*- */
/*
 * Copyright 2010 Free Software Foundation, Inc.
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

#include "utils.h" //local include
#include <uhd/utils/warning.hpp>
#include <boost/format.hpp>
#include <cmath>

void do_samp_rate_error_message(
    double target_rate,
    double actual_rate,
    const std::string &xx
){
    static const double max_allowed_error = 1.0; //Sps
    if (std::abs(target_rate - actual_rate) > max_allowed_error){
        uhd::print_warning(str(boost::format(
            "The hardware does not support the requested %s sample rate:\n"
            "Target sample rate: %f MSps\n"
            "Actual sample rate: %f MSps\n"
        ) % xx % (target_rate/1e6) % (actual_rate/1e6)));
    }
}

void do_tune_freq_error_message(
    double target_freq,
    double actual_freq,
    const std::string &xx
){
    static const double max_allowed_error = 1.0; //Hz
    if (std::abs(target_freq - actual_freq) > max_allowed_error){
        uhd::print_warning(str(boost::format(
            "The hardware does not support the requested %s frequency:\n"
            "Target frequency: %f MHz\n"
            "Actual frequency: %f MHz\n"
        ) % xx % (target_freq/1e6) % (actual_freq/1e6)));
    }
}
