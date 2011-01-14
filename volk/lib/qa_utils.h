#ifndef VOLK_QA_UTILS_H
#define VOLK_QA_UTILS_H

#include <stdlib.h>
#include <string>
#include <volk/volk.h>

float uniform(void);
void random_floats(float *buf, unsigned n);

bool run_volk_tests(const int[], void(*)(), std::string, float, int, int);

#define VOLK_RUN_TESTS(func, tol, len, iter) BOOST_CHECK_EQUAL(run_volk_tests(func##_arch_defs, (void (*)())func##_manual, std::string(#func), tol, len, iter), 0)

typedef void (*volk_fn_2arg)(void *, void *, unsigned int, const char*);
typedef void (*volk_fn_3arg)(void *, void *, void *, unsigned int, const char*);
typedef void (*volk_fn_4arg)(void *, void *, void *, void *, unsigned int, const char*);

#endif //VOLK_QA_UTILS_H
