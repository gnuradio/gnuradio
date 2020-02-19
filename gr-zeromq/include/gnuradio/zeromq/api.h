/*
 * Copyright 2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_ZEROMQ_API_H
#define INCLUDED_ZEROMQ_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_zeromq_EXPORTS
#define ZEROMQ_API __GR_ATTR_EXPORT
#else
#define ZEROMQ_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_ZEROMQ_API_H */
