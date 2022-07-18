# SPDX-FileCopyrightText: 2020 Josh Morman 
#

project('gnuradio', 'cpp', 
  version : '4.0.0.0-preview0',
  license : 'GPLv3',
  default_options : ['cpp_std=c++17'])

cc = meson.get_compiler('cpp')
rt_dep = cc.find_library('rt', required : false)
libdl_dep = cc.find_library('dl')

c_available = add_languages('c', required : true)
cuda_available = add_languages('cuda', required : false)
USE_CUDA = cuda_available and get_option('enable_cuda')
IMPLEMENT_CUDA = USE_CUDA
IMPLEMENT_CPU = true

SCRIPTS_DIR=join_paths(meson.project_source_root(),'utils','blockbuilder','scripts')

volk_dep = dependency('volk', version : '>=2.2')
yaml_dep = dependency('yaml-cpp', version : '>=0.6')
fmt_dep = dependency('fmt', method: 'cmake', modules: ['fmt::fmt'])
spdlog_dep = dependency('spdlog', method: 'cmake', modules : ['spdlog::spdlog'])
python3_dep = dependency('python3', required : get_option('enable_python'))
python3_embed_dep = dependency('python3-embed', required : get_option('enable_python'))

# Import python3 meson module which is used to find the Python dependencies.
py3_inst = import('python').find_installation('python3')
py3_mod = import('python3')
py3 = py3_mod.find_python()
py3_version = py3_mod.language_version()
if py3_version.version_compare('< 3.6')
  error('Invalid python version!?')
endif

pybind11_dep = dependency('pybind11', required : get_option('enable_python'))
threads_dep = dependency('threads')
gtest_dep = dependency('gtest', main : true, version : '>=1.10', required : get_option('enable_testing'))
cuda_dep = dependency('cuda', version : '>=10.1', required : USE_CUDA, modules : ['cufft'])

libpmtf = subproject('pmt')
pmtf_dep = libpmtf.get_variable('pmtf_dep')

libCLI11 = subproject('CLI11')
CLI11_dep = libCLI11.get_variable('CLI11_dep')

libjson = subproject('json')
json_dep = libjson.get_variable('nlohmann_json_dep')

libcpphttp = subproject('cpp-httplib')
cpphttp_dep = libcpphttp.get_variable('cpp_httplib_dep')

cmake = import('cmake')
libcppzmq = cmake.subproject('cppzmq')
cppzmq_dep = libcppzmq.dependency('cppzmq')

if USE_CUDA
  libcusp = subproject('cusp')
  cusp_dep = libcusp.get_variable('cusp_dep')
endif

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
    )
    TEST_ENV.prepend('PYTHONPATH', join_paths(meson.build_root(),'python')+':'+join_paths(meson.build_root(), 'subprojects/pmt/python') )

endif

subdir('gr')
subdir('kernel')
subdir('blocklib')
subdir('schedulers')
subdir('utils')
if (get_option('enable_python'))
  subdir('python/gnuradio')
endif
if (get_option('enable_testing'))
    subdir('test')
endif
if (get_option('enable_bench'))
    subdir('bench')
endif
subdir('docs')