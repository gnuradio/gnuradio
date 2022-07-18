test('qa_conjugate', py3, args : files('qa_conjugate.py'), env: TEST_ENV)
test('qa_type_conversions', py3, args : files('qa_type_conversions.py'), env: TEST_ENV)
test('qa_add_mult_div_sub', py3, args : files('qa_add_mult_div_sub.py'), env: TEST_ENV)
test('qa_add_numpy', py3, args : files('qa_add_numpy.py'), env: TEST_ENV)
if (cuda_available and get_option('enable_cuda'))
subdir('cuda')
endif
