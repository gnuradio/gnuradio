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
#ifndef INCLUDED_GC_PROC_DEF_UTILS_H
#define INCLUDED_GC_PROC_DEF_UTILS_H

#include <gc_declare_proc.h>
#include <libspe2.h>

/*!
 * \brief find the gc_proc_def table in the SPE program
 *
 * \param[in]  program is the handle to the loaded SPE program
 * \param[out] table points to the table, if it's found
 * \param[out] nentries is set to the number of entries in the table.
 * \param[out] ls_addr is set to the Local Store address of the table
 *
 * \returns true if successful, else false
 */
bool
gcpd_find_table(spe_program_handle_t *program,
		struct gc_proc_def **table, int *nentries, uint32_t *ls_addr);


#endif /* INCLUDED_GC_PROC_DEF_UTILS_H */
