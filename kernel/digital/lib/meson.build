deps = [gnuradio_gr_dep, gr_kernel_math_lib_dep]

srcs = [
    'constellation.cc'
]

incdir = include_directories(['../include'])
gr_kernel_digital_lib = library('gnuradio-kernel-digital', 
    srcs, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : deps)

gr_kernel_digital_lib_dep = declare_dependency(include_directories : incdir,
					   link_with : gr_kernel_digital_lib,
                       dependencies : deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'kernel-digital')
cmake.configure_package_config_file(
  name : 'gnuradio-kernel-digital',
  input : join_paths(meson.source_root(),'cmake','Modules','gnuradioConfigModule.cmake.in'),
  install_dir : get_option('prefix') / get_option('libdir') / 'cmake' / 'gnuradio',
  configuration : cmake_conf
)

pkg = import('pkgconfig')
libs = []     # the library/libraries users need to link against
h = ['.'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : 'libgnuradio-kernel-digital',
             filebase : 'gnuradio-kernel-digital',
             description : 'GNU Radio Kernel - digital')
