# incdir = include_directories('../include')

if (CLI11_dep.found())
srcs = ['bm_copy.cc']
executable('bm_nbt_copy', 
    srcs, 
    # include_directories : incdir, 
    link_language : 'cpp',
    dependencies: [gnuradio_gr_dep,
                   gnuradio_blocklib_blocks_dep,
                   gnuradio_blocklib_streamops_dep,
                   gnuradio_scheduler_nbt_dep,
                   CLI11_dep], 
    install : true)

srcs = ['bm_fanout.cc']
executable('bm_nbt_fanout', 
    srcs, 
    # include_directories : incdir,
    link_language : 'cpp', 
    dependencies: [gnuradio_gr_dep,
                   gnuradio_blocklib_blocks_dep,
                   gnuradio_blocklib_streamops_dep,
                   gnuradio_scheduler_nbt_dep,
                   CLI11_dep], 
    install : true)

srcs = ['bm_nop.cc']
executable('bm_nbt_nop', 
    srcs, 
    # include_directories : incdir, 
    link_language : 'cpp',
    dependencies: [gnuradio_gr_dep,
                   gnuradio_blocklib_blocks_dep,
                   gnuradio_blocklib_streamops_dep,
                   gnuradio_scheduler_nbt_dep,
                   CLI11_dep], 
    install : true)

srcs = ['bm_msg_forward.cc']
executable('bm_msg_forward', 
    srcs, 
    # include_directories : incdir, 
    link_language : 'cpp',
    dependencies: [gnuradio_gr_dep,
                   gnuradio_blocklib_blocks_dep,
                   gnuradio_blocklib_streamops_dep,
                   gnuradio_scheduler_nbt_dep,
                   CLI11_dep], 
    install : true)


if cuda_dep.found() and get_option('enable_cuda')
    subdir('cuda')
endif
endif