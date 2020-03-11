/*
 * Copyright 2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DTV_API_H
#define INCLUDED_DTV_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_dtv_EXPORTS
#define DTV_API __GR_ATTR_EXPORT
#else
#define DTV_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_DTV_API_H */
