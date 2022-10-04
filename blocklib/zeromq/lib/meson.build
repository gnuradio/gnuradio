zeromq_deps += [gnuradio_gr_dep, volk_dep, fmt_dep, pmtf_dep, cppzmq_dep]
zeromq_sources += ['base.cc', 'tag_headers.cc']
block_cpp_args = ['-DHAVE_CPU']

incdir = include_directories(['../include/gnuradio/zeromq','../include'])
gnuradio_blocklib_zeromq_lib = library('gnuradio-blocklib-zeromq', 
    zeromq_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : zeromq_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_zeromq_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_zeromq_lib,
                       dependencies : zeromq_deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'zeromq')
cmake.configure_package_config_file(
  name : 'gnuradio-zeromq',
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
             name : 'libgnuradio-zeromq',
             filebase : 'gnuradio-zeromq',
             description : 'GNU Radio ZeroMQ Module')
