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
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <vector>

namespace gr {
namespace audio {
namespace details {
static const std::unordered_map<std::string, reg_prio_type> arch_prios{
    /* good native architectures */
    { "alsa", REG_PRIO_HIGH },
    { "pulse", REG_PRIO_HIGH },
    { "osx", REG_PRIO_HIGH },
    /* all-purpose cross-platform architectures */
    { "portaudio", REG_PRIO_MED },
    { "jack", REG_PRIO_MED },
    /* old / little-used / bad / worse platform-specific architectures */
    { "oss", REG_PRIO_LOW },
    { "windows", REG_PRIO_LOW }
};
} // namespace details

/***********************************************************************
 * Create registries
 **********************************************************************/

static std::vector<source_entry_t>& get_source_registry(void)
{
    static std::vector<source_entry_t> s_registry = {
#ifdef PULSE_FOUND
        { details::arch_prios.at("pulse"), "pulse", pulse_source_fcn },
#endif /* PULSE_FOUND */

#ifdef ALSA_FOUND
        { details::arch_prios.at("alsa"), "alsa", alsa_source_fcn },
#endif /* ALSA_FOUND */

#ifdef OSS_FOUND
        { details::arch_prios.at("oss"), "oss", oss_source_fcn },
#endif /* OSS_FOUND */

#ifdef PORTAUDIO_FOUND
        { details::arch_prios.at("portaudio"), "portaudio", portaudio_source_fcn },
#endif /* PORTAUDIO_FOUND */

#ifdef JACK_FOUND
        { details::arch_prios.at("jack"), "jack", jack_source_fcn },
#endif /* JACK_FOUND */

#ifdef OSX_FOUND
        { details::arch_prios.at("osx"), "osx", osx_source_fcn },
#endif /* OSX_FOUND */

#ifdef WIN32_FOUND
        { details::arch_prios.at("windows"), "windows", windows_source_fcn },
#endif /* WIN32_FOUND */
    };

    return s_registry;
} // namespace audio

static std::vector<sink_entry_t>& get_sink_registry(void)
{
    static std::vector<sink_entry_t> s_registry = {
#ifdef PULSE_FOUND
        { details::arch_prios.at("pulse"), "pulse", pulse_sink_fcn },
#endif /* PULSE_FOUND */

#ifdef ALSA_FOUND
        { details::arch_prios.at("alsa"), "alsa", alsa_sink_fcn },
#endif /* ALSA_FOUND */

#ifdef OSS_FOUND
        { details::arch_prios.at("oss"), "oss", oss_sink_fcn },
#endif /* OSS_FOUND */

#ifdef PORTAUDIO_FOUND
        { details::arch_prios.at("portaudio"), "portaudio", portaudio_sink_fcn },
#endif /* PORTAUDIO_FOUND */

#ifdef JACK_FOUND
        { details::arch_prios.at("jack"), "jack", jack_sink_fcn },
#endif /* JACK_FOUND */

#ifdef OSX_FOUND
        { details::arch_prios.at("osx"), "osx", osx_sink_fcn },
#endif /* OSX_FOUND */

#ifdef WIN32_FOUND
        { details::arch_prios.at("windows"), "windows", windows_sink_fcn },
#endif /* WIN32_FOUND */
    };

    return s_registry;
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
    logger->error("Could not find audio architecture \"{:s}\" in registry."
                  " Defaulting to the first available architecture.",
                  arch);
}

source::sptr
source::make(int sampling_rate, const std::string& device_name, bool ok_to_block, const std::map<std::string, std::string>& properties)
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
        return e.source(sampling_rate, device_name, ok_to_block, properties);
    }

    debug_logger->info("Audio source arch: {:s}", entry.arch);
    return entry.source(sampling_rate, device_name, ok_to_block, properties);
}

sink::sptr sink::make(int sampling_rate, const std::string& device_name, bool ok_to_block, const std::map<std::string, std::string>& properties)
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
        return e.sink(sampling_rate, device_name, ok_to_block, properties);
    }

    do_arch_warning(arch);
    debug_logger->info("Audio sink arch: {:s}", entry.arch);
    return entry.sink(sampling_rate, device_name, ok_to_block, properties);
}

} // namespace audio
} /* namespace gr */
