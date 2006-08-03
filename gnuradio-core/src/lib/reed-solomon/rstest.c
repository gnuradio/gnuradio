/* Test the Reed-Solomon codecs
 * for various block sizes and with random data and random error patterns
 *
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rs.h"

int exercise_char(void *,int);

#ifdef ALL_VERSIONS
int exercise_int(void *,int);
int exercise_8(int);
int exercise_ccsds(int);
#endif

struct {
  int symsize;
  int genpoly;
  int fcs;
  int prim;
  int nroots;
  int ntrials;
} Tab[] = {
  {2, 0x7,     1,   1, 1, 10 },
  {3, 0xb,     1,   1, 2, 10 },
  {4, 0x13,    1,   1, 4, 10 },
  {5, 0x25,    1,   1, 6, 10 },
  {6, 0x43,    1,   1, 8, 10 },
  {7, 0x89,    1,   1, 10, 10 },
  {8, 0x11d,   1,   1, 32, 10 },
  {8, 0x187,   112,11, 32, 10 }, /* Duplicates CCSDS codec */
#ifdef ALL_VESIONS
  {9, 0x211,   1,   1, 32, 10 },
  {10,0x409,   1,   1, 32, 10 },
  {11,0x805,   1,   1, 32, 10 },
  {12,0x1053,  1,   1, 32, 5 },
  {13,0x201b,  1,   1, 32, 2 },
  {14,0x4443,  1,   1, 32, 1 },
  {15,0x8003,  1,   1, 32, 1 },
  {16,0x1100b, 1,   1, 32, 1 },
#endif
  {0, 0, 0, 0, 0},
};

int main(){
  void *handle;
  int errs,terrs;
  int i;

  terrs = 0;
  srandom(time(NULL));

#ifdef ALL_VERSIONS
  printf("Testing fixed (255,223) RS codec...");
  fflush(stdout);
  errs = exercise_8(10);
  terrs += errs;
  if(errs == 0){
    printf("OK\n");
  }
  printf("Testing CCSDS standard (255,223) RS codec...");
  fflush(stdout);
  errs = exercise_ccsds(10);
  terrs += errs;
  if(errs == 0){
    printf("OK\n");
  }
#endif

  for(i=0;Tab[i].symsize != 0;i++){
    int nn,kk;

    nn = (1<<Tab[i].symsize) - 1;
    kk = nn - Tab[i].nroots;
    printf("Testing (%d,%d) RS codec...",nn,kk);
    fflush(stdout);
    if(Tab[i].symsize <= 8){
      if((handle = init_rs_char(Tab[i].symsize,Tab[i].genpoly,Tab[i].fcs,Tab[i].prim,Tab[i].nroots)) == NULL){
	printf("init_rs_char failed!\n");
	continue;
      }
      errs = exercise_char(handle,Tab[i].ntrials);
    } else {
#ifdef ALL_VERSIONS
      if((handle = init_rs_int(Tab[i].symsize,Tab[i].genpoly,Tab[i].fcs,Tab[i].prim,Tab[i].nroots)) == NULL){
	printf("init_rs_int failed!\n");
	continue;
      }
      errs = exercise_int(handle,Tab[i].ntrials);
#else
      printf ("init_rs_init support is not enabled\n");
      exit (1);
#endif

    }
    terrs += errs;
    if(errs == 0){
      printf("OK\n");
    }
    free_rs_char(handle);
  }
  if(terrs == 0)
    printf("All codec tests passed!\n");

  exit(0);
}


