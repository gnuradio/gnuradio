
###############################################################################
# If this is an OOT module, the following top level project things are needed
###############################################################################
project('gr4-newmod', 'cpp',
  version : '0.0.0',
  license : 'GPLv3',
  default_options : ['cpp_std=c++17'])

# Import python3 meson module which is used to find the Python dependencies.
# Python3 is required for code generation
pymod = import('python')
# This gets the Python interpreter that will run natively for building
# find_program is the only way to specify native : true and pymod.find_installation
# always defers to a cross file (if used) for finding python
py3_build = find_program('python3', 'python', native : true, required: true, version : '>=3.6')

GR_ENABLE_PYTHON = false
if (get_option('enable_python'))
# This gets the target Python and dependency, which may be different than the
# Python used for running scripts during build (e.g. cross-compilation)
py3 = pymod.find_installation(required : get_option('enable_python'))
py3_version = py3.language_version()
if py3_version.version_compare('< 3.6')
  error('Invalid python version!?')
endif
python3_dep = py3.dependency(required: get_option('enable_python'))
python3_embed_dep = py3.dependency(embed : true, required : get_option('enable_python'))
pybind11_dep = dependency('pybind11', required : get_option('enable_python'))
incdir_numpy = run_command(py3,
  ['-c', 'import os; os.chdir(".."); import numpy; print(numpy.get_include())'],
  check : true
).stdout().strip()

GR_ENABLE_PYTHON = true
endif

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
# IF Out of Tree Module
###############################################################################
share_dir = join_paths(prefix,'share')
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
pmtf_dep = dependency('pmtf', version : '>= 0.0.2')
gnuradio_gr_dep = dependency('gnuradio-runtime')

if GR_ENABLE_PYTHON
run_command(py3_build, join_paths(SCRIPTS_DIR,'gen_meson.py'),
  join_paths(meson.project_source_root(),'blocklib'), '--enable_python', check: true)
else
run_command(py3_build, join_paths(SCRIPTS_DIR,'gen_meson.py'),
  join_paths(meson.project_source_root(),'blocklib'), check: true)
endif

subdir('blocklib/newmod')

