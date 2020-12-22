/*
 * Copyright 2020 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_MESSAGE_API_H
#define INCLUDED_MESSAGE_API_H

#include <gnuradio/attributes.h>

#ifdef gnuradio_message_EXPORTS
#define MESSAGE_API __GR_ATTR_EXPORT
#else
#define MESSAGE_API __GR_ATTR_IMPORT
#endif

#endif /* INCLUDED_MESSAGE_API_H */
