#ifndef INCLUDED_volk_32fc_x2_dot_prod_32fc_a_H
#define INCLUDED_volk_32fc_x2_dot_prod_32fc_a_H

#include <volk/volk_common.h>
#include <volk/volk_complex.h>
#include <stdio.h>
#include <string.h>


#ifdef LV_HAVE_GENERIC 


static inline void volk_32fc_x2_dot_prod_32fc_a_generic(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {
  
  float * res = (float*) result;
  float * in = (float*) input;
  float * tp = (float*) taps;
  unsigned int n_2_ccomplex_blocks = num_bytes >> 4;
  unsigned int isodd = (num_bytes >> 3) &1;
  
  
  
  float sum0[2] = {0,0};
  float sum1[2] = {0,0};
  unsigned int i = 0;

  
  for(i = 0; i < n_2_ccomplex_blocks; ++i) {
    

    sum0[0] += in[0] * tp[0] - in[1] * tp[1];
    sum0[1] += in[0] * tp[1] + in[1] * tp[0];
    sum1[0] += in[2] * tp[2] - in[3] * tp[3];
    sum1[1] += in[2] * tp[3] + in[3] * tp[2];
    
    
    in += 4;
    tp += 4;

  }

  
  res[0] = sum0[0] + sum1[0];
  res[1] = sum0[1] + sum1[1];
  
  
  
  for(i = 0; i < isodd; ++i) {


    *result += input[(num_bytes >> 3) - 1] * taps[(num_bytes >> 3) - 1];

  }

}

#endif /*LV_HAVE_GENERIC*/


#if LV_HAVE_SSE && LV_HAVE_64


static inline void volk_32fc_x2_dot_prod_32fc_a_sse_64(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {
  

  asm 
    (
     "#  ccomplex_dotprod_generic (float* result, const float *input,\n\t"
     "#                         const float *taps, unsigned num_bytes)\n\t"
     "#    float sum0 = 0;\n\t"
     "#    float sum1 = 0;\n\t"
     "#    float sum2 = 0;\n\t"
     "#    float sum3 = 0;\n\t"
     "#    do {\n\t"
     "#      sum0 += input[0] * taps[0] - input[1] * taps[1];\n\t"
     "#      sum1 += input[0] * taps[1] + input[1] * taps[0];\n\t"
     "#      sum2 += input[2] * taps[2] - input[3] * taps[3];\n\t"
     "#      sum3 += input[2] * taps[3] + input[3] * taps[2];\n\t"
     "#      input += 4;\n\t"
     "#      taps += 4;  \n\t"
     "#    } while (--n_2_ccomplex_blocks != 0);\n\t"
     "#    result[0] = sum0 + sum2;\n\t"
     "#    result[1] = sum1 + sum3;\n\t"
     "# TODO: prefetch and better scheduling\n\t"
     "  xor    %%r9,  %%r9\n\t"
     "  xor    %%r10, %%r10\n\t"
     "  movq   %%rcx, %%rax\n\t"
     "  movq   %%rcx, %%r8\n\t"
     "  movq   %[rsi],  %%r9\n\t"
     "  movq   %[rdx], %%r10\n\t"
     "	xorps	%%xmm6, %%xmm6		# zero accumulators\n\t"
     "	movaps	0(%%r9), %%xmm0\n\t"
     "	xorps	%%xmm7, %%xmm7		# zero accumulators\n\t"
     "	movaps	0(%%r10), %%xmm2\n\t"
     "	shr	$5, %%rax		# rax = n_2_ccomplex_blocks / 2\n\t"
     "  shr     $4, %%r8\n\t"
     "	jmp	.%=L1_test\n\t"
     "	# 4 taps / loop\n\t"
     "	# something like ?? cycles / loop\n\t"
     ".%=Loop1:	\n\t"
     "# complex prod: C += A * B,  w/ temp Z & Y (or B), xmmPN=$0x8000000080000000\n\t"
     "#	movaps	(%%r9), %%xmmA\n\t"
     "#	movaps	(%%r10), %%xmmB\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	shufps	$0xb1, %%xmmZ, %%xmmZ	# swap internals\n\t"
     "#	mulps	%%xmmB, %%xmmA\n\t"
     "#	mulps	%%xmmZ, %%xmmB\n\t"
     "#	# SSE replacement for: pfpnacc %%xmmB, %%xmmA\n\t"
     "#	xorps	%%xmmPN, %%xmmA\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	unpcklps %%xmmB, %%xmmA\n\t"
     "#	unpckhps %%xmmB, %%xmmZ\n\t"
     "#	movaps	%%xmmZ, %%xmmY\n\t"
     "#	shufps	$0x44, %%xmmA, %%xmmZ	# b01000100\n\t"
     "#	shufps	$0xee, %%xmmY, %%xmmA	# b11101110\n\t"
     "#	addps	%%xmmZ, %%xmmA\n\t"
     "#	addps	%%xmmA, %%xmmC\n\t"
     "# A=xmm0, B=xmm2, Z=xmm4\n\t"
     "# A'=xmm1, B'=xmm3, Z'=xmm5\n\t"
     "	movaps	16(%%r9), %%xmm1\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	movaps	16(%%r10), %%xmm3\n\t"
     "	movaps	%%xmm1, %%xmm5\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm3, %%xmm1\n\t"
     "	shufps	$0xb1, %%xmm5, %%xmm5	# swap internals\n\t"
     "	addps	%%xmm1, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	movaps	32(%%r9), %%xmm0\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     "	mulps	%%xmm5, %%xmm3\n\t"
     "	add	$32, %%r9\n\t"
     "	movaps	32(%%r10), %%xmm2\n\t"
     "	addps	%%xmm3, %%xmm7\n\t"
     "	add	$32, %%r10\n\t"
     ".%=L1_test:\n\t"
     "	dec	%%rax\n\t"
     "	jge	.%=Loop1\n\t"
     "	# We've handled the bulk of multiplies up to here.\n\t"
     "	# Let's sse if original n_2_ccomplex_blocks was odd.\n\t"
     "	# If so, we've got 2 more taps to do.\n\t"
     "	and	$1, %%r8\n\t"
     "	je	.%=Leven\n\t"
     "	# The count was odd, do 2 more taps.\n\t"
     "	# Note that we've already got mm0/mm2 preloaded\n\t"
     "	# from the main loop.\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     ".%=Leven:\n\t"
     "	# neg inversor\n\t"
     "	xorps	%%xmm1, %%xmm1\n\t"
     "	mov	$0x80000000, %%r9\n\t"
     "	movd	%%r9, %%xmm1\n\t"
     "	shufps	$0x11, %%xmm1, %%xmm1	# b00010001 # 0 -0 0 -0\n\t"
     "	# pfpnacc\n\t"
     "	xorps	%%xmm1, %%xmm6\n\t"
     "	movaps	%%xmm6, %%xmm2\n\t"
     "	unpcklps %%xmm7, %%xmm6\n\t"
     "	unpckhps %%xmm7, %%xmm2\n\t"
     "	movaps	%%xmm2, %%xmm3\n\t"
     "	shufps	$0x44, %%xmm6, %%xmm2	# b01000100\n\t"
     "	shufps	$0xee, %%xmm3, %%xmm6	# b11101110\n\t"
     "	addps	%%xmm2, %%xmm6\n\t"
     "					# xmm6 = r1 i2 r3 i4\n\t"
     "	movhlps	%%xmm6, %%xmm4		# xmm4 = r3 i4 ?? ??\n\t"
     "	addps	%%xmm4, %%xmm6		# xmm6 = r1+r3 i2+i4 ?? ??\n\t"
     "	movlps	%%xmm6, (%[rdi])		# store low 2x32 bits (complex) to memory\n\t"
     :
     :[rsi] "r" (input), [rdx] "r" (taps), "c" (num_bytes), [rdi] "r" (result)
     :"rax", "r8", "r9", "r10"
     );
  
  
  int getem = num_bytes % 16;
  
  
  for(; getem > 0; getem -= 8) {
  
    
    *result += (input[(num_bytes >> 3) - 1] * taps[(num_bytes >> 3) - 1]);
  
  }

  return;
  
}

#endif

#if LV_HAVE_SSE && LV_HAVE_32

static inline void volk_32fc_x2_dot_prod_32fc_a_sse_32(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {

  volk_32fc_x2_dot_prod_32fc_a_generic(result, input, taps, num_bytes);

#if 0
  asm volatile 
    (
     "	#pushl	%%ebp\n\t"
     "	#movl	%%esp, %%ebp\n\t"
     "	movl	12(%%ebp), %%eax		# input\n\t"
     "	movl	16(%%ebp), %%edx		# taps\n\t"
     "	movl	20(%%ebp), %%ecx                # n_bytes\n\t"
     "	xorps	%%xmm6, %%xmm6		# zero accumulators\n\t"
     "	movaps	0(%%eax), %%xmm0\n\t"
     "	xorps	%%xmm7, %%xmm7		# zero accumulators\n\t"
     "	movaps	0(%%edx), %%xmm2\n\t"
     "	shrl	$5, %%ecx		# ecx = n_2_ccomplex_blocks / 2\n\t"
     "	jmp	.%=L1_test\n\t"
     "	# 4 taps / loop\n\t"
     "	# something like ?? cycles / loop\n\t"
     ".%=Loop1:	\n\t"
     "# complex prod: C += A * B,  w/ temp Z & Y (or B), xmmPN=$0x8000000080000000\n\t"
     "#	movaps	(%%eax), %%xmmA\n\t"
     "#	movaps	(%%edx), %%xmmB\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	shufps	$0xb1, %%xmmZ, %%xmmZ	# swap internals\n\t"
     "#	mulps	%%xmmB, %%xmmA\n\t"
     "#	mulps	%%xmmZ, %%xmmB\n\t"
     "#	# SSE replacement for: pfpnacc %%xmmB, %%xmmA\n\t"
     "#	xorps	%%xmmPN, %%xmmA\n\t"
     "#	movaps	%%xmmA, %%xmmZ\n\t"
     "#	unpcklps %%xmmB, %%xmmA\n\t"
     "#	unpckhps %%xmmB, %%xmmZ\n\t"
     "#	movaps	%%xmmZ, %%xmmY\n\t"
     "#	shufps	$0x44, %%xmmA, %%xmmZ	# b01000100\n\t"
     "#	shufps	$0xee, %%xmmY, %%xmmA	# b11101110\n\t"
     "#	addps	%%xmmZ, %%xmmA\n\t"
     "#	addps	%%xmmA, %%xmmC\n\t"
     "# A=xmm0, B=xmm2, Z=xmm4\n\t"
     "# A'=xmm1, B'=xmm3, Z'=xmm5\n\t"
     "	movaps	16(%%eax), %%xmm1\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	movaps	16(%%edx), %%xmm3\n\t"
     "	movaps	%%xmm1, %%xmm5\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm3, %%xmm1\n\t"
     "	shufps	$0xb1, %%xmm5, %%xmm5	# swap internals\n\t"
     "	addps	%%xmm1, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	movaps	32(%%eax), %%xmm0\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     "	mulps	%%xmm5, %%xmm3\n\t"
     "	addl	$32, %%eax\n\t"
     "	movaps	32(%%edx), %%xmm2\n\t"
     "	addps	%%xmm3, %%xmm7\n\t"
     "	addl	$32, %%edx\n\t"
     ".%=L1_test:\n\t"
     "	decl	%%ecx\n\t"
     "	jge	.%=Loop1\n\t"
     "	# We've handled the bulk of multiplies up to here.\n\t"
     "	# Let's sse if original n_2_ccomplex_blocks was odd.\n\t"
     "	# If so, we've got 2 more taps to do.\n\t"
     "	movl	20(%%ebp), %%ecx		# n_2_ccomplex_blocks\n\t"
     "  shrl    $4, %%ecx\n\t"
     "	andl	$1, %%ecx\n\t"
     "	je	.%=Leven\n\t"
     "	# The count was odd, do 2 more taps.\n\t"
     "	# Note that we've already got mm0/mm2 preloaded\n\t"
     "	# from the main loop.\n\t"
     "	movaps	%%xmm0, %%xmm4\n\t"
     "	mulps	%%xmm2, %%xmm0\n\t"
     "	shufps	$0xb1, %%xmm4, %%xmm4	# swap internals\n\t"
     "	addps	%%xmm0, %%xmm6\n\t"
     "	mulps	%%xmm4, %%xmm2\n\t"
     "	addps	%%xmm2, %%xmm7\n\t"
     ".%=Leven:\n\t"
     "	# neg inversor\n\t"
     "  movl 8(%%ebp), %%eax \n\t"
     "	xorps	%%xmm1, %%xmm1\n\t"
     "  movl	$0x80000000, (%%eax)\n\t"
     "	movss	(%%eax), %%xmm1\n\t"
     "	shufps	$0x11, %%xmm1, %%xmm1	# b00010001 # 0 -0 0 -0\n\t"
     "	# pfpnacc\n\t"
     "	xorps	%%xmm1, %%xmm6\n\t"
     "	movaps	%%xmm6, %%xmm2\n\t"
     "	unpcklps %%xmm7, %%xmm6\n\t"
     "	unpckhps %%xmm7, %%xmm2\n\t"
     "	movaps	%%xmm2, %%xmm3\n\t"
     "	shufps	$0x44, %%xmm6, %%xmm2	# b01000100\n\t"
     "	shufps	$0xee, %%xmm3, %%xmm6	# b11101110\n\t"
     "	addps	%%xmm2, %%xmm6\n\t"
     "					# xmm6 = r1 i2 r3 i4\n\t"
     "	#movl	8(%%ebp), %%eax		# @result\n\t"
     "	movhlps	%%xmm6, %%xmm4		# xmm4 = r3 i4 ?? ??\n\t"
     "	addps	%%xmm4, %%xmm6		# xmm6 = r1+r3 i2+i4 ?? ??\n\t"
     "	movlps	%%xmm6, (%%eax)		# store low 2x32 bits (complex) to memory\n\t"
     "	#popl	%%ebp\n\t"
     :
     :
     : "eax", "ecx", "edx"
     );

  
  int getem = num_bytes % 16;
  
  for(; getem > 0; getem -= 8) {
    
    
    *result += (input[(num_bytes >> 3) - 1] * taps[(num_bytes >> 3) - 1]);
    
  }
  
  return;
#endif  
}

#endif /*LV_HAVE_SSE*/  

#ifdef LV_HAVE_SSE3

#include <pmmintrin.h>

static inline void volk_32fc_x2_dot_prod_32fc_a_sse3(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {
  

  lv_32fc_t dotProduct;
  memset(&dotProduct, 0x0, 2*sizeof(float));

  unsigned int number = 0;
  const unsigned int halfPoints = num_bytes >> 4;

  __m128 x, y, yl, yh, z, tmp1, tmp2, dotProdVal;

  const lv_32fc_t* a = input;
  const lv_32fc_t* b = taps;

  dotProdVal = _mm_setzero_ps();

  for(;number < halfPoints; number++){
      
    x = _mm_load_ps((float*)a); // Load the ar + ai, br + bi as ar,ai,br,bi
    y = _mm_load_ps((float*)b); // Load the cr + ci, dr + di as cr,ci,dr,di
      
    yl = _mm_moveldup_ps(y); // Load yl with cr,cr,dr,dr
    yh = _mm_movehdup_ps(y); // Load yh with ci,ci,di,di
      
    tmp1 = _mm_mul_ps(x,yl); // tmp1 = ar*cr,ai*cr,br*dr,bi*dr
      
    x = _mm_shuffle_ps(x,x,0xB1); // Re-arrange x to be ai,ar,bi,br
      
    tmp2 = _mm_mul_ps(x,yh); // tmp2 = ai*ci,ar*ci,bi*di,br*di
      
    z = _mm_addsub_ps(tmp1,tmp2); // ar*cr-ai*ci, ai*cr+ar*ci, br*dr-bi*di, bi*dr+br*di

    dotProdVal = _mm_add_ps(dotProdVal, z); // Add the complex multiplication results together

    a += 2;
    b += 2;
  }

  __VOLK_ATTR_ALIGNED(16) lv_32fc_t dotProductVector[2];

  _mm_store_ps((float*)dotProductVector,dotProdVal); // Store the results back into the dot product vector

  dotProduct += ( dotProductVector[0] + dotProductVector[1] );

  if((num_bytes >> 2) != 0) {
    dotProduct += (*a) * (*b);
  }

  *result = dotProduct;
}  

#endif /*LV_HAVE_SSE3*/

#ifdef LV_HAVE_SSE4_1

#include <smmintrin.h>

static inline void volk_32fc_x2_dot_prod_32fc_a_sse4_1(lv_32fc_t* result, const lv_32fc_t* input, const lv_32fc_t* taps, unsigned int num_bytes) {
  volk_32fc_x2_dot_prod_32fc_a_sse3(result, input, taps, num_bytes);
  // SSE3 version runs twice as fast as the SSE4.1 version, so turning off SSE4 version for now
   /* 
    __m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7, real0, real1, im0, im1;
    float *p_input, *p_taps;
    __m64 *p_result;

    p_result = (__m64*)result;
    p_input = (float*)input;
    p_taps = (float*)taps;

    static const __m128i neg = {0x000000000000000080000000};

    int i = 0;
  
    int bound = (num_bytes >> 5);
    int leftovers = (num_bytes & 24) >> 3;

    real0 = _mm_sub_ps(real0, real0);
    real1 = _mm_sub_ps(real1, real1);
    im0 = _mm_sub_ps(im0, im0);
    im1 = _mm_sub_ps(im1, im1);
  
    for(; i < bound; ++i) {
  
    
    xmm0 = _mm_load_ps(p_input);
    xmm1 = _mm_load_ps(p_taps);
    
    p_input += 4;
    p_taps += 4;
    
    xmm2 = _mm_load_ps(p_input);
    xmm3 = _mm_load_ps(p_taps);
    
    p_input += 4;
    p_taps += 4;
    
    xmm4 = _mm_unpackhi_ps(xmm0, xmm2);
    xmm5 = _mm_unpackhi_ps(xmm1, xmm3);
    xmm0 = _mm_unpacklo_ps(xmm0, xmm2);
    xmm2 = _mm_unpacklo_ps(xmm1, xmm3);
    
    //imaginary vector from input
    xmm1 = _mm_unpackhi_ps(xmm0, xmm4);
    //real vector from input
    xmm3 = _mm_unpacklo_ps(xmm0, xmm4);
    //imaginary vector from taps
    xmm0 = _mm_unpackhi_ps(xmm2, xmm5);
    //real vector from taps
    xmm2 = _mm_unpacklo_ps(xmm2, xmm5);
    
    xmm4 = _mm_dp_ps(xmm3, xmm2, 0xf1);
    xmm5 = _mm_dp_ps(xmm1, xmm0, 0xf1);
    
    xmm6 = _mm_dp_ps(xmm3, xmm0, 0xf2);
    xmm7 = _mm_dp_ps(xmm1, xmm2, 0xf2);
    
    real0 = _mm_add_ps(xmm4, real0);
    real1 = _mm_add_ps(xmm5, real1);
    im0 = _mm_add_ps(xmm6, im0);
    im1 = _mm_add_ps(xmm7, im1);
    
    }


    
    
    real1 = _mm_xor_ps(real1, (__m128)neg);
    
  
    im0 = _mm_add_ps(im0, im1);
    real0 = _mm_add_ps(real0, real1);
  
    im0 = _mm_add_ps(im0, real0);
  
    _mm_storel_pi(p_result, im0);
  
    for(i = bound * 4; i < (bound * 4) + leftovers; ++i) {
    
    *result += input[i] * taps[i];
    }
  */
}  

#endif /*LV_HAVE_SSE4_1*/

#endif /*INCLUDED_volk_32fc_x2_dot_prod_32fc_a_H*/
