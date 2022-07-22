audio_sources += [
    'alsa_internal.cc'
]

alsa_dep = dependency('alsa', required: true)

audio_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, python3_embed_dep, alsa_dep]

block_cpp_args = ['-DHAVE_CPU']

incdir = include_directories(['../include/gnuradio/audio','../include'])
gnuradio_blocklib_audio_lib = library('gnuradio-blocklib-audio', 
    audio_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : audio_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_audio_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_audio_lib,
                       dependencies : audio_deps)
