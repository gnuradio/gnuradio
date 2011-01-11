#include <volk/volk_runtime.h>
#include <volk/volk.h>
#include <qa_32fc_32f_multiply_aligned16.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <qa_utils.h>

#define	ERR_DELTA	(1e-4)

void qa_32fc_32f_multiply_aligned16::t1() {

  const int vlen = 2046;
  const int ITERS = 100000;

  volk_environment_init();
  int ret;
  clock_t start, end;
  double total;
  std::complex<float>* input;
  float * taps;
  int i;
  std::vector<std::string> archs;
  archs.push_back("generic");
#ifdef LV_HAVE_SSE3
  archs.push_back("sse3");
#endif
#ifdef LV_HAVE_ORC
  archs.push_back("orc");
#endif
  
  std::vector<std::complex<float>* > results;

  ret = posix_memalign((void**)&input, 16, vlen * 2 * sizeof(float));
  ret = posix_memalign((void**)&taps, 16, vlen * sizeof(float));
  
  for(i=0; i < archs.size(); i++) {
      std::complex<float> *ptr;
      ret = posix_memalign((void**)&ptr, 16, vlen * 2 * sizeof(float));
      if(ret) {
          printf("Couldn't allocate memory\n");
          exit(1);
      }
      results.push_back(ptr);
  }

  random_floats((float*)input, vlen * 2);
  random_floats(taps, vlen);
  
  printf("32fc_32f_multiply_aligned16\n");

  for(i=0; i < archs.size(); i++) {
    start = clock();
    for(int count = 0; count < ITERS; ++count) {
      volk_32fc_32f_multiply_aligned16_manual(results[i], input, taps, vlen, archs[i].c_str());
    }
    end = clock();
    total = (double)(end-start)/(double)CLOCKS_PER_SEC;
    printf("%s_time: %f\n", archs[i].c_str(), total);
  }

  for(i=0; i < vlen; i++) {
      int j = 1;
      for(j; j < archs.size(); j++) {
          assertcomplexEqual(results[0][i], results[j][i], ERR_DELTA);
      }
  }

  free(input);
  free(taps);
  for(i=0; i < archs.size(); i++) {      
    free(results[i]);
  }
}

