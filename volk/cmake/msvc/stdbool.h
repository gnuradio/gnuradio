/*
 * Copyright (C) 2005, 2006 Apple Computer, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef STDBOOL_WIN32_H
#define STDBOOL_WIN32_H

#ifndef _MSC_VER // [
#error "Use this header only with Microsoft Visual C++ compilers!"
#endif // _MSC_VER ]

#ifndef __cplusplus

typedef unsigned char bool;

#define true 1
#define false 0

#ifndef CASSERT
#define CASSERT(exp, name) typedef int dummy##name [(exp) ? 1 : -1];
#endif

CASSERT(sizeof(bool) == 1, bool_is_one_byte)
CASSERT(true, true_is_true)
CASSERT(!false, false_is_false)

#endif

#endif
