/*
 * Copyright 2010 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_QTGUI_API_H
#define INCLUDED_QTGUI_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_qtgui_EXPORTS
#define QTGUI_API __GR_ATTR_EXPORT
#else
#define QTGUI_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_QTGUI_API_H */
