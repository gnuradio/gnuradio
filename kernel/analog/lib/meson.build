deps = [gnuradio_gr_dep]

srcs = [
    'agc.cc'
]

incdir = include_directories(['../include'])
gr_kernel_analog_lib = library('gnuradio-kernel-analog', 
    srcs, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : deps)

gr_kernel_analog_lib_dep = declare_dependency(include_directories : incdir,
					   link_with : gr_kernel_analog_lib,
                       dependencies : deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'kernel-analog')
cmake.configure_package_config_file(
  name : 'gnuradio-kernel-analog',
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
             name : 'libgnuradio-kernel-analog',
             filebase : 'gnuradio-kernel-analog',
             description : 'GNU Radio Kernel - analog')