/* -*- c++ -*- */
/*
 * Copyright 2002 Free Software Foundation, Inc.
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
#ifndef _CREATE_ATSC_EQUALIZER_H_
#define _CREATE_ATSC_EQUALIZER_H_

#include <atsc_api.h>

class atsci_equalizer;

ATSC_API atsci_equalizer *create_atsci_equalizer_nop ();
ATSC_API atsci_equalizer *create_atsci_equalizer_lms ();
ATSC_API atsci_equalizer *create_atsci_equalizer_lms2 ();

#endif /* _CREATE_ATSC_EQUALIZER_H_ */
