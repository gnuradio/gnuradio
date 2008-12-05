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

#ifndef INCLUDED_HAL_UART_H
#define INCLUDED_HAL_UART_H


/*!
 * \brief one-time call to init
 */
void hal_uart_init(void);

typedef enum {
  US_9600,
  US_19200,
  US_38400,
  US_57600,
  US_115200,
  US_230400,
} hal_uart_speed_t;

typedef struct {
  hal_uart_speed_t	speed;
} hal_uart_config_t;

/*!
 * \brief Set uart parameters
 *  Default is 115,200 bps, 8N1.
 */
void hal_uart_set_config(const hal_uart_config_t *c);

/*!
 * \brief Get uart configuation.
 */
void hal_uart_get_config(hal_uart_config_t *c);

/*!
 * \brief Enqueue \p ch for output over serial port
 */
void hal_uart_putc(int ch);

/*!
 * \brief Enqueue \p ch for output over serial port, silent fail if queue is full
 */
void hal_uart_putc_nowait(int ch);

/*
 * \brief Blocking read of next char from serial port
 */
int hal_uart_getc(void);


#endif /* INCLUDED_HAL_UART_H */
