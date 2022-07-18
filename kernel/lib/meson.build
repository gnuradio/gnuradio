fftw_dep = dependency('fftw3f')

kernel_deps = [volk_dep, gnuradio_gr_dep, fftw_dep]
kernel_sources = [
    'analog/agc.cc',
    'fft/window.cc',
    'fft/fftw_fft.cc',
    'filter/iir_filter.cc',
    'filter/fft_filter.cc',
    'filter/fir_filter.cc',
    'filter/firdes.cc',
    'filter/mmse_fir_interpolator_ff.cc',
    'filter/moving_averager.cc',
    'filter/pfb_arb_resampler.cc',
    'filter/polyphase_filterbank.cc',
    'math/fast_atan2f.cc',
    'math/fxpt.cc',
    'math/random.cc',
    'digital/constellation.cc'
]

compiler = meson.get_compiler('cpp')
cpp_args = []
code = '''#define _GNU_SOURCE
     #include <math.h>
     int main(){double x, sin, cos; sincos(x, &sin, &cos); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SINCOS')
  cpp_args += '-DHAVE_SINCOS'
endif

code = '''#define _GNU_SOURCE
     #include <math.h>
     int main(){float x, sin, cos; sincosf(x, &sin, &cos); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SINCOSF')
  cpp_args += '-DHAVE_SINCOSF'
endif

code = '''#include <math.h>
     int main(){sinf(0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SINF')
  cpp_args += '-DHAVE_SINF'
endif

code = '''#include <math.h>
     int main(){cosf(0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_COSF')
  cpp_args += '-DHAVE_COSF'
endif


subdir('math')

incdir = include_directories(['../include'])
gr_kernel_lib = library('gnuradio-kernel', 
    kernel_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : kernel_deps)

gr_kernel_lib_dep = declare_dependency(include_directories : incdir,
					   link_with : gr_kernel_lib,
                       dependencies : kernel_deps)

prefix = get_option('prefix')
# Escape spaces
prefix_parts = prefix.split(' ')
prefix = '\ '.join(prefix_parts)

conf = configuration_data()
conf.set('prefix', prefix)
conf.set('exec_prefix', '${prefix}')
conf.set('libdir', join_paths('${prefix}',get_option('libdir')))
conf.set('includedir', join_paths('${prefix}',get_option('includedir')))
conf.set('LIBVER', '0.0.1')

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'kernel')
cmake.configure_package_config_file(
  name : 'gnuradio-kernel',
  input : join_paths(meson.source_root(),'cmake','Modules','gnuradioConfigModule.cmake.in'),
  install_dir : get_option('prefix') / 'lib' / 'cmake' / 'gnuradio',
  configuration : cmake_conf
)

pkg = import('pkgconfig')
libs = []     # the library/libraries users need to link against
h = ['.'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : 'libgnuradio-kernel',
             filebase : 'gnuradio-kernel',
             install_dir : get_option('prefix') / 'lib' / 'pkgconfig',
             description : 'GNU Radio Kernel Library (Math and Signal Processing)')
