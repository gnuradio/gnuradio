incdir = [
    '../include',
]


gr_deps = [yaml_dep, spdlog_dep, threads_dep, fmt_dep, rt_dep, pmtf_dep, libdl_dep, cppzmq_dep, json_dep]

libdir = get_option('libdir')
prefix = get_option('prefix')
# r = run_command('python3',['-c','"import os; print(os.path.relpath(\'@0@\',\'@1@\'))"'.format(prefix, join_paths(prefix,libdir))], check:true, capture:true)
r = run_command('python3', join_paths(meson.project_source_root(),'utils','relative_path.py'), '--dir1', prefix, '--dir2', join_paths(prefix,libdir), check:true)
prefix_relative_to_lib = r.stdout().strip()
message('prefix_relative_to_lib')
message(prefix_relative_to_lib)


cdata = configuration_data()
cdata.set('VERSION', meson.project_version())
cdata.set('prefix_relative_to_lib', prefix_relative_to_lib)
cdata.set('GR_PREFSDIR_relative_to_prefix', 'etc/gnuradio/conf.d')
constants_file = configure_file(input: 'constants.cc.in',
                          output: 'constants.cc',
                          configuration: cdata,
                          install: false)


runtime_sources = [
  constants_file,
  'node.cc',
  'block.cc',
  'port.cc',
  'buffer.cc',
  'buffer_sm.cc',
  'buffer_management.cc',
  'buffer_cpu_simple.cc',
  'realtime.cc',
  'runtime.cc',
  'runtime_monitor.cc',
  'runtime_proxy.cc',
  'scheduler_message.cc',
  'thread.cc',
  'parameter_types.cc',
  'edge.cc',
  'graph.cc',
  'graph_utils.cc',
  'flat_graph.cc',
  'flowgraph.cc',
  # 'logging.cc',
  'logger.cc',
  'nodeid_generator.cc',
  'pagesize.cc',
  'sys_paths.cc',
  'buffer_cpu_vmcirc.cc',
  'buffer_cpu_vmcirc_sysv_shm.cc',
  # mmap requires librt - FIXME - handle this a conditional dependency
  'buffer_cpu_vmcirc_mmap_shm_open.cc',
  'buffer_net_zmq.cc',
  'rpc_client_interface.cc',
  'sptr_magic.cc',
  'hier_block.cc',
  'prefs.cc',
  'tag.cc'
]

if USE_CUDA
  runtime_sources += [
    'buffer_cuda.cc',
    'buffer_cuda_pinned.cc',
    'buffer_cuda_sm.cc'
  ]
  gr_deps += cuda_dep
endif

if (pybind11_dep.found() and python3_dep.found() and python3_embed_dep.found() and get_option('enable_python'))
runtime_sources += [
  'python_block.cc',
  'pyblock_detail.cc'
]
gr_deps += [python3_dep, python3_embed_dep, pybind11_dep]
endif

cpp_args = []
compiler = meson.get_compiler('cpp')
if compiler.has_header('sys/time.h')
  cpp_args += '-DHAVE_SYS_TIME_H'
endif
if compiler.has_header('sys/types.h')
  cpp_args += '-DHAVE_SYS_TYPES_H'
endif
if compiler.has_header('sys/socket.h')
  cpp_args += '-DHAVE_SYS_SOCKET_H'
endif
if compiler.has_header('io.h')
  cpp_args += '-DHAVE_IO_H'
endif
if compiler.has_header('sys/mman.h')
  cpp_args += '-DHAVE_SYS_MMAN_H'
endif
if compiler.has_header('sys/ipc.h')
  cpp_args += '-DHAVE_SYS_IPC_H'
endif
if compiler.has_header('sys/shm.h')
  cpp_args += '-DHAVE_SYS_SHM_H'
endif
if compiler.has_header('signal.h')
  cpp_args += '-DHAVE_SIGNAL_H'
endif
if compiler.has_header('netinet/in.h')
  cpp_args += '-DHAVE_NETINET_IN_H'
endif
if compiler.has_header('arpa/inet.h')
  cpp_args += '-DHAVE_ARPA_INET_H'
endif
if compiler.has_header('unistd.h')
  cpp_args += '-DHAVE_UNISTD_H'
endif
if compiler.has_header('malloc.h')
  cpp_args += '-DHAVE_MALLOC_H'
endif


code = '''#include <signal.h>
    int main(){sigaction(0, 0, 0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SIGACTION')
  cpp_args += '-DHAVE_SIGACTION'
endif

code = '''#include <unistd.h>
    int main(){sysconf(0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SYSCONF')
  cpp_args += '-DHAVE_SYSCONF'
endif

code = '''#include <unistd.h>
     int main(){getpagesize(); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_GETPAGESIZE')
  cpp_args += '-DHAVE_GETPAGESIZE'
endif

code = '''#include <Winbase.h>
     int main(){Sleep(0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SSLEEP')
  cpp_args += '-DHAVE_SSLEEP'
endif

code = '''#include <stdlib.h>
     int main(){posix_memalign(0, 0, 0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_POSIX_MEMALIGN')
  cpp_args += '-DHAVE_POSIX_MEMALIGN'
endif

code = '''#include <malloc.h>
     int main(){valloc(0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_VALLOC')
  cpp_args += '-DHAVE_VALLOC'
endif


# if(APPLE)
#     # Apple's malloc has been 16 byte aligned since at least 10.4
#     ADD_DEFINITIONS(-DALIGNED_MALLOC=16)
# else()
#     ADD_DEFINITIONS(-DALIGNED_MALLOC=0)
# endif()

code = '''#include <signal.h>
     int main(){pthread_sigmask(0, 0, 0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_PTHREAD_SIGMASK', dependencies : [threads_dep])
  cpp_args += '-DHAVE_PTHREAD_SIGMASK'
endif

code = '''#include <windows.h>
    int main(){
        HANDLE handle;
        int size;
        LPCTSTR lpName;
        handle = CreateFileMapping(
                         INVALID_HANDLE_VALUE,    // use paging file
                         NULL,                    // default security
                         PAGE_READWRITE,          // read/write access
                         0,                       // max. object size
                         size,                    // buffer size
                         lpName);                 // name of mapping object
        return 0;
'''
if compiler.compiles(code, name : 'HAVE_CREATEFILEMAPPING', dependencies : [threads_dep])
  cpp_args += '-DHAVE_CREATEFILEMAPPING'
endif

code = '''#include <sys/types.h>
     #include <sys/mman.h>
     int main(){shm_open(0, 0, 0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_SHM_OPEN', dependencies : [rt_dep])
  cpp_args += '-DHAVE_SHM_OPEN'
endif

code = '''#include <sys/mman.h>
    int main(){mmap(0, 0, 0, 0, 0, 0); return 0;}
'''
if compiler.compiles(code, name : 'HAVE_MMAP')
  cpp_args += '-DHAVE_MMAP'
endif

code = '''#include <pthread.h>
          int main(){
            pthread_t pthread;
            pthread_setschedparam(pthread,  0, 0);
            return 0;
        }
'''
if compiler.compiles(code, name : 'HAVE_PTHREAD_SETSCHEDPARAM', dependencies : [threads_dep])
  cpp_args += '-DHAVE_PTHREAD_SETSCHEDPARAM'
endif

code = '''#include <sched.h>
            int main(){
            pid_t pid;
            sched_setscheduler(pid, 0, 0);
            return 0;
        }
'''
if compiler.compiles(code, name : 'HAVE_SCHED_SETSCHEDULER', dependencies : [threads_dep])
  cpp_args += '-DHAVE_SCHED_SETSCHEDULER'
endif

code = '''#include <filesystem>
            int main(){
            namespace fs = std::filesystem;
            return 0;
        }
'''
if compiler.compiles(code, name : 'HAVE_STD_FILESYSTEM')
  cpp_args += '-DHAVE_STD_FILESYSTEM'
endif


gnuradio_gr_lib = library('gnuradio-runtime', 
    runtime_sources, 
    install : true, 
    dependencies : gr_deps,
    link_language : 'cpp',
    include_directories : incdir,
    cpp_args : cpp_args)

gnuradio_gr_dep = declare_dependency(include_directories : incdir,
					   dependencies: gr_deps, # have to disable this for blocklib_blocks_cuda_cu to link
					   link_with : gnuradio_gr_lib)


# TODO - export this as a subproject of gnuradio

prefix = get_option('prefix')
# Escape spaces
prefix_parts = prefix.split(' ')
prefix = '\ '.join(prefix_parts)

conf = configuration_data()
conf.set('prefix', prefix)
conf.set('exec_prefix', '${prefix}')
conf.set('libdir', join_paths('${prefix}',get_option('libdir')))
conf.set('includedir', join_paths('${prefix}',get_option('includedir')))
conf.set('LIBVER', '0.0.0')

cmake_conf = configuration_data()
cmake_conf.set('libdir', join_paths(prefix,get_option('libdir')))
cmake.configure_package_config_file(
  name : 'gnuradio-runtime',
  input : join_paths(meson.source_root(),'cmake','Modules','gnuradioConfig.cmake.in'),
  install_dir : get_option('prefix') / 'lib' / 'cmake' / 'gnuradio',
  configuration : cmake_conf
)

pkg = import('pkgconfig')
libs = []     # the library/libraries users need to link against
h = ['.'] # subdirectories of ${prefix}/${includedir} to add to header path
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : 'libgnuradio-runtime',
             filebase : 'gnuradio-runtime',
             install_dir : get_option('prefix') / 'lib' / 'pkgconfig',
             description : 'GNU Radio Core Components')

