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
#include <i2c.h>
#include <usrp2_i2c_addr.h>
#include <string.h>
#include <hal_io.h>



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
  bool ok;
  int  nerrors = 0;
  uint8_t buf[BUFSIZE];
  int not_dev_addr = 0x35;	// no device with this address on the i2c bus.
  int offset;
  int len;
  
  u2_init();

  puts("test_i2c\n");

  // try writing a non-existent device
  buf[0] = 0xA5;
  ok = i2c_write(not_dev_addr, buf, 1);
  ASSERT_FALSE(ok);

  // try read from non-existent device
  buf[0] = 0;
  ok = i2c_read(not_dev_addr, buf, 1);
  ASSERT_FALSE(ok);

  // try writing eeprom
  offset = 31;
  len = 8;
  memset(buf, 0, sizeof(buf));
  for (i = 0; i < len; i++)
    buf[i] = i;
  ok = eeprom_write(I2C_ADDR_MBOARD, offset, buf, len);
  ASSERT_TRUE(ok);

  // now try to read it back
  offset = 31;
  len = 8;
  memset(buf, 0, sizeof(buf));
  ok = eeprom_read(I2C_ADDR_MBOARD, offset, buf, len);
  ASSERT_TRUE(ok);

  // check result
  for (i = 0; i < len; i++){
    if (buf[i] != i){
      printf("buf[%d] = %d, should be %d\n", i, buf[i], i);
      nerrors++;
    }
  }
  
  if (nerrors == 0){
    output_regs->leds = 0x3;
    puts("PASSED\n");
  }
  else {
    output_regs->leds = 0x0;
    puts("FAILED\n");
  }

  hal_finish();
  return 0;
}

