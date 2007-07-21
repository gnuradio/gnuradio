/* -*- c++ -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <mb_gettid.h>

#define NEED_STUB

#if defined(HAVE_SYS_SYSCALL_H) && defined(HAVE_UNISTD_H) 

#include <sys/syscall.h>
#include <unistd.h>

#if defined(SYS_gettid)
#undef NEED_STUB

int mb_gettid()
{
  return syscall(SYS_gettid);
}

#endif
#endif

#if defined(NEED_STUB)

int
mb_gettid()
{
  return 0;
}

#endif
