/* -*- c++ -*- */
/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_NETWORK_API_H
#define INCLUDED_NETWORK_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_network_EXPORTS
#define NETWORK_API __GR_ATTR_EXPORT
#else
#define NETWORK_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_NETWORK_API_H */
