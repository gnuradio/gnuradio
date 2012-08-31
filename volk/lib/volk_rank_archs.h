/*
 * Copyright 2011-2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_VOLK_RANK_ARCHS_H
#define INCLUDED_VOLK_RANK_ARCHS_H

#include <stdlib.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

int volk_get_index(
    const char *impl_names[], //list of implementations by name
    const size_t n_impls,     //number of implementations available
    const char *impl_name     //the implementation name to find
);

int volk_rank_archs(
    const char *kern_name,    //name of the kernel to rank
    const char *impl_names[], //list of implementations by name
    const int* impl_deps,     //requirement mask per implementation
    const bool* alignment,    //alignment status of each implementation
    size_t n_impls,            //number of implementations available
    const bool align          //if false, filter aligned implementations
);

#ifdef __cplusplus
}
#endif
#endif /*INCLUDED_VOLK_RANK_ARCHS_H*/
