/* -*- c -*- */
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

// conditionalized on HAL_IO_USES_DBOARD_PINS && HAL_IO_USES_UART

#include "hal_io.h"
#include "memory_map.h"
#include "hal_uart.h"
#include "bool.h"
#include <stdio.h>
#include <string.h>
//#include <assert.h>

/*
 * ========================================================================
 *				    GPIOS
 * ========================================================================
 */
void 
hal_gpio_set_ddr(int bank, int value, int mask)
{
  bank &= 0x1;

  if (bank == GPIO_TX_BANK){	// tx in top half
    value <<= 16;
    mask <<= 16;
  }
  else {
    value &= 0xffff;
    mask &= 0xffff;
  }

  int ei = hal_disable_ints();
  gpio_base->ddr = (gpio_base->ddr & ~mask) | (value & mask);
  hal_restore_ints(ei);
}

static bool
code_to_int(char code, int *val)
{
  switch(code){
  case 's':	*val = GPIO_SEL_SW; 	 return true;
  case 'a':	*val = GPIO_SEL_ATR; 	 return true;
  case '0':	*val = GPIO_SEL_DEBUG_0; return true;
  case '1':	*val = GPIO_SEL_DEBUG_1; return true;
  case '.':	
  default:
    return false;
  }
}

void
hal_gpio_set_sel(int bank, int bitno, char code)
{
  bank &= 0x1;
  int	t;

  if (!code_to_int(code, &t))
    return;

  int val  =   t << (2 * bitno);
  int mask = 0x3 << (2 * bitno);

  volatile uint32_t *sel = bank == GPIO_TX_BANK ? &gpio_base->tx_sel : &gpio_base->rx_sel;
  int ei = hal_disable_ints();
  int v = (*sel & ~mask) | (val & mask);
  *sel = v;
  hal_restore_ints(ei);

  if (0)
    printf("hal_gpio_set_sel(bank=%d, bitno=%d, code=%c) *sel = 0x%x\n",
	   bank, bitno, code, v);
}

void
hal_gpio_set_sels(int bank, char *codes)
{
  //assert(strlen(codes) == 16);

  int val = 0;
  int mask = 0;
  int i;

  for (i = 15; i >= 0; i--){
    val <<= 2;
    mask <<= 2;
    int t;
    if (code_to_int(codes[i], &t)){
      val  |= t;
      mask |= 0x3;
    }
  }

  volatile uint32_t *sel = bank == GPIO_TX_BANK ? &gpio_base->tx_sel : &gpio_base->rx_sel;
  int ei = hal_disable_ints();
  *sel = (*sel & ~mask) | (val & mask);
  hal_restore_ints(ei);
}


/*!
 * \brief write \p value to gpio pins specified by \p mask.
 */
void
hal_gpio_write(int bank, int value, int mask)
{
  static uint32_t	_gpio_io_shadow;

  bank &= 0x1;

  if (bank == GPIO_TX_BANK){	// tx in top half
    value <<= 16;
    mask <<= 16;
  }
  else {
    value &= 0xffff;
    mask &= 0xffff;
  }

  //int ei = hal_disable_ints();
  _gpio_io_shadow = (_gpio_io_shadow & ~mask) | (value & mask);
  gpio_base->io = _gpio_io_shadow;
  //hal_restore_ints(ei);
}


/*!
 * \brief read GPIO bits
 */
int
hal_gpio_read(int bank)
{
  bank &= 0x1;
  int r = gpio_base->io;
  if (bank == GPIO_TX_BANK)
    r >>= 16;

  return r & 0xffff;
}

/*
 * ========================================================================
 *				leds
 * ========================================================================
 */

static unsigned long leds_shadow = 0;
static unsigned long led_src_shadow = 0;

void 
hal_set_leds(int value, int mask)
{
  int ei = hal_disable_ints();
  leds_shadow = (leds_shadow & ~mask) | (value & mask);
  output_regs->leds = leds_shadow;
  hal_restore_ints(ei);
}

// Allow hardware control over leds.  1 = hardware, 0 = software
void 
hal_set_led_src(int value, int mask)
{
  int ei = hal_disable_ints();
  led_src_shadow = (led_src_shadow & ~mask) | (value & mask);
  output_regs->led_src = led_src_shadow;
  hal_restore_ints(ei);
}

void 
hal_toggle_leds(int mask)
{
  int ei = hal_disable_ints();
  leds_shadow ^= mask;
  output_regs->leds = leds_shadow;
  hal_restore_ints(ei);
}


// ================================================================
//		    		primitives
// ================================================================

#if defined(HAL_IO_USES_DBOARD_PINS)
//
// Does i/o using high 9-bits of rx daughterboard pins.
//
//  1 1 1 1 1 1
//  5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |      char     |W|             |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// 
// Asserts W when writing char
//

#define W 	0x0080

void
hal_io_init(void)
{
  // make high 9 bits of tx daughterboard outputs
  hal_gpio_set_rx_mode(15, 7, GPIOM_OUTPUT);

  // and set them to zero
  hal_gpio_set_rx(0x0000, 0xff80);
}

void
hal_finish(void)
{
  volatile unsigned long *p = (unsigned long *) 0xC2F0;
  *p = 0;
}

// %c
inline int
putchar(int ch)
{
  hal_gpio_set_rx((s << 8) | W, 0xff80);
  hal_gpio_set_rx(0, 0xff80);
  return ch;
}

#elif defined(HAL_IO_USES_UART)

void
hal_io_init(void)
{
  hal_uart_init();
}

void
hal_finish(void)
{
}

// %c
inline int
putchar(int ch)
{
  hal_uart_putc(ch);
  return ch;
}

int
getchar(void)
{
  return hal_uart_getc();
}

#else	// nop all i/o

void
hal_io_init(void)
{
}

void
hal_finish(void)
{
}

// %c
inline int
putchar(int ch)
{
  return ch;
}

int
getchar(void)
{
  return EOF;
}

#endif

// ================================================================
//             (slightly) higher level functions
//
// These are here so we can inline the calls to putchar.
// The rest of the stuff was moved to nonstdio.c
// ================================================================

// \n
inline void 
newline(void)
{
  putchar('\n');
}

int
putstr(const char *s)
{
  while (*s)
    putchar(*s++);

  return 0;
}

int
puts(const char *s)
{
  putstr(s);
  putchar('\n');
  return 0;
}
