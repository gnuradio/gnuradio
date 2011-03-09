/*
 * Copyright 2011 Free Software Foundation, Inc.
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

#include "gr_audio_registry.h"
#include <boost/foreach.hpp>
#include <gr_prefs.h>
#include <stdexcept>
#include <vector>
#include <utility>
#include <iostream>

/***********************************************************************
 * Create registries
 **********************************************************************/
typedef std::pair<std::string, source_factory_t> source_pair_t;
static std::vector<source_pair_t> &get_source_registry(void){
    static std::vector<source_pair_t> _registry;
    return _registry;
}

typedef std::pair<std::string, sink_factory_t> sink_pair_t;
static std::vector<sink_pair_t> &get_sink_registry(void){
    static std::vector<sink_pair_t> _registry;
    return _registry;
}

/***********************************************************************
 * Register functions
 **********************************************************************/
void audio_register_source(const std::string &name, source_factory_t source){
    get_source_registry().push_back(std::make_pair(name, source));
}

void audio_register_sink(const std::string &name, sink_factory_t sink){
    get_sink_registry().push_back(std::make_pair(name, sink));
}

/***********************************************************************
 * Factory functions
 **********************************************************************/
static std::string default_arch_name(void){
    return gr_prefs::singleton()->get_string("audio", "default_arch", "");
}

static void do_arch_warning(const std::string &arch){
    std::cerr << "Could not find audio architecture \"" << arch << "\" in registry." << std::endl;
    std::cerr << "    Defaulting to the first available architecture..." << std::endl;
}

audio_source::sptr audio_make_source(
    int sampling_rate,
    const std::string device_name,
    bool ok_to_block
){
    if (get_source_registry().empty()){
        throw std::runtime_error("no available audio source factories");
    }
    std::string arch = default_arch_name();
    BOOST_FOREACH(const source_pair_t &e, get_source_registry()){
        if (arch.empty() || arch == e.first){
            return e.second(sampling_rate, device_name, ok_to_block);
        }
    }
    do_arch_warning(arch);
    return get_source_registry().front().second(sampling_rate, device_name, ok_to_block);
}

audio_sink::sptr audio_make_sink(
    int sampling_rate,
    const std::string device_name,
    bool ok_to_block
){
    if (get_sink_registry().empty()){
        throw std::runtime_error("no available audio sink factories");
    }
    std::string arch = default_arch_name();
    BOOST_FOREACH(const sink_pair_t &e, get_sink_registry()){
        if (arch.empty() || arch == e.first){
            return e.second(sampling_rate, device_name, ok_to_block);
        }
    }
    do_arch_warning(arch);
    return get_sink_registry().front().second(sampling_rate, device_name, ok_to_block);
}

/***********************************************************************
 * Default constructors
 **********************************************************************/
#include <gr_io_signature.h>

audio_sink::audio_sink(
        const std::string &name,
        gr_io_signature_sptr insig,
        gr_io_signature_sptr outsig
):
    gr_sync_block(name, insig, outsig)
{}

audio_source::audio_source(
        const std::string &name,
        gr_io_signature_sptr insig,
        gr_io_signature_sptr outsig
):
    gr_sync_block(name, insig, outsig)
{}
