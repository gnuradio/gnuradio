blocks_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep]

block_cpp_args = ['-DHAVE_CPU']
link_args = []
# if USE_CUDA
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_blocks_cu = library('gnuradio-blocklib-blocks-cu', 
#         blocks_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_blocks_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_blocks_cu,
#                         dependencies : cuda_dep)

#     blocks_deps += [gnuradio_blocklib_blocks_cu_dep, cuda_dep, cusp_dep]
# endif 

incdir = include_directories(['../include/gnuradio/blocks','../include'])
gnuradio_blocklib_blocks_lib = library('gnuradio-blocklib-blocks', 
    blocks_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : blocks_deps,
    link_args : link_args,
    cpp_args : block_cpp_args)

gnuradio_blocklib_blocks_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_blocks_lib,
                       dependencies : blocks_deps)
