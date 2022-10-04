soapy_sources += ['block_impl.cc']

soapysdr_dep = dependency('SoapySDR', version : '>=0.7', required : true)

soapy_deps += [gnuradio_gr_dep, soapysdr_dep, volk_dep, fmt_dep, pmtf_dep]

block_cpp_args = ['-DHAVE_CPU']

incdir = include_directories(['../include/gnuradio/soapy','../include'])
gnuradio_blocklib_soapy_lib = library('gnuradio-blocklib-soapy', 
    soapy_sources, 
    include_directories : incdir, 
    install : true,
    link_language: 'cpp',
    dependencies : soapy_deps,
    cpp_args : block_cpp_args)

gnuradio_blocklib_soapy_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_blocklib_soapy_lib,
                       dependencies : soapy_deps)

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'soapy')
cmake.configure_package_config_file(
  name : 'gnuradio-soapy',
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
             name : 'libgnuradio-soapy',
             filebase : 'gnuradio-soapy',
             description : 'GNU Radio Soapy Module')