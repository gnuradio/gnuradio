/* $Id: aeMB_testbench.c,v 1.14 2007/12/28 21:44:04 sybreon Exp $
** 
** AEMB Function Verification C Testbench
** Copyright (C) 2004-2007 Shawn Tan Ser Ngiap <shawn.tan@aeste.net>
**
** This file is part of AEMB.
**
** AEMB is free software: you can redistribute it and/or modify it
** under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** AEMB is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
** or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
** License for more details.
**
** You should have received a copy of the GNU General Public License
** along with AEMB.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <malloc.h>
#include <errno.h>
#include <reent.h>

#include "libaemb.h"

/**
   INTERRUPT TEST

   This tests for the following:
   - Pointer addressing
   - Interrupt handling
 */

void __attribute__ ((interrupt_handler)) int_handler();
volatile int service = 0xDEADDEAD;

void int_service() 
{
  int* pio = (int*)0xFFFFFFFC;
  *pio = 0x52544E49; // "INTR"
  service = 0;
}

void int_handler()
{
  int_service();
}

/**
   INTERRUPT TEST ROUTINE
*/
int int_test ()
{
  // Delay loop until hardware interrupt triggers
  volatile int i;
  for (i=0; i < 999; i++) {
    if (service == 0) return 0;
  };

  return -1;
}

/**
   FIBONACCI TEST
   http://en.literateprograms.org/Fibonacci_numbers_(C)

   This tests for the following:
   - Recursion & Iteration
   - 32/16/8-bit data handling
*/

unsigned int fib_slow(unsigned int n)
{
  return n < 2 ? n : fib_slow(n-1) + fib_slow(n-2);
}

unsigned int fib_fast(unsigned int n)
{
  unsigned int a[3];
  unsigned int *p=a;
  unsigned int i;
  
  for(i=0; i<=n; ++i) {
    if(i<2) *p=i;
    else {
      if(p==a) *p=*(a+1)+*(a+2);
      else if(p==a+1) *p=*a+*(a+2);
      else *p=*a+*(a+1);
    }
    if(++p>a+2) p=a;
  }
  
  return p==a?*(p+2):*(p-1);
}

int fib_test(int max) {
  unsigned int n;
  unsigned int fast, slow;  
  // 32-bit LUT
  unsigned int fib_lut32[] = {
    0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233
  };  
  // 16-bit LUT
  unsigned short fib_lut16[] = {
    0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233
  };    
  // 8-bit LUT
  unsigned char fib_lut8[] = {
    0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233
  };  
  
  for (n=0;n<max;n++) {
    slow = fib_slow(n);    
    fast = fib_fast(n);
    if ((slow != fast) || 
	(fast != fib_lut32[n]) || 
	(fast != fib_lut16[n]) || 
	(fast != fib_lut8[n])) {
      return -1;      
    }
  }      
  return 0;  
}

/**
   EUCLIDEAN TEST
   http://en.literateprograms.org/Euclidean_algorithm_(C)
   
   This tests for the following:
   - Modulo arithmetic
   - Goto
*/

int euclid_gcd(int a, int b) {
  if (b > a) goto b_larger;
  while (1) {
    a = a % b;
    if (a == 0) return b;
  b_larger:
    b = b % a;
    if (b == 0) return a;
  }
}

int euclid_test(int max) 
{
  int n;
  int euclid;
  // Random Numbers
  int euclid_a[] = {
    1804289383, 1681692777, 1957747793, 719885386, 596516649,
    1025202362, 783368690, 2044897763, 1365180540, 304089172,
    35005211, 294702567, 336465782, 278722862
  };  
  int euclid_b[] = {
    846930886, 1714636915, 424238335, 1649760492, 1189641421,
    1350490027, 1102520059, 1967513926, 1540383426, 1303455736,
    521595368, 1726956429, 861021530, 233665123
  };
  
  // GCD 
  int euclid_lut[] = {
    1, 1, 1, 2, 1, 1, 1, 1, 6, 4, 1, 3, 2, 1
  };
    
  for (n=0;n<max;n++) {
    euclid = euclid_gcd(euclid_a[n],euclid_b[n]);
    if (euclid != euclid_lut[n]) {
      return -1;
    }
  }
  return 0;
}

/**
   NEWTON-RHAPSON
   http://en.literateprograms.org/Newton-Raphson's_method_for_root_finding_(C)

   This tests for the following:
   - Multiplication & Division
   - Floating point arithmetic
   - Integer to Float conversion
*/

float newton_sqrt(float n)
{
  float x = 0.0;
  float xn = 0.0;  
  int iters = 0;  
  int i;
  for (i = 0; i <= (int)n; ++i)
    {
      float val = i*i-n;
      if (val == 0.0)
	return i;
      if (val > 0.0)
	{
	  xn = (i+(i-1))/2.0;
	  break;
	}
    }  
  while (!(iters++ >= 100
	   || x == xn))
    {
      x = xn;
      xn = x - (x * x - n) / (2 * x);
    }
  return xn;
}

int newton_test (int max) {
  int n;
  float newt;
  // 32-bit LUT
  float newt_lut[] = {
    0.000000000000000000000000,
    1.000000000000000000000000,
    1.414213538169860839843750,
    1.732050776481628417968750,
    2.000000000000000000000000,
    2.236068010330200195312500,
    2.449489831924438476562500,
    2.645751237869262695312500,
    2.828427076339721679687500,
    3.000000000000000000000000,
    3.162277698516845703125000,
    3.316624879837036132812500,
    3.464101552963256835937500,
    3.605551242828369140625000,
    3.741657495498657226562500
  };

  for (n=0;n<max;n++) {
    newt = newton_sqrt(n);
    if (newt != newt_lut[n]) {
      return -1;
    }
  } 
  return 0;
}


/**
   FSL TEST ROUTINE
*/

int fsl_test ()
{
  // TEST FSL1 ONLY
  int FSL = 0xCAFEF00D;

  asm ("PUT %0, RFSL1" :: "r"(FSL));
  asm ("GET %0, RFSL1" : "=r"(FSL));
  
  if (FSL != 0x01) return -1;
  
  asm ("PUT %0, RFSL31" :: "r"(FSL));
  asm ("GET %0, RFSL31" : "=r"(FSL));
  
  if (FSL != 0x1F) return -1;
  
  return 0;  
}

// static int errnum;
/*
int *__errno ()
{
  return &_REENT->_errno;
  // return &errnum;
}
*/

int malloc_test()
{
  void *alloc;

  alloc = (void *)malloc(256); // allocate 32 bytes

  if (alloc == NULL)
    return -1;
  else
    return (int) alloc;
}

/**
   MAIN TEST PROGRAMME

   This is the main test procedure. It will output signals onto the
   MPI port that is checked by the testbench.
 */

int main () 
{
  // Message Passing Port
  int* mpi = (int*)0xFFFFFFFF;
  
  // Number of each test to run
  int max = 10;

  // lock T0 if it's multi-threaded
  /*
  if ((aemb_isthreaded() == 0) && (aemb_isthread1() != 0)) {
    while (1) {
      asm volatile ("nop;");
    }
  }
  */

  // Enable Global Interrupts
  aemb_enable_interrupt();

  // INT TEST
  //if (int_test() == -1) { *mpi = 0x4641494C; }

  // TEST MALLOC
  if (malloc_test() == -1) { *mpi = 0x4641494C; }

  // FSL TEST
  //if (fsl_test() == -1) { *mpi = 0x4641494C; }

  // Fibonacci Test
  if (fib_test(max) == -1) { *mpi = 0x4641494C; }

  // Euclid Test
  if (euclid_test(max) == -1) { *mpi = 0x4641494C; }

  // Newton-Rhapson Test
  if (newton_test(max) == -1) { *mpi = 0x4641494C; }
  
  // Disable Global Interrupts
  aemb_disable_interrupt();

  // ALL PASSED
  return 0;
}

/*
  HISTORY
  $Log: aeMB_testbench.c,v $
  Revision 1.14  2007/12/28 21:44:04  sybreon
  Added malloc() test

  Revision 1.13  2007/12/11 00:44:31  sybreon
  Modified for AEMB2
  
  Revision 1.12  2007/11/18 19:41:45  sybreon
  Minor simulation fixes.
  
  Revision 1.11  2007/11/14 23:41:06  sybreon
  Fixed minor interrupt test typo.
  
  Revision 1.10  2007/11/14 22:12:02  sybreon
  Added interrupt test routine.
  
  Revision 1.9  2007/11/09 20:51:53  sybreon
  Added GET/PUT support through a FSL bus.
  
  Revision 1.8  2007/11/03 08:40:18  sybreon
  Minor code cleanup.
  
  Revision 1.7  2007/11/02 18:32:19  sybreon
  Enable MSR_IE with software.
  
  Revision 1.6  2007/04/30 15:57:10  sybreon
  Removed byte acrobatics.
  
  Revision 1.5  2007/04/27 15:17:59  sybreon
  Added code documentation.
  Added new tests that test floating point, modulo arithmetic and multiplication/division.
  
  Revision 1.4  2007/04/25 22:15:05  sybreon
  Added support for 8-bit and 16-bit data types.
  
  Revision 1.3  2007/04/04 14:09:04  sybreon
  Added initial interrupt/exception support.
  
  Revision 1.2  2007/04/04 06:07:45  sybreon
  Fixed C code bug which passes the test
  
  Revision 1.1  2007/03/09 17:41:57  sybreon
  initial import  
*/
