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

int
main(void)
{
  int i;
  unsigned char buf[512];

  u2_init();

  puts("\ntest_sd\n");
  

  i = sd_init();
  if(i)
    puts("Successfully Init'ed Card\n");
  else
    puts("FAILED INIT of Card\n");

  i = sd_read_block(2048,buf);
  if(i) {
    puts("READ Command accepted\n");
    for(i=0;i<512;i++)
      if((i&15) == 15)
	puthex8_nl(buf[i]);
      else {
	puthex8(buf[i]);
	putchar(' ');
      }
  }
  else
    puts("READ Command Rejected\n");
  
  puts("Done");
  hal_finish();
  return 0;
}

