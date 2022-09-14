###################################################
#    QA
###################################################

if GR_ENABLE_PYTHON
    test('qa_fft', py3, args : files('qa_fft.py'), env: TEST_ENV)
    if (IMPLEMENT_CUDA)
        test('qa_cufft', py3, args : files('qa_cufft.py'), env: TEST_ENV)
    endif

endif
