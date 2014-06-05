
#include <stdio.h>
#include <math.h>
#include <inttypes.h>
#include <iostream>
#include <immintrin.h>

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

 __m128 aVal, bVal, charac, mantissa;
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
 __m128 frac = _mm_castsi128_ps(fracMask);
 frac = _mm_and_ps(aVal, frac);
 __m128 leadingOne = _mm_set1_ps(1.0f);
 frac = _mm_or_ps(leadingOne, frac);

 
 // Add the decimal bits one by one, can use LUT to speed up
 __m128 compareResults, compareResultsNot, compareTo, fracByTwo, allOnes, a, b, c;
 __m128i allOnesi; 
 allOnesi = _mm_set1_epi32(0xffffffff);
 allOnes = _mm_castsi128_ps(allOnesi);
 compareTo = _mm_set1_ps(2.0);

 mantissa = _mm_set1_ps(0.5);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 //b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_sub_ps(frac, a);
 c = _mm_and_ps(compareResults, mantissa);
 charac = _mm_add_ps(charac, c);


 mantissa = _mm_set1_ps(0.25);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 //b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_sub_ps(frac, a);
 charac = _mm_add_ps(charac, _mm_and_ps(compareResults, mantissa));


 mantissa = _mm_set1_ps(0.125);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_add_ps(a, b);
 charac = _mm_add_ps(charac, _mm_and_ps(compareResults, mantissa));

/* 
 mantissa = _mm_set1_ps(0.0625);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_add_ps(a, b);
 charac = _mm_add_ps(charac, _mm_and_ps(compareResults, mantissa));
 
 mantissa = _mm_set1_ps(0.03125);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_add_ps(a, b);
 charac = _mm_add_ps(charac, _mm_and_ps(compareResults, mantissa));

 mantissa = _mm_set1_ps(0.015625);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_add_ps(a, b);
 charac = _mm_add_ps(charac, _mm_and_ps(compareResults, mantissa));
 
 mantissa = _mm_set1_ps(0.0078125);
 frac = _mm_mul_ps(frac, frac);
 compareResults = _mm_cmpge_ps(frac, compareTo); // ge compare
 compareResultsNot = _mm_andnot_ps(compareResults, allOnes);
 fracByTwo = _mm_div_ps(frac, compareTo);
 a = _mm_and_ps(compareResults, fracByTwo);
 b = _mm_and_ps(compareResultsNot, frac);  
 frac = _mm_add_ps(a, b);
 charac = _mm_add_ps(charac, _mm_and_ps(compareResults, mantissa));
*/ 
 _mm_store_ps(bPtr, charac);
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
