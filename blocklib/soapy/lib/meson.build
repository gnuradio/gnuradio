soapy_sources += ['block_impl.cc']
soapy_deps += [gnuradio_gr_dep, soapysdr_dep, volk_dep, fmt_dep, pmtf_dep]

block_cpp_args = ['-DHAVE_CPU']

incdir = include_directories(['../include/gnuradio/soapy','../include'])
gnuradio_blocklib_soapy_lib = library('gnuradio-blocklib-soapy', 
    soapy_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : soapy_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_soapy_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_soapy_lib,
                       dependencies : soapy_deps)
