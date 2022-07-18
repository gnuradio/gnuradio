/*
 * Copyright 2011 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#pragma once

#include <gnuradio/attributes.h>

#ifdef gnuradio_blocks_EXPORTS
#define BLOCKS_API __GR_ATTR_EXPORT
#else
#define BLOCKS_API __GR_ATTR_IMPORT
#endif
