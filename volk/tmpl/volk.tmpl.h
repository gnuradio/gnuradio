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
#include <volk/volk_malloc.h>

#include <stdlib.h>
#include <stdbool.h>

__VOLK_DECL_BEGIN

typedef struct volk_func_desc
{
    const char **impl_names;
    const int *impl_deps;
    const bool *impl_alignment;
    const size_t n_impls;
} volk_func_desc_t;

//! Prints a list of machines available
VOLK_API void volk_list_machines(void);

//! Returns the name of the machine this instance will use
VOLK_API const char* volk_get_machine(void);

//! Get the machine alignment in bytes
VOLK_API size_t volk_get_alignment(void);

/*!
 * The VOLK_OR_PTR macro is a convenience macro
 * for checking the alignment of a set of pointers.
 * Example usage:
 * volk_is_aligned(VOLK_OR_PTR((VOLK_OR_PTR(p0, p1), p2)))
 */
#define VOLK_OR_PTR(ptr0, ptr1) \
    (const void *)(((intptr_t)(ptr0)) | ((intptr_t)(ptr1)))

/*!
 * Is the pointer on a machine alignment boundary?
 *
 * Note: for performance reasons, this function
 * is not usable until another volk API call is made
 * which will perform certain initialization tasks.
 *
 * \param ptr the pointer to some memory buffer
 * \return 1 for alignment boundary, else 0
 */
VOLK_API bool volk_is_aligned(const void *ptr);

#for $kern in $kernels

//! A function pointer to the dispatcher implementation
extern VOLK_API $kern.pname $kern.name;

//! A function pointer to the fastest aligned implementation
extern VOLK_API $kern.pname $(kern.name)_a;

//! A function pointer to the fastest unaligned implementation
extern VOLK_API $kern.pname $(kern.name)_u;

//! Call into a specific implementation given by name
extern VOLK_API void $(kern.name)_manual($kern.arglist_full, const char* impl_name);

//! Get description paramaters for this kernel
extern VOLK_API volk_func_desc_t $(kern.name)_get_func_desc(void);
#end for

__VOLK_DECL_END

#endif /*INCLUDED_VOLK_RUNTIME*/
