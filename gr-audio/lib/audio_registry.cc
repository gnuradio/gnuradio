/*
 * Copyright 2011,2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "audio_registry.h"
#include <gnuradio/logger.h>
#include <gnuradio/prefs.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace gr {
namespace audio {

/***********************************************************************
 * Create registries
 **********************************************************************/

static std::vector<source_entry_t>& get_source_registry(void)
{
    static bool src_reg = false;
    static std::vector<source_entry_t> s_registry;

    if (!src_reg) {
#ifdef ALSA_FOUND
        s_registry.push_back(register_source(REG_PRIO_HIGH, "alsa", alsa_source_fcn));
#endif /* ALSA_FOUND */

#ifdef OSS_FOUND
        s_registry.push_back(register_source(REG_PRIO_LOW, "oss", oss_source_fcn));
#endif /* OSS_FOUND */

#ifdef PORTAUDIO_FOUND
        s_registry.push_back(
            register_source(REG_PRIO_MED, "portaudio", portaudio_source_fcn));
#endif /* PORTAUDIO_FOUND */

#ifdef JACK_FOUND
        s_registry.push_back(register_source(REG_PRIO_MED, "jack", jack_source_fcn));
#endif /* JACK_FOUND */

#ifdef OSX_FOUND
        s_registry.push_back(register_source(REG_PRIO_HIGH, "osx", osx_source_fcn));
#endif /* OSX_FOUND */

#ifdef WIN32_FOUND
        s_registry.push_back(
            register_source(REG_PRIO_LOW, "windows", windows_source_fcn));
#endif /* WIN32_FOUND */

        src_reg = true;
    }


    return s_registry;
}

static std::vector<sink_entry_t>& get_sink_registry(void)
{
    static bool snk_reg = false;
    static std::vector<sink_entry_t> s_registry;

    if (!snk_reg) {
#if ALSA_FOUND
        s_registry.push_back(register_sink(REG_PRIO_HIGH, "alsa", alsa_sink_fcn));
#endif /* ALSA_FOUND */

#if OSS_FOUND
        s_registry.push_back(register_sink(REG_PRIO_LOW, "oss", oss_sink_fcn));
#endif /* OSS_FOUND */

#if PORTAUDIO_FOUND
        s_registry.push_back(
            register_sink(REG_PRIO_MED, "portaudio", portaudio_sink_fcn));
#endif /* PORTAUDIO_FOUND */

#if JACK_FOUND
        s_registry.push_back(register_sink(REG_PRIO_MED, "jack", jack_sink_fcn));
#endif /* JACK_FOUND */

#ifdef OSX_FOUND
        s_registry.push_back(register_sink(REG_PRIO_HIGH, "osx", osx_sink_fcn));
#endif /* OSX_FOUND */

#ifdef WIN32_FOUND
        s_registry.push_back(register_sink(REG_PRIO_HIGH, "windows", windows_sink_fcn));
#endif /* WIN32_FOUND */

        snk_reg = true;
    }

    return s_registry;
}

/***********************************************************************
 * Register functions
 **********************************************************************/
source_entry_t
register_source(reg_prio_type prio, const std::string& arch, source_factory_t source)
{
    source_entry_t entry;
    entry.prio = prio;
    entry.arch = arch;
    entry.source = source;
    return entry;
}

sink_entry_t
register_sink(reg_prio_type prio, const std::string& arch, sink_factory_t sink)
{
    sink_entry_t entry;
    entry.prio = prio;
    entry.arch = arch;
    entry.sink = sink;
    return entry;
}

/***********************************************************************
 * Factory functions
 **********************************************************************/
static std::string default_arch_name(void)
{
    return prefs::singleton()->get_string("audio", "audio_module", "auto");
}

static void do_arch_warning(const std::string& arch)
{
    if (arch == "auto")
        return; // no warning when arch not specified

    gr::logger_ptr logger, debug_logger;
    gr::configure_default_loggers(logger, debug_logger, "audio_registry");
    std::ostringstream msg;
    msg << "Could not find audio architecture \"" << arch << "\" in registry.";
    msg << " Defaulting to the first available architecture.";
    GR_LOG_ERROR(logger, msg.str());
}

source::sptr
source::make(int sampling_rate, const std::string device_name, bool ok_to_block)
{
    gr::logger_ptr logger, debug_logger;
    configure_default_loggers(logger, debug_logger, "audio_source");

    if (get_source_registry().empty()) {
        throw std::runtime_error("no available audio source factories");
    }

    std::string arch = default_arch_name();
    source_entry_t entry = get_source_registry().front();

    for (const auto& e : get_source_registry()) {
        if (e.prio > entry.prio)
            entry = e; // entry is highest prio
        if (arch != e.arch)
            continue; // continue when no match
        return e.source(sampling_rate, device_name, ok_to_block);
    }

    GR_LOG_INFO(debug_logger, boost::format("Audio source arch: %1%") % (entry.arch));
    return entry.source(sampling_rate, device_name, ok_to_block);
}

sink::sptr sink::make(int sampling_rate, const std::string device_name, bool ok_to_block)
{
    gr::logger_ptr logger, debug_logger;
    configure_default_loggers(logger, debug_logger, "audio source");

    if (get_sink_registry().empty()) {
        throw std::runtime_error("no available audio sink factories");
    }

    std::string arch = default_arch_name();
    sink_entry_t entry = get_sink_registry().front();

    for (const sink_entry_t& e : get_sink_registry()) {
        if (e.prio > entry.prio)
            entry = e; // entry is highest prio
        if (arch != e.arch)
            continue; // continue when no match
        return e.sink(sampling_rate, device_name, ok_to_block);
    }

    do_arch_warning(arch);
    GR_LOG_INFO(debug_logger, boost::format("Audio sink arch: %1%") % (entry.arch));
    return entry.sink(sampling_rate, device_name, ok_to_block);
}

} /* namespace audio */
} /* namespace gr */
