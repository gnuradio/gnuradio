fileio_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep]
fileio_sources += 'file_sink_base.cc'
block_cpp_args = ['-DHAVE_CPU']
# if cuda_dep.found() and get_option('enable_cuda')
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_fileio_cu = library('gnuradio-blocklib-fileio-cu', 
#         fileio_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_fileio_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_fileio_cu,
#                         dependencies : cuda_dep)

#     fileio_deps += [gnuradio_blocklib_fileio_cu_dep, cuda_dep]

# endif

incdir = include_directories(['../include/gnuradio/fileio','../include'])
gnuradio_blocklib_fileio_lib = library('gnuradio-blocklib-fileio', 
    fileio_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : fileio_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_fileio_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_fileio_lib,
                       dependencies : fileio_deps)
