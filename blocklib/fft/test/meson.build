###################################################
#    QA
###################################################

if get_option('enable_testing')
    test('qa_fft', py3, args : files('qa_fft.py'), env: TEST_ENV)
    if (cuda_available and get_option('enable_cuda'))
    test('qa_cufft', py3, args : files('qa_cufft.py'), env: TEST_ENV)
    endif

endif
