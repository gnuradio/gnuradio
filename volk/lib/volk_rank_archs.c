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

#include <volk_rank_archs.h>
#include <volk/volk_prefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __GNUC__ > 3 || __GNUC__ == 3 && __GNUC_MINOR__ >= 4
    #define __popcnt __builtin_popcount
#elif HAVE_INTRIN_H
    #include <intrin.h> //defines __popcnt
#else
    #error no popcnt for your compiler, add one here
#endif

int volk_get_index(
    const char *impl_names[], //list of implementations by name
    const size_t n_impls,     //number of implementations available
    const char *impl_name     //the implementation name to find
){
    unsigned int i;
    for (i = 0; i < n_impls; i++) {
        if(!strncmp(impl_names[i], impl_name, 20)) {
            return i;
        }
    }
    //TODO return -1;
    //something terrible should happen here
    printf("Volk warning: no arch found, returning generic impl\n");
    return volk_get_index(impl_names, n_impls, "generic"); //but we'll fake it for now
}

int volk_rank_archs(
    const char *kern_name,    //name of the kernel to rank
    const char *impl_names[], //list of implementations by name
    const int* impl_deps,     //requirement mask per implementation
    const bool* alignment,    //alignment status of each implementation
    size_t n_impls,            //number of implementations available
    const bool align          //if false, filter aligned implementations
){
  size_t i;
  static volk_arch_pref_t *volk_arch_prefs;
  static size_t n_arch_prefs = 0;
  static int prefs_loaded = 0;
  if(!prefs_loaded) {
      n_arch_prefs = volk_load_preferences(&volk_arch_prefs);
      prefs_loaded = 1;
  }

    //now look for the function name in the prefs list
    for(i = 0; i < n_arch_prefs; i++)
    {
        if(!strncmp(kern_name, volk_arch_prefs[i].name, sizeof(volk_arch_prefs[i].name))) //found it
        {
            const char *impl_name = align? volk_arch_prefs[i].impl_a : volk_arch_prefs[i].impl_u;
            return volk_get_index(impl_names, n_impls, impl_name);
        }
    }

    //return the best index with the largest deps
    size_t best_index_a = 0;
    size_t best_index_u = 0;
    int best_value_a = -1;
    int best_value_u = -1;
    for(i = 0; i < n_impls; i++)
    {
        const signed val = __popcnt(impl_deps[i]);
        if (alignment[i] && val > best_value_a)
        {
            best_index_a = i;
            best_value_a = val;
        }
        if (!alignment[i] && val > best_value_u)
        {
            best_index_u = i;
            best_value_u = val;
        }
    }

    //when align and we found a best aligned, use it
    if (align && best_value_a != -1) return best_index_a;

    //otherwise return the best unaligned
    return best_index_u;
}
