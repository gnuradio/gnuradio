/*
 * Copyright 2007,2008 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "u2_init.h"
#include "memory_map.h"
#include "spi.h"
#include "hal_io.h"
#include "buffer_pool.h"
#include "pic.h"
#include "bool.h"
#include "ethernet.h"
#include "nonstdio.h"
#include "usrp2_eth_packet.h"
#include "dbsm.h"
#include "app_common_v2.h"
#include "memcpy_wa.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <i2c.h>
#include <usrp2_i2c_addr.h>
#include <clocks.h>
#include "sd.h"
#include "mdelay.h"

#define HW_REV_MAJOR 4
#define HW_REV_MINOR 0

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

int test_sd()
{
  int i = sd_init();
  if(i==0) {
    puts("FAILED INIT of Card\n");
    return 0;
  }
  
  unsigned char buf[512];
  i = sd_read_block(2048,buf);
  if(i == 0) {
    puts("READ Command Rejected\n");
    return 0;
  }
  if((buf[0]==0xb8)&&(buf[1]==0x08)&&(buf[2]==0x00)&&(buf[3]==0x50))
    ;
  else {
    puts("Read bad data from SD Card\n");
    return 0;
  }
  return 1;
}

int
main(void)
{
  u2_init();

  putstr("\nFactory Test, Board Rev 4.0\n");

  bool ok = true;
  unsigned char maj = HW_REV_MAJOR;
  unsigned char min = HW_REV_MINOR;
  ok = eeprom_write(I2C_ADDR_MBOARD, MBOARD_REV_MSB, &maj, 1);
  ok &= eeprom_write(I2C_ADDR_MBOARD, MBOARD_REV_LSB, &min, 1);

  putstr("\nset_hw_rev\n");
  if (ok)
    printf("OK: set h/w rev to %d.%d\n", HW_REV_MAJOR, HW_REV_MINOR);
  else {
    printf("FAILED to set h/w rev to %d.%d\n", HW_REV_MAJOR, HW_REV_MINOR);
    hal_finish();
    return 0;
  }

  if(test_sd())
    puts("SD OK\n");
  else {
    puts("SD FAIL\n");
    //hal_finish();
    //return 0;
  }
  if(test_ram())
    puts("RAM OK\n");
  else {
    puts("RAM FAIL\n");
    hal_finish();
    return 0;
  }

  print_mac_addr(ethernet_mac_addr()->addr);
  newline();

  clocks_mimo_config(MC_WE_LOCK_TO_SMA);

  while (!clocks_lock_detect()) {
    puts("No Lock");
    mdelay(1000);
  }
  puts("Clock Locked\n");

}
