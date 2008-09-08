/* -*- c -*- */
/*
 * Copyright 2007 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef INCLUDED_NONSTDIO_H
#define INCLUDED_NONSTDIO_H

#include <stdio.h>
#include <usrp2_types.h>
#include <stddef.h>

void putstr(const char *s);		// cf puts, no added newline
void puthex4(unsigned long x);		// output 1 hex digit
void puthex8(unsigned long x);		// output 2 hex digits
void puthex16(unsigned long x);		// output 4 hex digits
void puthex32(unsigned long x);		// output 8 hex digits
void puthex4_nl(unsigned long x);	// ... followed by newline
void puthex8_nl(unsigned long x);
void puthex16_nl(unsigned long x);
void puthex32_nl(unsigned long x);
#define puthex puthex32
#define puthex_nl puthex32_nl
void newline();				// putchar('\n')

void print_mac_addr(const unsigned char addr[6]);
void print_fxpt_freq(u2_fxpt_freq_t v);
void print_fxpt_gain(u2_fxpt_gain_t v);
void print_uint64(uint64_t v);

void print_buffer(uint32_t *buf, size_t n);

#endif /* INCLUDED_NONSTDIO_H */
