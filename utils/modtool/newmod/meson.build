
###############################################################################
# If this is an OOT module, the following top level project things are needed
###############################################################################
project('gr4-newmod', 'cpp', 
  version : '0.0.0',
  license : 'GPLv3',
  default_options : ['cpp_std=c++20'])

# Import python3 meson module which is used to find the Python dependencies.
# Python3 is required for code generation
py3 = import('python').find_installation('python3')
py3_version = py3.language_version()
if py3_version.version_compare('< 3.6')
  error('Invalid python version!?')
endif

GR_ENABLE_PYTHON = false
if (get_option('enable_python'))
python3_dep = dependency('python3', required : get_option('enable_python'))
python3_embed_dep = dependency('python3-embed', required : get_option('enable_python'))
pybind11_dep = dependency('pybind11', required : get_option('enable_python'))
incdir_numpy = run_command(py3,
  ['-c', 'import os; os.chdir(".."); import numpy; print(numpy.get_include())'],
  check : true
).stdout().strip()

GR_ENABLE_PYTHON = true
endif

py3 = import('python').find_installation('python3')
json_dep = dependency('nlohmann_json')

if (get_option('enable_testing'))
    TEST_ENV = environment()
    TEST_ENV.prepend('LD_LIBRARY_PATH', 
      join_paths( meson.build_root(),'blocklib','newmod',get_option('libdir')),
    )
    TEST_ENV.prepend('PYTHONPATH', join_paths(meson.build_root(),'blocklib/newmod/python'))
endif

prefix = get_option('prefix')
# Escape spaces
prefix_parts = prefix.split(' ')
prefix = '\ '.join(prefix_parts)

cuda_available = add_languages('cuda', required : false)
IMPLEMENT_CUDA = cuda_available and get_option('enable_cuda')
IMPLEMENT_CPU = true
###############################################################################
# For OOT modules we need script path from gnuradio, so let's look for
# gnuradio-runtime already here
###############################################################################
gnuradio_gr_dep = dependency('gnuradio-runtime')
###############################################################################
# IF Out of Tree Module
# The script dir is part of the gnuradio installation, not of the oot tree,
# so we have to find the install prefix of gnuradio
# Even if we build an In-Tree module, it's the correct location 
###############################################################################
share_dir = join_paths(gnuradio_gr_dep.get_pkgconfig_variable('prefix'),'share')
SCRIPTS_DIR = join_paths(share_dir, 'utils','blockbuilder','scripts' )
###############################################################################
# Else In-Tree module
###############################################################################
# share_dir = meson.project_source_root()

###############################################################################
# Dependencies
###############################################################################
volk_dep = dependency('volk', version : '>=2.2')
fmt_dep = dependency('fmt', method: 'cmake', modules: ['fmt::fmt'])
spdlog_dep = dependency('spdlog')
pmt_dep = dependency('pmt', version : '>= 0.0.2')

if GR_ENABLE_PYTHON
run_command('python3', join_paths(SCRIPTS_DIR,'gen_meson.py'), 
  join_paths(meson.project_source_root(),'blocklib'), '--enable_python', check: true)
else
run_command('python3', join_paths(SCRIPTS_DIR,'gen_meson.py'), 
  join_paths(meson.project_source_root(),'blocklib'), check: true)
endif
  
subdir('blocklib/newmod')

