blocks_headers = [
    'api.h'
]

install_headers(blocks_headers, subdir : 'gnuradio/blocks')

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake_conf.set('module', 'blocks')
cmake.configure_package_config_file(
  name : 'gnuradio-blocks',
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
             name : 'libgnuradio-blocks',
             filebase : 'gnuradio-blocks',
             description : 'GNU Radio General Purpose Blocks')
