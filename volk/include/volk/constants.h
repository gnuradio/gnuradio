/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_VOLK_CONSTANTS_H
#define INCLUDED_VOLK_CONSTANTS_H

#include <volk/volk_common.h>

__VOLK_DECL_BEGIN

VOLK_API char* volk_prefix();
VOLK_API char* volk_build_date();
VOLK_API char* volk_version();
VOLK_API char* volk_c_compiler();
VOLK_API char* volk_compiler_flags();
VOLK_API char* volk_available_machines();

__VOLK_DECL_END

#endif /* INCLUDED_VOLK_CONSTANTS_H */
