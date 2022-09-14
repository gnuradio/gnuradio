# SPDX-FileCopyrightText: 2020 Josh Morman 
#

project('gnuradio', 'cpp', 
  version : '4.0.0.0-preview0',
  license : 'GPLv3',
  default_options : ['cpp_std=c++20'])

## Compilers
cc = meson.get_compiler('cpp')
rt_dep = cc.find_library('rt', required : false)
libdl_dep = cc.find_library('dl')
c_available = add_languages('c', required : true)
cuda_available = add_languages('cuda', required : false)

IMPLEMENT_CUDA = false # By default
IMPLEMENT_CPU = true

SCRIPTS_DIR=join_paths(meson.project_source_root(),'utils','blockbuilder','scripts')

## Build Dependencies
volk_dep = dependency('volk', version : '>=2.2')
yaml_dep = dependency('yaml-cpp', version : '>=0.6')
fmt_dep = dependency('fmt')
spdlog_dep = dependency('spdlog')
threads_dep = dependency('threads')

# Import python3 meson module which is used to find the Python dependencies.
# Python3 is required for code generation
py3 = import('python').find_installation('python3')
py3_version = py3.language_version()
if py3_version.version_compare('< 3.6')
  error('Invalid python version!?')
endif

if (get_option('enable_testing'))
gtest_dep = dependency('gtest', main : true, version : '>=1.10', required : get_option('enable_testing'))
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

# Python module min versions
# TODO: distinguish between build and runtime requirements
py_check_script = join_paths(meson.project_source_root(),'utils','py_module_check.py')
run_command('python3', py_check_script, 'numpy', '1.22', check:true)
run_command('python3', py_check_script, 'jinja2', '3.0.2', check:true)
run_command('python3', py_check_script, 'yaml', '5.3.1', check:true)
run_command('python3', py_check_script, 'zmq', '22.3.0', check:true)
run_command('python3', py_check_script, 'jsonschema', '3.2.0', check:true)
run_command('python3', py_check_script, 'jsonschema_default', check:true)
GR_ENABLE_PYTHON = true
endif

if (get_option('enable_cuda'))
cuda_dep = dependency('cuda', version : '>=10.1', required : true, modules : ['cufft'])
if not cuda_available
error('CUDA compiler not found, but enable_cuda option specified')
endif
IMPLEMENT_CUDA = true
endif



## Subprojects
libpmtf = subproject('pmt')
pmtf_dep = libpmtf.get_variable('pmtf_dep')

CLI11_dep = dependency('cli11')
json_dep = dependency('nlohmann_json')

cmake = import('cmake')
opt_var = cmake.subproject_options()
opt_var.add_cmake_defines({'CPPZMQ_BUILD_TESTS': false})
libcppzmq = cmake.subproject('cppzmq', options: opt_var)
cppzmq_dep = libcppzmq.dependency('cppzmq')

if IMPLEMENT_CUDA
  libcusp = subproject('cusp')
  cusp_dep = libcusp.get_variable('cusp_dep')
endif


## Test Environment
if (get_option('enable_testing'))
    TEST_ENV = environment()
    TEST_ENV.prepend('LD_LIBRARY_PATH', 
      join_paths( meson.build_root(),'schedulers','nbt','lib'),
      join_paths( meson.build_root(),'runtime','lib'),
      join_paths( meson.build_root(),'blocklib','analog','lib'),
      join_paths( meson.build_root(),'blocklib','blocks','lib'),
      join_paths( meson.build_root(),'blocklib','fec','lib'),
      join_paths( meson.build_root(),'blocklib','fft','lib'),
      join_paths( meson.build_root(),'blocklib','fileio','lib'),
      join_paths( meson.build_root(),'blocklib','filter','lib'),
      join_paths( meson.build_root(),'blocklib','math','lib'),
      join_paths( meson.build_root(),'blocklib','streamops','lib'),
      join_paths( meson.build_root(),'blocklib','soapy','lib'),
    )
    TEST_ENV.prepend('PYTHONPATH', join_paths(meson.build_root(),'python')+':'+join_paths(meson.build_root(), 'subprojects/pmt/python') )
endif

prefix = get_option('prefix')

GR_DATA_DIR = 'share'
GR_PKG_DATA_DIR = join_paths(GR_DATA_DIR,meson.project_name())

# Set location of config/prefs files in /etc
# Special exception if prefix is /usr so we don't make a /usr/etc.
GR_CONF_DIR = 'etc'
isusr = prefix == '/usr'
if isusr
  SYSCONFDIR = join_paths('/',GR_CONF_DIR)
else
  SYSCONFDIR = join_paths(prefix, GR_CONF_DIR)
endif
GR_PKG_CONF_DIR = join_paths(SYSCONFDIR, meson.project_name(), 'conf.d')
GR_PREFSDIR = join_paths(SYSCONFDIR, meson.project_name(), 'conf.d')
GRC_BLOCKS_DIR = join_paths(GR_PKG_DATA_DIR, 'grc/blocks')

## Subdirectories with meson.build
subdir('gr')
subdir('kernel')
subdir('blocklib')
subdir('schedulers')
subdir('utils')
if (get_option('enable_python'))
  subdir('python/gnuradio')
  subdir('grc')
  subdir('domains/grc')
endif
if (get_option('enable_testing'))
    subdir('test')
endif
if (get_option('enable_bench'))
    subdir('bench')
endif
subdir('docs')
