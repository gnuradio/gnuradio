
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <iostream>
#include <immintrin.h>


#define POLY0(x, c0) _mm_set1_ps(c0)
#define POLY1(x, c0, c1) _mm_add_ps(_mm_mul_ps(POLY0(x, c1), x), _mm_set1_ps(c0))
#define POLY2(x, c0, c1, c2) _mm_add_ps(_mm_mul_ps(POLY1(x, c1, c2), x), _mm_set1_ps(c0))
#define POLY3(x, c0, c1, c2, c3) _mm_add_ps(_mm_mul_ps(POLY2(x, c1, c2, c3), x), _mm_set1_ps(c0))
#define POLY4(x, c0, c1, c2, c3, c4) _mm_add_ps(_mm_mul_ps(POLY3(x, c1, c2, c3, c4), x), _mm_set1_ps(c0))
#define POLY5(x, c0, c1, c2, c3, c4, c5) _mm_add_ps(_mm_mul_ps(POLY4(x, c1, c2, c3, c4, c5), x), _mm_set1_ps(c0))

#define LOG_POLY_DEGREE 3

void print128_num(__m128i var)
{
    uint32_t *val = (uint32_t*) &var;//can also use uint32_t instead of 16_t
    printf("Vector of Ints: %x %x %d %d \n", 
           val[3], val[2], val[1], val[0]);
}

void print128f_num(__m128 var)
{
    float *val = (float*) &var;//can also use uint32_t instead of 16_t
    printf("Vector of Floats: %f %f %f %f \n", 
           val[3], val[2], val[1], val[0]);
}

static inline void volk_32f_log2_32f_a_avx(float* bVector, float* aVector, unsigned int num_points){

 float* bPtr = bVector;
 float* aPtr = aVector;
 int exp1 = 0x7f800000;

 __m128 aVal, bVal, charac, mantissa, frac;
 __m128i bias = _mm_set1_epi32(127);
 __m128i aVali = _mm_castps_si128(aVal);
 __m128i exp = _mm_set1_epi32(exp1);
 aVal = _mm_load_ps(aPtr); 
 exp = _mm_and_si128(aVali, exp);
 exp = _mm_srli_epi32(exp, 23);
 exp = _mm_sub_epi32(exp, bias);
 charac = _mm_cvtepi32_ps(exp);

 // Now to extract mantissa
 int frac1 = 0x007fffff;
 __m128i fracMask = _mm_set1_epi32(frac1);
 frac = _mm_castsi128_ps(fracMask);
 frac = _mm_and_ps(aVal, frac);
 __m128 leadingOne = _mm_set1_ps(1.0f);
 frac = _mm_or_ps(leadingOne, frac);


 // This is where MIT Licensed code starts
 #if LOG_POLY_DEGREE == 6
   mantissa = POLY5( frac, 3.1157899f, -3.3241990f, 2.5988452f, -1.2315303f,  3.1821337e-1f, -3.4436006e-2f);
#elif LOG_POLY_DEGREE == 5
   mantissa = POLY4( frac, 2.8882704548164776201f, -2.52074962577807006663f, 1.48116647521213171641f, -0.465725644288844778798f, 0.0596515482674574969533f);
#elif LOG_POLY_DEGREE == 4
   mantissa = POLY3( frac, 2.61761038894603480148f, -1.75647175389045657003f, 0.688243882994381274313f, -0.107254423828329604454f);
#elif LOG_POLY_DEGREE == 3
   mantissa = POLY2( frac, 2.28330284476918490682f, -1.04913055217340124191f, 0.204446009836232697516f);
#else
#error
#endif

 mantissa = _mm_mul_ps(mantissa, _mm_sub_ps(frac, leadingOne));

 bVal = _mm_add_ps(charac, mantissa);

 _mm_store_ps(bPtr, bVal);
}


int main(){

clock_t start, end;
double time;
float dummy;
//float input[] = {0.5, 1.0, 4.0, 9.5};
float* input = new float(4*sizeof(float));
input[0] = 0.125;
input[1] = 2.4;
input[2] = 8.0;
input[3] = 16.0;
float* output = new float(4*sizeof(float));

start = clock();
for(int i = 0; i < 10000000; i++)
	volk_32f_log2_32f_a_avx(output, input, 4);
end = clock();
time = 1000 * (double)(end-start);
std::cout << "my time is " << time << std::endl;

start = clock();
for(int j = 0; j < 40000000; j++)
	dummy = log2(input[0]);
end = clock();
time = 1000 * (double)(end-start);
std::cout << "cmath time is " << time << std::endl;


for(int i =0; i < 4; i ++){ 
	std::cout << input[i] << "\t" << output[i];
        std::cout << " cmath : \t " << log2(input[i]) << std::endl;
}

delete input;
delete output;
return 0;

}
