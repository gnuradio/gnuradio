;;; -*- asm -*-
;;;
;;; Copyright 2003 Free Software Foundation, Inc.
;;; 
;;; This file is part of GNU Radio
;;; 
;;; GNU Radio is free software; you can redistribute it and/or modify
;;; it under the terms of the GNU General Public License as published by
;;; the Free Software Foundation; either version 3, or (at your option)
;;; any later version.
;;; 
;;; GNU Radio is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;; 
;;; You should have received a copy of the GNU General Public License
;;; along with GNU Radio; see the file COPYING.  If not, write to
;;; the Free Software Foundation, Inc., 51 Franklin Street,
;;; Boston, MA 02110-1301, USA.
;;; 

;;; Interrupt vectors.

;;; N.B. This object module must come first in the list of modules

	.module vectors

;;; ----------------------------------------------------------------
;;;		  standard FX2 interrupt vectors
;;; ----------------------------------------------------------------

	.area CSEG (CODE)
	.area GSINIT (CODE)
	.area CSEG (CODE)
__standard_interrupt_vector::
__reset_vector::
	ljmp	s_GSINIT
	
	;; 13 8-byte entries.  We point them all at __isr_nop
	ljmp	__isr_nop	; 3 bytes
	.ds	5		; + 5 = 8 bytes for vector slot
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5
	ljmp	__isr_nop
	.ds	5

__isr_nop::
	reti

;;; ----------------------------------------------------------------
;;; the FIFO/GPIF autovector.  14 4-byte entries.
;;; must start on a 128 byte boundary.
;;; ----------------------------------------------------------------
	
	. = __reset_vector + 0x0080
		
__fifo_gpif_autovector::
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	
	ljmp	__isr_nop
	nop	

	
;;; ----------------------------------------------------------------
;;; the USB autovector.  32 4-byte entries.
;;; must start on a 256 byte boundary.
;;; ----------------------------------------------------------------

	. = __reset_vector + 0x0100
	
__usb_autovector::
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
	ljmp	__isr_nop
	nop
