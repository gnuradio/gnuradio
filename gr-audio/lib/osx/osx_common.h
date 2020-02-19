/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_AUDIO_OSX_COMMON_H
#define INCLUDED_AUDIO_OSX_COMMON_H

#include <gnuradio/audio/osx_impl.h>

namespace gr {
namespace audio {
namespace osx {

#ifndef _OSX_AU_DEBUG_
#define _OSX_AU_DEBUG_ 0
#endif

#ifndef _OSX_AU_DEBUG_RENDER_
#define _OSX_AU_DEBUG_RENDER_ 0
#endif

#define check_error_and_throw(err, what, throw_str)                                    \
    if (err) {                                                                         \
        OSStatus error = static_cast<OSStatus>(err);                                   \
        char err_str[sizeof(OSStatus) + 1];                                            \
        memcpy((void*)(&err_str), (void*)(&error), sizeof(OSStatus));                  \
        err_str[sizeof(OSStatus)] = 0;                                                 \
        GR_LOG_FATAL(d_logger, boost::format(what));                                   \
        GR_LOG_FATAL(d_logger, boost::format("  Error# %u ('%s')") % error % err_str); \
        GR_LOG_FATAL(d_logger, boost::format("  %s:%d") % __FILE__ % __LINE__);        \
        throw std::runtime_error(throw_str);                                           \
    }

#define check_error(err, what)                                                        \
    if (err) {                                                                        \
        OSStatus error = static_cast<OSStatus>(err);                                  \
        char err_str[sizeof(OSStatus) + 1];                                           \
        memcpy((void*)(&err_str), (void*)(&error), sizeof(OSStatus));                 \
        err_str[sizeof(OSStatus)] = 0;                                                \
        GR_LOG_WARN(d_logger, boost::format(what));                                   \
        GR_LOG_WARN(d_logger, boost::format("  Error# %u ('%s')") % error % err_str); \
        GR_LOG_WARN(d_logger, boost::format("  %s:%d") % __FILE__ % __LINE__);        \
    }

#include <boost/detail/endian.hpp> //BOOST_BIG_ENDIAN
#ifdef BOOST_BIG_ENDIAN
#define GR_PCM_ENDIANNESS kLinearPCMFormatFlagIsBigEndian
#else
#define GR_PCM_ENDIANNESS 0
#endif

} /* namespace osx */
} /* namespace audio */
} /* namespace gr */

#endif /* INCLUDED_AUDIO_OSX_COMMON_H */
