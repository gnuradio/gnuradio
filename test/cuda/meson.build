###################################################
#    QA
###################################################

if get_option('enable_testing')
    srcs = ['qa_scheduler_nbt_cuda_copy.cc']
    e = executable('qa_scheduler_nbt_cuda_copy', 
        srcs, 
        # include_directories : incdir, 
        link_language : 'cpp',
        dependencies: [gnuradio_gr_dep,
                    cuda_dep,
                    gnuradio_blocklib_blocks_dep,
                    gnuradio_blocklib_streamops_dep,
                    gnuradio_scheduler_nbt_dep,
                    gtest_dep], 
        install : true)
    test('NBT Cuda Copy Tests', e)

endif

