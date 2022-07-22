analog_deps += [gnuradio_gr_dep, gnuradio_blocklib_filter_dep, volk_dep, fmt_dep, pmtf_dep, gr_kernel_lib_dep]

# analog_sources += 'kernel/agc.cc'
block_cpp_args = ['-DHAVE_CPU', '-DHAVE_HIER']
# if cuda_dep.found() and get_option('enable_cuda')
#     block_cpp_args += '-DHAVE_CUDA'

#     gnuradio_blocklib_analog_cu = library('gnuradio-blocklib-analog-cu', 
#         analog_cu_sources, 
#         include_directories : incdir, 
#         install : true, 
#         dependencies : [cuda_dep])

#     gnuradio_blocklib_analog_cu_dep = declare_dependency(include_directories : incdir,
#                         link_with : gnuradio_blocklib_analog_cu,
#                         dependencies : cuda_dep)

#     analog_deps += [gnuradio_blocklib_analog_cu_dep, cuda_dep]

# endif

incdir = include_directories(['../include/gnuradio/analog','../include'])
gnuradio_blocklib_analog_lib = library('gnuradio-blocklib-analog', 
    analog_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : analog_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_analog_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_analog_lib,
                       dependencies : analog_deps)
