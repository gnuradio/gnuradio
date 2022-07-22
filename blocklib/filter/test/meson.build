###################################################
#    QA
###################################################

if get_option('enable_testing')
    test('qa_fir_filter', find_program('qa_fir_filter.py'), env: TEST_ENV)
    test('qa_iir_filter', find_program('qa_iir_filter.py'), env: TEST_ENV)
    test('qa_pfb_channelizer', find_program('qa_pfb_channelizer.py'), env: TEST_ENV)
    # test('qa_moving_average', find_program('qa_moving_average.py'), env: TEST_ENV)
endif
