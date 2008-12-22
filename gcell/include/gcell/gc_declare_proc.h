/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
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
#ifndef INCLUDED_GCELL_GC_DECLARE_PROC_H
#define INCLUDED_GCELL_GC_DECLARE_PROC_H

#include <stdint.h>
#include <gcell/gc_job_desc.h>

/*
 * This is C, not C++ code...
 *
 * ...and is used by both PPE and SPE code
 */
__GC_BEGIN_DECLS

#define GC_PROC_DEF_SECTION ".gcell.proc_def"

typedef struct gc_proc_def {
#if defined(__SPU__)
  gc_spu_proc_t	proc;
#else
  uint32_t	proc;
#endif
  char		name[28];
} _AL16 gc_proc_def_t;


#if defined(__SPU__)
/*!
 * \brief Tell gcell about a SPU procedure
 *
 * \param _proc_   pointer to function (gc_spu_proc_t)
 * \param _name_   the name of the procedure ("quoted string")
 *
 * This macro registers the given procedure with the gcell runtime.
 * From the PPE, use gc_job_manager::lookup_proc to map \p _name_ to a gc_proc_id_t
 */
#define GC_DECLARE_PROC(_proc_, _name_) \
static struct gc_proc_def \
  _GCPD_ ## _proc_ __attribute__((section(GC_PROC_DEF_SECTION), used)) = \
  { _proc_, _name_ }
#endif

__GC_END_DECLS

#endif /* INCLUDED_GCELL_GC_DECLARE_PROC_H */
