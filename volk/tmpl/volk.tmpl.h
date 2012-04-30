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

#ifndef INCLUDED_VOLK_RUNTIME
#define INCLUDED_VOLK_RUNTIME

#include <volk/volk_typedefs.h>
#include <volk/volk_config_fixed.h>
#include <volk/volk_common.h>
#include <volk/volk_complex.h>

__VOLK_DECL_BEGIN

struct volk_func_desc {
    const char **indices;
    const int *arch_defs;
    const int n_archs;
};

VOLK_API unsigned int volk_get_alignment(void);

#for $kern in $kernels
extern VOLK_API $kern.pname $kern.name;
extern VOLK_API void $(kern.name)_manual($kern.arglist_namedefs, const char* arch);
extern VOLK_API struct volk_func_desc $(kern.name)_get_func_desc(void);
#end for

__VOLK_DECL_END

#endif /*INCLUDED_VOLK_RUNTIME*/
