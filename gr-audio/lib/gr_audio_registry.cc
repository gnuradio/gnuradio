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
#include <stdexcept>
#include <vector>
#include <utility>

/***********************************************************************
 * Create registries
 **********************************************************************/
static std::vector<std::pair<std::string, source_factory_t> > &get_source_registry(void){
    static std::vector<std::pair<std::string, source_factory_t> > _registry;
    return _registry;
}

static std::vector<std::pair<std::string, sink_factory_t> > &get_sink_registry(void){
    static std::vector<std::pair<std::string, sink_factory_t> > _registry;
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
audio_source::sptr audio_make_source(
    int sampling_rate,
    const std::string device_name,
    bool ok_to_block
){
    if (get_source_registry().empty()){
        throw std::runtime_error("no available audio factories");
    }
    //TODO we may prefer to use a specific entry in the registry
    return get_source_registry().front().second(sampling_rate, device_name, ok_to_block);
}

audio_sink::sptr audio_make_sink(
    int sampling_rate,
    const std::string device_name,
    bool ok_to_block
){
    if (get_sink_registry().empty()){
        throw std::runtime_error("no available audio factories");
    }
    //TODO we may prefer to use a specific entry in the registry
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
