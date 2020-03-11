/*
 * Copyright 2010,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_PMT_API_H
#define INCLUDED_PMT_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_pmt_EXPORTS
#define PMT_API __GR_ATTR_EXPORT
#else
#define PMT_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_PMT_API_H */
