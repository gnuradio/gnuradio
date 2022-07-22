###################################################
#    QA
###################################################

if get_option('enable_testing')
    test('qa_delay', py3, args : files('qa_delay.py'), env: TEST_ENV)
    test('qa_type_conversions', py3, args : files('qa_type_conversions.py'), env: TEST_ENV)
    test('qa_keep_m_in_n', py3, args : files('qa_keep_m_in_n.py'), env: TEST_ENV)
    test('qa_probe_signal', py3, args : files('qa_probe_signal.py'), env: TEST_ENV)
    test('qa_interleave', py3, args : files('qa_interleave.py'), env: TEST_ENV)
    test('qa_selector', py3, args : files('qa_selector.py'), env: TEST_ENV)
    # if (cuda_available and get_option('enable_cuda'))
    # test('qa_cufft', find_program('qa_cufft.py'), env: TEST_ENV)
    # endif
endif
