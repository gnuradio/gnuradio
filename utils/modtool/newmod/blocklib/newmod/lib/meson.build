newmod_sources += []
newmod_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, python3_embed_dep]

block_cpp_args = ['-DHAVE_CPU']

# if cuda_dep.found() and get_option('enable_cuda')
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_newmod_cu = library('gnuradio-blocklib-newmod-cu', 
#         newmod_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_newmod_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_newmod_cu,
#                         dependencies : cuda_dep)

#     newmod_deps += [gnuradio_blocklib_newmod_cu_dep, cuda_dep]

# endif

incdir = include_directories(['../include/gnuradio/newmod','../include'])
gnuradio_blocklib_newmod_lib = library('gnuradio-blocklib-newmod', 
    newmod_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : newmod_deps,
    link_args : ['-lgnuradio-runtime'],
    cpp_args : block_cpp_args)

gnuradio_blocklib_newmod_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_newmod_lib,
                       dependencies : newmod_deps)
