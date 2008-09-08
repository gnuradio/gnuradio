/* $Id: libaemb.h,v 1.3 2007/12/16 03:26:37 sybreon Exp $
**
** AEMB2 CUSTOM LIBRARY
** 
** Copyright (C) 2004-2007 Shawn Tan Ser Ngiap <shawn.tan@aeste.net>
**  
** This file is part of AEMB.
**
** AEMB is free software: you can redistribute it and/or modify it
** under the terms of the GNU Lesser General Public License as
** published by the Free Software Foundation, either version 3 of the
** License, or (at your option) any later version.
**
** AEMB is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General
** Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with AEMB. If not, see <http:**www.gnu.org/licenses/>.
*/

#ifndef LIBAEMB_H
#define LIBAEMB_H

#define AEMB_TXE 0x0100
#define AEMB_TXP 0x0400
#define AEMB_TX0 0x0300
#define AEMB_TX1 0x0700
#define AEMB_MSK 0x0F00

#define AEMB_BIP 0x0008
#define AEMB_CCC 0x0004
#define AEMB_ITE 0x0002
#define AEMB_BLE 0x0001


// Linker symbols
extern void* _STACK_SIZE;
extern void* _stack_end;


void aemb_hook_init () asm ("_program_init");
void aemb_hook_clean () asm ("_program_clean");

inline void aemb_enable_interrupt ();
inline void aemb_disable_interrupt ();
inline int aemb_isthread1();
inline int aemb_isthread0();
inline int aemb_isthreaded();

/*!
* Assembly macro to enable MSR_IE
*/
void aemb_enable_interrupt ()
{
  int msr, tmp;
  asm volatile ("mfs %0, rmsr;"
		"ori %1, %0, 0x02;"
		"mts rmsr, %1;"
		: "=r"(msr)
		: "r" (tmp)
		);
}

/*!
* Assembly macro to disable MSR_IE
*/
void aemb_disable_interrupt ()
{
  int msr, tmp;
  asm volatile ("mfs %0, rmsr;"
		"andi %1, %0, 0xFD;"
		"mts rmsr, %1;"
		: "=r"(msr)
		: "r" (tmp)
		);
}

/*!
* Bootstrap Hook Override
*/

void aemb_hook_init ()
{
  int msr, tmp;
  int stk_end, stk_siz;

  /*
  // Check to see if hardware threads are enabled
  if (((msr & AEMB_TXE) != AEMB_TXE) ||
  // Check to see if second thread is started
      ((msr & AEMB_TX1) == AEMB_TX1)) return;
  */
  //return;
  asm volatile ("mfs %0, rmsr;"
		// Check for BIP
		"andi %1, %0, %5;"
		"xori %1, %1, %5;"
		"beqi %1, 44;"

		// Check for TXE
		"andi %1, %0, %2;"
		"xori %1, %1, %2;"
		"bnei %1, 36;"	

		// Check for TX1
		"andi %1, %0, %3;"
		"xori %1, %1, %3;"
		"beqi %1, 20;"
		
		// reallocate stack pointer for T0
		"ori %1, r0, %4;"
		"sra %1, %1;"
		"rsubk r1, %1, r1;"

		// reboot the machine
		//"brki r0, _crtinit;"
		"brid 0;"
		"nop;"

		// clear BIP
		"andni %1, %0, %5;"
		"mts rmsr, %1;"
		
		:"=r"(msr), "=r"(tmp)
		:"i"(AEMB_TXE), "i"(AEMB_TX1), "i"(&_STACK_SIZE), "i"(AEMB_BIP)
		);

}

/*!
 Undo the changes made by programme init
*/

void aemb_hook_clean ()
{
  int msr, tmp;
  int stk_end, stk_siz;

  /*
  // Check to see if hardware threads are enabled
  if (((msr & AEMB_TXE) != AEMB_TXE) ||
  // Check to see if second thread is started
      ((msr & AEMB_TX1) == AEMB_TX1)) return;
  */
  return;
  asm volatile ("mfs %0, rmsr;"
		"andi %1, %0, %2;"
		"xori %1, %1, %2;"
		"bnei %1, 28;"		
		"andi %1, %0, %3;"
		"xori %1, %1, %3;"
		"beqi %1, 16;"
		
		// reallocate stack pointer for T0
		"ori %1, r0, %4;"
		"sra %1, %1;"
		"addk r1, %1, r1;"

		:"=r"(msr), "=r"(tmp)
		:"i"(AEMB_TXE), "i"(AEMB_TX1), "i"(&_STACK_SIZE)
		);

  
}

/* Checks if it's T1, and returns 0 if true */

int aemb_isthread1 ()
{
  int msr, tmp, res;
  asm volatile ("mfs %0, rmsr;"
		"andi %1, %0, %3;"
		"xori %1, %1, %3;"	       
		:"=r"(msr), "=r"(tmp)
		:"i"(AEMB_TXE), "i"(AEMB_TXP)
		);
  return tmp;  
}

/* Checks if it's T0, and returns 0 if true */

int aemb_isthread0 ()
{
  int msr, tmp, res;
  asm volatile ("mfs %0, rmsr;"
		"andi %1, %0, %3;"
		"xori %1, %1, %2;"	       
		:"=r"(msr), "=r"(tmp)
		:"i"(AEMB_TXP), "i"(AEMB_MSK)
		);
  return tmp;  
}

/* Checks if TXE is available, and returns 0 if true */

int aemb_isthreaded ()
{
  int msr, tmp, res;
  asm volatile ("mfs %0, rmsr;"
		"andi %1, %0, %2;"
		"xori %1, %1, %2;"	       
		:"=r"(msr), "=r"(tmp)
		:"i"(AEMB_TXE), "i"(AEMB_MSK)
		);
  return tmp;  
}

#endif

/* $Log: libaemb.h,v $
/* Revision 1.3  2007/12/16 03:26:37  sybreon
/* Made T0 loop.
/*
/* Revision 1.1  2007/12/11 00:44:04  sybreon
/* initial import
/* */
