deps = [gnuradio_gr_dep, gr_kernel_fft_lib_dep, gr_kernel_math_lib_dep]

srcs = [
    'fft_filter.cc',
    'fir_filter.cc',
    'firdes.cc',
    'iir_filter.cc',
    'mmse_fir_interpolator_ff.cc',
    'moving_averager.cc',
    'pfb_arb_resampler.cc',
    'pm_remez.cc',
    'polyphase_filterbank.cc'
]

incdir = include_directories(['../include'])
gr_kernel_filter_lib = library('gnuradio-kernel-filter', 
    srcs, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : deps)

gr_kernel_filter_lib_dep = declare_dependency(include_directories : incdir,
					   link_with : gr_kernel_filter_lib,
                       dependencies : deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'kernel-filter')
cmake.configure_package_config_file(
  name : 'gnuradio-kernel-filter',
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
             name : 'libgnuradio-kernel-filter',
             filebase : 'gnuradio-kernel-filter',
             description : 'GNU Radio Kernel - filter')