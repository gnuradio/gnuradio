/* -*- c++ -*- */
/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio.
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

#ifndef INCLUDED_AUDIO_OSX_H
#define INCLUDED_AUDIO_OSX_H

#include <iostream>
#include <string.h>

#define CheckErrorAndThrow(err,what,throw_str)				\
  if (err) {								\
    OSStatus error = static_cast<OSStatus>(err);			\
    char err_str[4];							\
    strncpy (err_str, (char*)(&err), 4);				\
    std::cerr << what << std::endl;					\
    std::cerr << "  Error# " << error << " ('" << err_str		\
	      << "')" << std::endl;					\
    std::cerr << "  " << __FILE__ << ":" << __LINE__ << std::endl;	\
    fflush (stderr);							\
    throw std::runtime_error (throw_str);				\
  }

#define CheckError(err,what)						\
  if (err) {								\
    OSStatus error = static_cast<OSStatus>(err);			\
    char err_str[4];							\
    strncpy (err_str, (char*)(&err), 4);				\
    std::cerr << what << std::endl;					\
    std::cerr << "  Error# " << error << " ('" << err_str		\
	      << "')" << std::endl;					\
    std::cerr << "  " << __FILE__ << ":" << __LINE__ << std::endl;	\
    fflush (stderr);							\
  }

#ifdef WORDS_BIGENDIAN
#define GR_PCM_ENDIANNESS kLinearPCMFormatFlagIsBigEndian
#else
#define GR_PCM_ENDIANNESS 0
#endif

// Check the version of MacOSX being used
#ifdef __APPLE_CC__
#include <AvailabilityMacros.h>
#ifndef MAC_OS_X_VERSION_10_6
#define MAC_OS_X_VERSION_10_6 1060
#endif
#if MAC_OS_X_VERSION_MAX_ALLOWED < MAC_OS_X_VERSION_10_6
#define GR_USE_OLD_AUDIO_UNIT
#endif
#endif

#endif /* INCLUDED_AUDIO_OSX_H */
