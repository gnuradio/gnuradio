/* Exercise an RS codec a specified number of times using random
 * data and error patterns
 *
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */
#define FLAG_ERASURE 1 /* Randomly flag 50% of errors as erasures */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef FIXED
#include "fixed.h"
#define EXERCISE exercise_8
#elif defined(CCSDS)
#include "fixed.h"
#include "ccsds.h"
#define EXERCISE exercise_ccsds
#elif defined(BIGSYM)
#include "int.h"
#define EXERCISE exercise_int
#else
#include "char.h"
#define EXERCISE exercise_char
#endif

#ifdef FIXED
#define PRINTPARM printf("(255,223):");
#elif defined(CCSDS)
#define PRINTPARM printf("CCSDS (255,223):");
#else
#define PRINTPARM printf("(%d,%d):",rs->nn,rs->nn-rs->nroots);
#endif

/* Exercise the RS codec passed as an argument */
int EXERCISE(
#if !defined(CCSDS) && !defined(FIXED)
void *p,
#endif
int trials){
#if !defined(CCSDS) && !defined(FIXED)
  struct rs *rs = (struct rs *)p;
#endif
#if MAX_ARRAY
  DTYPE block[MAX_ARRAY],tblock[MAX_ARRAY];
  unsigned int i;
  int errors;
  int errlocs[MAX_ARRAY];
  int derrlocs[MAX_ARRAY];
#else
  DTYPE block[NN],tblock[NN];
  unsigned int i;
  int errors;
  int errlocs[NN];
  int derrlocs[NROOTS];
#endif
  int derrors;
  int errval,errloc;
  int erasures;
  int decoder_errors = 0;

  while(trials-- != 0){
    /* Test up to the error correction capacity of the code */
    for(errors=0;(unsigned int)errors <= NROOTS/2;errors++){

      /* Load block with random data and encode */
      for(i=0;i<NN-NROOTS;i++)
	block[i] = random() & NN;
      
#if defined(CCSDS) || defined(FIXED)
      ENCODE_RS(&block[0],&block[NN-NROOTS]);
#else
      ENCODE_RS(rs,&block[0],&block[NN-NROOTS]);
#endif

      /* Make temp copy, seed with errors */
      memcpy(tblock,block,sizeof(tblock));
      memset(errlocs,0,sizeof(errlocs));
      memset(derrlocs,0,sizeof(derrlocs));
      erasures=0;
      for(i=0;i<(unsigned int)errors;i++){
	do {
	  errval = random() & NN;
	} while(errval == 0); /* Error value must be nonzero */

	do {
	  errloc = random() % NN;
	} while(errlocs[errloc] != 0); /* Must not choose the same location twice */

	errlocs[errloc] = 1;

#if FLAG_ERASURE
	if(random() & 1) /* 50-50 chance */
	  derrlocs[erasures++] = errloc;
#endif
	tblock[errloc] ^= errval;
      }

      /* Decode the errored block */
#if defined(CCSDS) || defined(FIXED)
      derrors = DECODE_RS(tblock,derrlocs,erasures);
#else
      derrors = DECODE_RS(rs,tblock,derrlocs,erasures);
#endif

      if(derrors != errors){
	PRINTPARM
	printf(" decoder says %d errors, true number is %d\n",derrors,errors);
	decoder_errors++;
      }
      for(i=0;i<(unsigned int)derrors;i++){
	if(errlocs[derrlocs[i]] == 0){
	  PRINTPARM
	  printf(" decoder indicates error in location %d without error\n",i);
	  decoder_errors++;
	}
      }
      if(memcmp(tblock,block,sizeof(tblock)) != 0){
	PRINTPARM
	printf(" uncorrected errors! output ^ input:");
	decoder_errors++;
	for(i=0;i<NN;i++)
	  printf(" %02x",tblock[i] ^ block[i]);
	printf("\n");
      }
    }
  }
  return decoder_errors;
}
