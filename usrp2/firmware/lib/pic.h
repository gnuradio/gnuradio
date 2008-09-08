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

#ifndef INCLUDED_PIC_H
#define INCLUDED_PIC_H

typedef void (*irq_handler_t)(unsigned irq);

void pic_init(void);
void pic_register_handler(unsigned irq, irq_handler_t handler);

void nop_handler(unsigned irq);	// default handler does nothing

// FIXME inline assembler
int  pic_disable_interrupts();
int  pic_enable_interrupts();
void pic_restore_interrupts(int prev_status);


#endif /* INCLUDED_PIC_H */
