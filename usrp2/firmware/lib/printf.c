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

/*
 * Based on code from the SDCC z80 library ;)
 */

#include <stdarg.h>
#include <stdio.h>

static void 
_printn(unsigned u, unsigned base, char issigned,
	void (*emitter)(char, void *), void *pData)
{
  const char *_hex = "0123456789ABCDEF";
  if (issigned && ((int)u < 0)) {
    (*emitter)('-', pData);
    u = (unsigned)-((int)u);
  }
  if (u >= base)
    _printn(u/base, base, 0, emitter, pData);
  (*emitter)(_hex[u%base], pData);
}

static void 
_printf(const char *format, void (*emitter)(char, void *),
	void *pData, va_list va)
{
  while (*format) {
    if (*format != '%')
      (*emitter)(*format, pData);
    else {
      switch (*++format) {
      case 0:			/* hit end of format string */
	return;
      case 'c':
	{
	  char c = (char)va_arg(va, int);
	  (*emitter)(c, pData);
	  break;
	}
      case 'u':
	{
	  unsigned u = va_arg(va, unsigned);
	  _printn(u, 10, 0, emitter, pData);
	  break;
	}
      case 'd':
	{
	  unsigned u = va_arg(va, unsigned);
	  _printn(u, 10, 1, emitter, pData);
	  break;
	}
      case 'x':
      case 'p':
	{
	  unsigned u = va_arg(va, unsigned);
	  _printn(u, 16, 0, emitter, pData);
	  break;
	}
      case 's':
	{
	  char *s = va_arg(va, char *);
	  while (*s) {
	    (*emitter)(*s, pData);
	    s++;
	  }
	  break;
	}
      }
    }
    format++;
  }
}

static void 
_char_emitter(char c, void *pData __attribute__((unused)))
{
  putchar(c);
}

int 
printf(const char *format, ...)
{
  va_list va;
  va_start(va, format);

  _printf(format, _char_emitter, NULL, va);

  va_end(va);

  // wrong return value...
  return 0;
}


#if 0

// Totally dangerous.  Don't use
static void 
_buf_emitter(char c, void *pData)
{
  *((*((char **)pData)))++ = c;
}

int sprintf(char *pInto, const char *format, ...)
{
  va_list va;
  va_start(va, format);

  _printf(format, _buf_emitter, &pInto, va);
  *pInto++ = '\0';

  va_end(va);

  // FIXME wrong return value
  return 0;
}
#endif
