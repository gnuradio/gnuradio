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


// Wishbone National Semiconductor 16550A compatible UART 

#ifndef INCLUDED_WB16550_H
#define INCLUDED_WB16550_H

#include <stdint.h>

typedef struct {
  volatile uint8_t data;     // 0 r/w: r: rx fifo, w: tx fifo (if DLAB: LSB of divisor)
  volatile uint8_t ier;      // 1 r/w: Interrupt Enable Register (if DLAB: MSB of divisor)
  volatile uint8_t iir_fcr;  // 2 r/w: r: Interrupt ID Register,
                             //        w: Fifo Control Register
  volatile uint8_t lcr;      // 3 r/w: Line Control Register
  volatile uint8_t mcr;      // 4 w:   Modem Control Register
  volatile uint8_t lsr;      // 5 r:   Line Status Register
  volatile uint8_t msr;      // 6 r:   Modem Status Register

} wb16550_reg_t;

#define UART_IER_RDI		0x01  // Enable received data interrupt
#define UART_IER_THRI		0x02  // Enable transmitter holding reg empty int.
#define	UART_IER_RLSI		0x04  // Enable receiver line status interrupt
#define UART_IER_MSI		0x08  // Enable modem status interrupt

#define	UART_IIR_NO_INT		0x01  // No interrupts pending
#define UART_IIR_ID_MASK	0x06  // Mask for interrupt ID
#define	UART_IIR_MSI		0x00  // Modem status interrupt
#define	UART_IIR_THRI		0x02  // Tx holding register empty int
#define	UART_IIR_RDI		0x04  // Rx data available int
#define UART_IIR_RLSI		0x06  // Receiver line status int

#define	UART_FCR_ENABLE_FIFO	0x01  // ignore, always enabled
#define UART_FCR_CLEAR_RCVR	0x02  // Clear the RCVR FIFO
#define UART_FCR_CLEAR_XMIT	0x04  // Clear the XMIT FIFO
#define UART_FCR_TRIGGER_MASK	0xC0  // Mask for FIFO trigger range
#define	UART_FCR_TRIGGER_1	0x00  // Rx fifo trigger level:  1 byte
#define	UART_FCR_TRIGGER_4	0x40  // Rx fifo trigger level:  4 bytes
#define	UART_FCR_TRIGGER_8	0x80  // Rx fifo trigger level:  8 bytes
#define	UART_FCR_TRIGGER_14	0xC0  // Rx fifo trigger level: 14 bytes

#define UART_LCR_DLAB		0x80  // Divisor latch access bit 
#define UART_LCR_SBC		0x40  // Set break control 
#define UART_LCR_SPAR		0x20  // Stick parity
#define UART_LCR_EPAR		0x10  // Even parity select 
#define UART_LCR_PARITY		0x08  // Parity Enable 
#define UART_LCR_STOP		0x04  // Stop bits: 0=1 bit, 1=2 bits 
#define UART_LCR_WLEN5		0x00  // Wordlength: 5 bits 
#define UART_LCR_WLEN6		0x01  // Wordlength: 6 bits 
#define UART_LCR_WLEN7		0x02  // Wordlength: 7 bits 
#define UART_LCR_WLEN8		0x03  // Wordlength: 8 bits 

#define UART_MCR_LOOP		0x10  // Enable loopback test mode 
#define UART_MCR_OUT2n		0x08  // Out2 complement (loopback mode)
#define UART_MCR_OUT1n		0x04  // Out1 complement (loopback mode)
#define UART_MCR_RTSn		0x02  // RTS complement 
#define UART_MCR_DTRn		0x01  // DTR complement 

#define UART_LSR_TEMT		0x40  // Transmitter empty 
#define UART_LSR_THRE		0x20  // Transmit-hold-register empty 
#define UART_LSR_BI		0x10  // Break interrupt indicator 
#define UART_LSR_FE		0x08  // Frame error indicator 
#define UART_LSR_PE		0x04  // Parity error indicator 
#define UART_LSR_OE		0x02  // Overrun error indicator 
#define UART_LSR_DR		0x01  // Receiver data ready 
#define UART_LSR_BRK_ERROR_BITS	0x1E  // BI, FE, PE, OE bits 
#define UART_LSR_ERROR		0x80  // At least 1 PE, FE or BI are in the fifo

#define UART_MSR_DCD		0x80  // Data Carrier Detect 
#define UART_MSR_RI		0x40  // Ring Indicator 
#define UART_MSR_DSR		0x20  // Data Set Ready 
#define UART_MSR_CTS		0x10  // Clear to Send 
#define UART_MSR_DDCD		0x08  // Delta DCD 
#define UART_MSR_TERI		0x04  // Trailing edge ring indicator 
#define UART_MSR_DDSR		0x02  // Delta DSR 
#define UART_MSR_DCTS		0x01  // Delta CTS 
#define UART_MSR_ANY_DELTA	0x0F  // Any of the delta bits! 


#endif  // INCLUDED_WB16550_H 
