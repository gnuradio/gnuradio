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

#include <volk/volk_common.h>
#include "volk_machines.h"
#include <volk/volk_typedefs.h>
#include <volk/volk_cpu.h>
#include "volk_rank_archs.h"
#include <volk/volk.h>
#include <stdio.h>
#include <string.h>

struct volk_machine *get_machine(void) {
    extern struct volk_machine *volk_machines[];
    extern unsigned int n_volk_machines;
    static struct volk_machine *machine = NULL;
    
    if(machine != NULL) return machine;
    else {
        unsigned int max_score = 0;
        unsigned int i;
        for(i=0; i<n_volk_machines; i++) {
            if(!(volk_machines[i]->caps & (~volk_get_lvarch()))) {
                if(volk_machines[i]->caps > max_score) {
                    max_score = volk_machines[i]->caps;
                    machine = volk_machines[i];
                }
            }
        }
        printf("Using Volk machine: %s\n", machine->name);
        return machine;
    }
}

unsigned int volk_get_alignment(void) {
    return get_machine()->alignment;
}

#for $kern in $kernels

void get_$(kern.name)($kern.arglist_namedefs) {
    $kern.name = get_machine()->$(kern.name)_archs[volk_rank_archs(
        get_machine()->$(kern.name)_indices,
        get_machine()->$(kern.name)_arch_defs,
        get_machine()->$(kern.name)_n_archs,
        get_machine()->$(kern.name)_name,
        volk_get_lvarch()
    )];
    $(kern.name)($kern.arglist_names);
}

$kern.pname $kern.name = &get_$(kern.name);

void $(kern.name)_manual($kern.arglist_namedefs, const char* arch) {
    const size_t index = get_index(
        get_machine()->$(kern.name)_indices,
        get_machine()->$(kern.name)_n_archs,
        arch
    );
    get_machine()->$(kern.name)_archs[index](
        $kern.arglist_names
    );
}

struct volk_func_desc $(kern.name)_get_func_desc(void) {
    struct volk_func_desc desc = {
        get_machine()->$(kern.name)_indices,
        get_machine()->$(kern.name)_arch_defs,
        get_machine()->$(kern.name)_n_archs
    };
    return desc;
}

#end for
