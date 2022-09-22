
# GR namespace tests
qa_srcs = ['qa_host_buffer',
          ]
deps = [gnuradio_gr_dep,
                gtest_dep,]

foreach qa : qa_srcs
    e = executable(qa, 
        qa + '.cc', 
        include_directories : incdir, 
        link_language : 'cpp',
        dependencies: deps, 
        install : false)
    test(qa, e, env: TEST_ENV)
endforeach