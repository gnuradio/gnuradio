/*
 * Copyright 2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
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

#include "pageri_flex_modes.h"

const flex_mode_t flex_modes[] = 
{
    { 0x870C78F3, 1600, 2, true, false, false, false, 1 },
    { 0xB0684F97, 1600, 4, true, true,  false, false, 2 },
//  { 0xUNKNOWN,  3200, 2, true, false, true,  false, 2 },
    { 0xDEA0215F, 3200, 4, true, true,  true,  true,  4 },
    { 0x4C7CB383, 3200, 4, true, true,  true,  true,  4 }
};

const int num_flex_modes = sizeof(flex_modes);
