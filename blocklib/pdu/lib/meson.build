pdu_sources += []
pdu_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, python3_embed_dep]

link_args = []
block_cpp_args = ['-DHAVE_CPU']

incdir = include_directories(['../include/gnuradio/pdu','../include'])
gnuradio_blocklib_pdu_lib = library('gnuradio-blocklib-pdu', 
    pdu_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : pdu_deps,
    link_args : link_args,
    cpp_args : block_cpp_args)

gnuradio_blocklib_pdu_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_pdu_lib,
                       dependencies : pdu_deps)
