# incdir = include_directories('../../include')

srcs = ['bm_copy.cc']
executable('bm_nbt_cuda_copy', 
    srcs, 
    # include_directories : incdir, 
    link_language : 'cpp',
    dependencies: [gnuradio_gr_dep,
                   cuda_dep,
                   gnuradio_blocklib_blocks_dep,
                   gnuradio_blocklib_streamops_dep,
                   gnuradio_scheduler_nbt_dep,
                   CLI11_dep], 
    install : true)
