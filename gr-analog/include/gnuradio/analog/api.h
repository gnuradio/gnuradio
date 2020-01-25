/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ANALOG_API_H
#define INCLUDED_ANALOG_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_analog_EXPORTS
#define ANALOG_API __GR_ATTR_EXPORT
#else
#define ANALOG_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_ANALOG_API_H */
