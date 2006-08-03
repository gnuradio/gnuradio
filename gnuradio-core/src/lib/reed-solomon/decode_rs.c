/* Reed-Solomon decoder
 * Copyright 2002 Phil Karn, KA9Q
 * May be used under the terms of the GNU General Public License (GPL)
 */

#ifdef DEBUG
#include <stdio.h>
#endif

#include <string.h>

#define NULL ((void *)0)
#define	min(a,b)	((a) < (b) ? (a) : (b))

#ifdef FIXED
#include "fixed.h"
#elif defined(BIGSYM)
#include "int.h"
#else
#include "char.h"
#endif

int DECODE_RS(
#ifndef FIXED
void *p,
#endif
DTYPE *data, int *eras_pos, int no_eras){

#ifndef FIXED
  struct rs *rs = (struct rs *)p;
#endif
  int deg_lambda, el, deg_omega;
  int i, j, r,k;
  DTYPE u,q,tmp,num1,num2,den,discr_r;
  DTYPE lambda[NROOTS+1], s[NROOTS];	/* Err+Eras Locator poly
					 * and syndrome poly */
  DTYPE b[NROOTS+1], t[NROOTS+1], omega[NROOTS+1];
  DTYPE root[NROOTS], reg[NROOTS+1], loc[NROOTS];
  int syn_error, count;

  /* form the syndromes; i.e., evaluate data(x) at roots of g(x) */
  for(i=0;i<NROOTS;i++)
    s[i] = data[0];

  for(j=1;j<NN;j++){
    for(i=0;i<NROOTS;i++){
      if(s[i] == 0){
	s[i] = data[j];
      } else {
	s[i] = data[j] ^ ALPHA_TO[MODNN(INDEX_OF[s[i]] + (FCR+i)*PRIM)];
      }
    }
  }

  /* Convert syndromes to index form, checking for nonzero condition */
  syn_error = 0;
  for(i=0;i<NROOTS;i++){
    syn_error |= s[i];
    s[i] = INDEX_OF[s[i]];
  }

  if (!syn_error) {
    /* if syndrome is zero, data[] is a codeword and there are no
     * errors to correct. So return data[] unmodified
     */
    count = 0;
    goto finish;
  }
  memset(&lambda[1],0,NROOTS*sizeof(lambda[0]));
  lambda[0] = 1;

  if (no_eras > 0) {
    /* Init lambda to be the erasure locator polynomial */
    lambda[1] = ALPHA_TO[MODNN(PRIM*(NN-1-eras_pos[0]))];
    for (i = 1; i < no_eras; i++) {
      u = MODNN(PRIM*(NN-1-eras_pos[i]));
      for (j = i+1; j > 0; j--) {
	tmp = INDEX_OF[lambda[j - 1]];
	if(tmp != A0)
	  lambda[j] ^= ALPHA_TO[MODNN(u + tmp)];
      }
    }

#if DEBUG >= 1
    /* Test code that verifies the erasure locator polynomial just constructed
       Needed only for decoder debugging. */
    
    /* find roots of the erasure location polynomial */
    for(i=1;i<=no_eras;i++)
      reg[i] = INDEX_OF[lambda[i]];

    count = 0;
    for (i = 1,k=IPRIM-1; i <= NN; i++,k = MODNN(k+IPRIM)) {
      q = 1;
      for (j = 1; j <= no_eras; j++)
	if (reg[j] != A0) {
	  reg[j] = MODNN(reg[j] + j);
	  q ^= ALPHA_TO[reg[j]];
	}
      if (q != 0)
	continue;
      /* store root and error location number indices */
      root[count] = i;
      loc[count] = k;
      count++;
    }
    if (count != no_eras) {
      printf("count = %d no_eras = %d\n lambda(x) is WRONG\n",count,no_eras);
      count = -1;
      goto finish;
    }
#if DEBUG >= 2
    printf("\n Erasure positions as determined by roots of Eras Loc Poly:\n");
    for (i = 0; i < count; i++)
      printf("%d ", loc[i]);
    printf("\n");
#endif
#endif
  }
  for(i=0;i<NROOTS+1;i++)
    b[i] = INDEX_OF[lambda[i]];
  
  /*
   * Begin Berlekamp-Massey algorithm to determine error+erasure
   * locator polynomial
   */
  r = no_eras;
  el = no_eras;
  while (++r <= NROOTS) {	/* r is the step number */
    /* Compute discrepancy at the r-th step in poly-form */
    discr_r = 0;
    for (i = 0; i < r; i++){
      if ((lambda[i] != 0) && (s[r-i-1] != A0)) {
	discr_r ^= ALPHA_TO[MODNN(INDEX_OF[lambda[i]] + s[r-i-1])];
      }
    }
    discr_r = INDEX_OF[discr_r];	/* Index form */
    if (discr_r == A0) {
      /* 2 lines below: B(x) <-- x*B(x) */
      memmove(&b[1],b,NROOTS*sizeof(b[0]));
      b[0] = A0;
    } else {
      /* 7 lines below: T(x) <-- lambda(x) - discr_r*x*b(x) */
      t[0] = lambda[0];
      for (i = 0 ; i < NROOTS; i++) {
	if(b[i] != A0)
	  t[i+1] = lambda[i+1] ^ ALPHA_TO[MODNN(discr_r + b[i])];
	else
	  t[i+1] = lambda[i+1];
      }
      if (2 * el <= r + no_eras - 1) {
	el = r + no_eras - el;
	/*
	 * 2 lines below: B(x) <-- inv(discr_r) *
	 * lambda(x)
	 */
	for (i = 0; i <= NROOTS; i++)
	  b[i] = (lambda[i] == 0) ? A0 : MODNN(INDEX_OF[lambda[i]] - discr_r + NN);
      } else {
	/* 2 lines below: B(x) <-- x*B(x) */
	memmove(&b[1],b,NROOTS*sizeof(b[0]));
	b[0] = A0;
      }
      memcpy(lambda,t,(NROOTS+1)*sizeof(t[0]));
    }
  }

  /* Convert lambda to index form and compute deg(lambda(x)) */
  deg_lambda = 0;
  for(i=0;i<NROOTS+1;i++){
    lambda[i] = INDEX_OF[lambda[i]];
    if(lambda[i] != A0)
      deg_lambda = i;
  }
  /* Find roots of the error+erasure locator polynomial by Chien search */
  memcpy(&reg[1],&lambda[1],NROOTS*sizeof(reg[0]));
  count = 0;		/* Number of roots of lambda(x) */
  for (i = 1,k=IPRIM-1; i <= NN; i++,k = MODNN(k+IPRIM)) {
    q = 1; /* lambda[0] is always 0 */
    for (j = deg_lambda; j > 0; j--){
      if (reg[j] != A0) {
	reg[j] = MODNN(reg[j] + j);
	q ^= ALPHA_TO[reg[j]];
      }
    }
    if (q != 0)
      continue; /* Not a root */
    /* store root (index-form) and error location number */
#if DEBUG>=2
    printf("count %d root %d loc %d\n",count,i,k);
#endif
    root[count] = i;
    loc[count] = k;
    /* If we've already found max possible roots,
     * abort the search to save time
     */
    if(++count == deg_lambda)
      break;
  }
  if (deg_lambda != count) {
    /*
     * deg(lambda) unequal to number of roots => uncorrectable
     * error detected
     */
    count = -1;
    goto finish;
  }
  /*
   * Compute err+eras evaluator poly omega(x) = s(x)*lambda(x) (modulo
   * x**NROOTS). in index form. Also find deg(omega).
   */
  deg_omega = 0;
  for (i = 0; i < NROOTS;i++){
    tmp = 0;
    j = (deg_lambda < i) ? deg_lambda : i;
    for(;j >= 0; j--){
      if ((s[i - j] != A0) && (lambda[j] != A0))
	tmp ^= ALPHA_TO[MODNN(s[i - j] + lambda[j])];
    }
    if(tmp != 0)
      deg_omega = i;
    omega[i] = INDEX_OF[tmp];
  }
  omega[NROOTS] = A0;
  
  /*
   * Compute error values in poly-form. num1 = omega(inv(X(l))), num2 =
   * inv(X(l))**(FCR-1) and den = lambda_pr(inv(X(l))) all in poly-form
   */
  for (j = count-1; j >=0; j--) {
    num1 = 0;
    for (i = deg_omega; i >= 0; i--) {
      if (omega[i] != A0)
	num1  ^= ALPHA_TO[MODNN(omega[i] + i * root[j])];
    }
    num2 = ALPHA_TO[MODNN(root[j] * (FCR - 1) + NN)];
    den = 0;
    
    /* lambda[i+1] for i even is the formal derivative lambda_pr of lambda[i] */
    for (i = min(deg_lambda,NROOTS-1) & ~1; i >= 0; i -=2) {
      if(lambda[i+1] != A0)
	den ^= ALPHA_TO[MODNN(lambda[i+1] + i * root[j])];
    }
    if (den == 0) {
#if DEBUG >= 1
      printf("\n ERROR: denominator = 0\n");
#endif
      count = -1;
      goto finish;
    }
    /* Apply error to data */
    if (num1 != 0) {
      data[loc[j]] ^= ALPHA_TO[MODNN(INDEX_OF[num1] + INDEX_OF[num2] + NN - INDEX_OF[den])];
    }
  }
 finish:
  if(eras_pos != NULL){
    for(i=0;i<count;i++)
      eras_pos[i] = loc[i];
  }
  return count;
}
