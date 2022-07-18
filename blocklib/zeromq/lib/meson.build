zeromq_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, cppzmq_dep]
zeromq_sources += ['base.cc', 'tag_headers.cc']
block_cpp_args = ['-DHAVE_CPU']

incdir = include_directories(['../include/gnuradio/zeromq','../include'])
gnuradio_blocklib_zeromq_lib = library('gnuradio-blocklib-zeromq', 
    zeromq_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : zeromq_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_zeromq_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_zeromq_lib,
                       dependencies : zeromq_deps)
