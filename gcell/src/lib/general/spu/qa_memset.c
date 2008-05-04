/* -*- c++ -*- */
/*
 * Copyright 2008 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <gc_declare_proc.h>
#include <string.h>
#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>


#define MAX_QA_BYTES  1024
#define	MAX_OFFSET    32
#define	ALIGNMENT     16
#define	K	      0xA5

// FIXME should be passed at gcell init time
//static const int TIMEBASE = 79800000; // ps3
static const int TIMEBASE = 26666666; // qs21

typedef void* (*memset_fptr)(void *s, int val, size_t n);

void *
memset_ref(void *sv, int c, size_t n)
{
  unsigned char *s = (unsigned char *) sv;
  size_t i;
  for (i = 0; i < n; i++)
    s[i] = c;

  return sv;
}

static bool
check_before(unsigned char *buf, size_t len, size_t offset)
{
  unsigned char *p = buf + sizeof(vector unsigned char) + offset;
  bool ok = true;
  int i;

  for (i = -16; i < 0; i++){
    unsigned char expected = (&p[i] - buf) & 0xff;
    if (p[i] != expected){
      printf("b:memset(%p, 0x%x, %zu) <offset %2zd> [%3d] expected %02x, got %02x\n",
	     p, K, len, offset, i, K, p[i]);
      ok = false;
    }
  }
  return ok;
}

static bool
check_middle(unsigned char *buf, size_t len, size_t offset)
{
  unsigned char *p = buf + sizeof(vector unsigned char) + offset;
  bool ok = true;
  size_t i;

  for (i = 0; i < len; i++){
    unsigned char expected = K;
    if (p[i] != expected){
      printf("m:memset(%p, 0x%x, %zu) <offset %2zd> [%3zd] expected %02x, got %02x\n",
	     p, K, len, offset, i, expected, p[i]);
      ok = false;
    }
  }
  return ok;
}

static bool
check_after(unsigned char *buf, size_t len, size_t offset)
{
  unsigned char *p = buf + sizeof(vector unsigned char) + offset;
  bool ok = true;
  size_t i;

  for (i = len; i < len + 16; i++){
    unsigned char expected = (&p[i] - buf) & 0xff;
    if (p[i] != expected){
      printf("a:memset(%p, 0x%x, %zu) <offset %2zd> [%3zd] expected %02x, got %02x\n",
	     p, K, len, offset, i, expected, p[i]);
      ok = false;
    }
  }
  return ok;
}


static bool
test_memset_aux(memset_fptr f,
		unsigned char *buf, size_t buflen, size_t len, size_t offset)
{
  size_t i;

  // init buffer to non-zero known state
  for (i = 0; i < buflen; i++)
    buf[i] = i;
  
  // Our working buffer.  Starts 16 bytes + offset into buf.
  // We offset by 16 so that we can see if data before is getting damaged.
  unsigned char *p = buf + sizeof(vector unsigned char) + offset;

  (*f)(p, K, len);

  bool ok = true;
  ok &= check_before(buf, len, offset);
  ok &= check_middle(buf, len, offset);
  ok &= check_after(buf, len, offset);

  return ok;
}

bool
test_memset(memset_fptr f)
{
  size_t BUFLEN = MAX_QA_BYTES + 2*sizeof(vector unsigned char) + MAX_OFFSET;
  unsigned char unaligned_buf[BUFLEN + ALIGNMENT -1];
  unsigned char *aligned_buf =
    (unsigned char *)((((intptr_t) unaligned_buf) + ALIGNMENT - 1) & -ALIGNMENT);

  // printf("unaligned = %p\n", unaligned_buf);
  // printf("aligned   = %p\n", aligned_buf);

  size_t len;
  size_t offset;
  bool ok = true;

  for (len = 0; len < MAX_QA_BYTES; len++){
    for (offset = 0; offset <= MAX_OFFSET; offset++){
      ok &= test_memset_aux(f, aligned_buf, BUFLEN, len, offset);
    }
  }

  return ok;
}

// returns bytes/s
float
benchmark_memset(memset_fptr f, bool aligned)
{
  static const int SIZE = 32768;
  unsigned char buf[SIZE];
  uint32_t	t0, t1;
  int		nbytes;

  spu_write_decrementer(0xffffffff);

  if (aligned){
    nbytes = SIZE;
    t0 = spu_read_decrementer();
    (*f)(buf, 0x55, nbytes);
    (*f)(buf, 0x55, nbytes);
    (*f)(buf, 0x55, nbytes);
    (*f)(buf, 0x55, nbytes);
    t1 = spu_read_decrementer();
  }
  else {
    nbytes = SIZE - 2;
    t0 = spu_read_decrementer();
    (*f)(buf + 1, 0x55, nbytes);
    (*f)(buf + 1, 0x55, nbytes);
    (*f)(buf + 1, 0x55, nbytes);
    (*f)(buf + 1, 0x55, nbytes);
    t1 = spu_read_decrementer();
  }

  //printf("delta ticks: %d\n", t0 - t1);
  return (float) nbytes * 4 / ((t0 - t1) * 1.0/TIMEBASE);
}

/*
 * Implement the standard QA stub.
 * No input arguments, 1 bool output.
 */
static void
gcs_qa_memset(const gc_job_direct_args_t *input _UNUSED,
	      gc_job_direct_args_t *output,
	      const gc_job_ea_args_t *eaa _UNUSED)
{
  bool ok = test_memset(memset);
  output->arg[0].u32 = ok;
}

GC_DECLARE_PROC(gcs_qa_memset, "qa_memset");
