/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_FILTER_API_H
#define INCLUDED_FILTER_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_filter_EXPORTS
#define FILTER_API __GR_ATTR_EXPORT
#else
#define FILTER_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_FILTER_API_H */
