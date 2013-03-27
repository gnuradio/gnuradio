/* Wrapper functions for malloc/free that force 16-byte alignment
 * See http://perso.club-internet.fr/matmac/sourcesc.htm

 * Copyright 2001 Phil Karn, KA9Q
 * May be used under the terms of the GNU Public License (GPL)
 */

#include "malloc16.h"
#include <string.h>

void *malloc16Align(int size){
  void *p;
  void **p1;

  if((p = malloc(size+31)) == NULL)
    return NULL;

  /* Round up to next 16-byte boundary */
  p1 = (void **)(((long)p + 31) & (~15));

  /* Stash actual start of block just before ptr we return */
  p1[-1] = p;

  /* Return 16-byte aligned address */
  return (void *)p1;
}

void *calloc16Align(size_t nmemb,size_t size){
  int nbytes;
  void *p;

  nbytes = nmemb*size;
  if((p = malloc16Align(nbytes)) == NULL)
    return NULL;

  memset(p,0,nbytes);
  return p;
}

void free16Align(void *p){

  if(p != NULL){
    /* Retrieve pointer to actual start of block and free it */
    free(((void **)p)[-1]);
  }
}
