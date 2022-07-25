run_command('python3', join_paths(meson.project_source_root(),'utils','blockbuilder','scripts','gen_meson.py'), 
  join_paths(meson.project_source_root(),'blocklib'), check: true)

if get_option('enable_gr_math') 
subdir('math')
endif
if get_option('enable_gr_blocks')
subdir('blocks')
endif

if get_option('enable_gr_fec')
subdir('fec')
endif
if get_option('enable_gr_fft')
subdir('fft')
endif
if get_option('enable_gr_filter')
subdir('filter')
endif
if get_option('enable_gr_analog')
subdir('analog')
endif
if get_option('enable_gr_streamops')
subdir('streamops')
endif
if get_option('enable_gr_fileio')
subdir('fileio')
endif
if get_option('enable_gr_zeromq')
subdir('zeromq')
endif
if get_option('enable_gr_audio')
subdir('audio')
endif
if get_option('enable_gr_soapy') and soapysdr_dep.found()
subdir('soapy')
endif