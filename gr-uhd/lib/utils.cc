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

#include "utils.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/foreach.hpp>

static std::string trim(const std::string &in){
    return boost::algorithm::trim_copy(in);
}

uhd::device_addr_t args_to_device_addr(const std::string &args){
    uhd::device_addr_t addr;

    //split the args at the semi-colons
    std::vector<std::string> pairs;
    boost::split(pairs, args, boost::is_any_of(";"));
    BOOST_FOREACH(std::string pair, pairs){
        if (trim(pair) == "") continue;

        //split the key value pairs at the equals
        std::vector<std::string> key_val;
        boost::split(key_val, pair, boost::is_any_of("="));
        if (key_val.size() != 2) throw std::runtime_error("invalid args string: "+args);
        addr[trim(key_val[0])] = trim(key_val[1]);
    }

    return addr;
}
