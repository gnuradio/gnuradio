/* -*- c++ -*- */
/*
 * Copyright 2008,2009 Free Software Foundation, Inc.
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
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifndef INCLUDED_DOTPROD_FFF_ARMV7_A_H
#define INCLUDED_DOTPROD_FFF_ARMV7_A_H

#include <gr_core_api.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * <pre>
 *
 *  preconditions:
 *
 *    n > 0 and a multiple of 4
 *    a   4-byte aligned
 *    b  16-byte aligned
 *
 * </pre>
 */
float 
dotprod_fff_armv7_a(const float *a, const float *b, size_t n);

#ifdef __cplusplus
}
#endif


#endif /* INCLUDED_DOTPROD_FFF_ARMV7_A_H */
