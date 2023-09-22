/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_TRELLIS_API_H
#define INCLUDED_TRELLIS_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_trellis_EXPORTS
#define TRELLIS_API __GR_ATTR_EXPORT
#else
#define TRELLIS_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_TRELLIS_API_H */
