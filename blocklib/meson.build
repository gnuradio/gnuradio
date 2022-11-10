
gen_meson_args = join_paths(meson.project_source_root(),'utils','blockbuilder','scripts','gen_meson.py')
if GR_ENABLE_PYTHON
if GR_PURELIB 
run_command('python3', gen_meson_args, join_paths(meson.project_source_root(),'blocklib'), '--enable_python', '--enable_purelib', check: true)
else
run_command('python3', gen_meson_args, join_paths(meson.project_source_root(),'blocklib'), '--enable_python', check: true)
endif
else
run_command('python3', gen_meson_args, join_paths(meson.project_source_root(),'blocklib'), check: true)
endif
  

if get_option('enable_gr_math') 
subdir('math')
endif
if get_option('enable_gr_blocks')
subdir('blocks')
endif
if get_option('enable_gr_streamops')
subdir('streamops')
endif
