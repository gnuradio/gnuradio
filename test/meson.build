
# GR namespace tests
qa_srcs = ['qa_default_runtime',
           'qa_scheduler_nbt',
           'qa_block_grouping',
           'qa_single_mapped_buffers',
           'qa_message_ports',
           'qa_tags',
           'qa_zmq_buffers',
           'qa_hier_block'
          ]
deps = [gnuradio_gr_dep,
                gnuradio_blocklib_blocks_dep,
                gnuradio_blocklib_streamops_dep,
                gnuradio_blocklib_math_dep,
                gnuradio_scheduler_nbt_dep,
                gtest_dep,
                gr_kernel_lib_dep]

foreach qa : qa_srcs
    e = executable(qa, 
        qa + '.cc', 
        include_directories : incdir, 
        link_language : 'cpp',
        dependencies: deps, 
        install : false)
    test(qa, e, env: TEST_ENV)
endforeach

test('Basic Python', py3, args : files('qa_basic.py'), env: TEST_ENV)
test('Block Parameters', py3, args : files('qa_parameters.py'), env: TEST_ENV)
test('Python Blocks', py3, args : files('qa_python_block.py'), env: TEST_ENV)
test('Hier Block', py3, args : files('qa_hier_block.py'), env: TEST_ENV)

if cuda_dep.found() and get_option('enable_cuda')
    subdir('cuda')
endif
