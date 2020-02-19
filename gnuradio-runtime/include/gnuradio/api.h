/*
 * Copyright 2010-2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_GR_RUNTIME_RUNTIME_API_H
#define INCLUDED_GR_RUNTIME_RUNTIME_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_runtime_EXPORTS
#define GR_RUNTIME_API __GR_ATTR_EXPORT
#else
#define GR_RUNTIME_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_GR_RUNTIME_RUNTIME_API_H */
