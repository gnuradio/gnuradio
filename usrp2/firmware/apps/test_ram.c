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

#include <stdio.h>
#include <stdint.h>
#include <u2_init.h>		/* FIXME */
#include <sd.h>
#include <string.h>
#include <hal_io.h>
#include <nonstdio.h>
#include <hal_uart.h>

#define ASSERT_TRUE(x) \
  do { \
    if (!(x)){ \
      printf("ASSERT_TRUE failed on line %d\n", __LINE__); \
      nerrors++; \
    } \
  } while(0)

#define ASSERT_FALSE(x) \
  do { \
    if (x){ \
      printf("ASSERT_FALSE failed on line %d\n", __LINE__); \
      nerrors++; \
    } \
  } while(0)


#define BUFSIZE 128

int test_ram()
{
  int i,j,k;
  output_regs->ram_page = 1<<10;
  
  extram[0] = 0xDEADBEEF;
  extram[1] = 0xF00D1234;
  extram[7] = 0x76543210;
  
  output_regs->ram_page = 2<<10;
  extram[7] = 0x55555555;
  extram[1] = 0xaaaaaaaa;
  extram[0] = 0xeeeeeeee;
  
  output_regs->ram_page = 1<<10;
  
  i = extram[0];
  k = extram[1];
  j = extram[7];
  
  if((i != 0xDEADBEEF)||(j!=0x76543210)||(k!=0xF00D1234)) {
    puts("RAM FAIL1!\n");
    puthex32_nl(i);
    puthex32_nl(j);
    puthex32_nl(k);
    return 0;
  }
  
  output_regs->ram_page = 2<<10;

  j = extram[7];
  k = extram[1];
  i = extram[0];

  if((i != 0xeeeeeeee)||(j!=0x55555555)||(k!=0xaaaaaaaa)) {
    puts("RAM FAIL2!\n");
    puthex32_nl(i);
    puthex32_nl(j);
    puthex32_nl(k);
    return 0;
  }
  return 1;
}

int
main(void)
{

  u2_init();
  puts("\ntest_ram\n");
  int success = test_ram();
  if(success)
    puts("RAM Passed Tests\n");
  else
    puts("RAM Failed\n");

  hal_finish();
  return 0;
}

