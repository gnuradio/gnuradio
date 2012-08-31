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

#ifndef INCLUDED_LIBVOLK_MACHINES_H
#define INCLUDED_LIBVOLK_MACHINES_H

#include <volk/volk_common.h>
#include <volk/volk_typedefs.h>

#include <stdbool.h>
#include <stdlib.h>

__VOLK_DECL_BEGIN

struct volk_machine {
    const unsigned int caps; //capabilities (i.e., archs compiled into this machine, in the volk_get_lvarch format)
    const char *name;
    const size_t alignment; //the maximum byte alignment required for functions in this library
    #for $kern in $kernels
    const char *$(kern.name)_name;
    const char *$(kern.name)_impl_names[$(len($archs))];
    const int $(kern.name)_impl_deps[$(len($archs))];
    const bool $(kern.name)_impl_alignment[$(len($archs))];
    const $(kern.pname) $(kern.name)_impls[$(len($archs))];
    const size_t $(kern.name)_n_impls;
    #end for
};

#for $machine in $machines
#ifdef LV_MACHINE_$(machine.name.upper())
extern struct volk_machine volk_machine_$(machine.name);
#endif
#end for

__VOLK_DECL_END

#endif //INCLUDED_LIBVOLK_MACHINES_H
