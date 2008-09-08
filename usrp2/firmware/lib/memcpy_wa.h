/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_MEMCPY_WA_H
#define INCLUDED_MEMCPY_WA_H

#include <stddef.h>

/*
 * For copying to/from non-byte-adressable memory, such as
 * the buffers.  dst, src, and nbytes must all satisfy (x % 4 == 0)
 */
void memcpy_wa(void *dst, const void *src, size_t nbytes);

#endif /* INCLUDED_MEMCPY_WA_H */


