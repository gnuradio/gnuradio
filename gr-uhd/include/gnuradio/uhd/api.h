/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_UHD_API_H
#define INCLUDED_GR_UHD_API_H

#include <uhd/config.hpp>

#ifdef gnuradio_uhd_EXPORTS
#define GR_UHD_API UHD_EXPORT
#else
#define GR_UHD_API UHD_IMPORT
#endif

#endif /* INCLUDED_GR_UHD_API_H */
