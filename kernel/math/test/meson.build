
# GR namespace tests
qa_srcs = ['qa_fast_atan2f',
           'qa_fxpt_nco',
           'qa_fxpt_vco',
           'qa_fxpt',
           'qa_math',
           'qa_sincos'
           
          ]
deps = [
        gr_kernel_math_lib_dep,
        # gr_kernel_fft_lib_dep,
        # gr_kernel_filter_lib_dep,
        # gr_kernel_analog_lib_dep,
        # gr_kernel_digital_lib_dep,
        # gnuradio_gr_dep,
                gtest_dep,
                ]

foreach qa : qa_srcs
    e = executable(qa, 
        qa + '.cc', 
        link_language : 'cpp',
        dependencies: deps, 
        install : false)
    test(qa, e, env: TEST_ENV)
endforeach

if GR_ENABLE_PYTHON
    test('Random', py3, args : files('qa_random.py'), env: TEST_ENV)
endif

