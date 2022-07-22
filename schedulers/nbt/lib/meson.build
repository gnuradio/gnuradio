scheduler_nbt_sources = [
    'graph_executor.cc',
    'thread_wrapper.cc',
    'scheduler_nbt.cc',
]
scheduler_nbt_deps = [gnuradio_gr_dep, threads_dep, fmt_dep, pmtf_dep, cppzmq_dep, yaml_dep]

incdir = include_directories('../include', '../include/gnuradio/schedulers/nbt')
gnuradio_scheduler_nbt_lib = library('gnuradio-scheduler-nbt', 
    scheduler_nbt_sources, include_directories : incdir, 
    install : true,
    link_language : 'cpp',
    dependencies : scheduler_nbt_deps)

gnuradio_scheduler_nbt_dep = declare_dependency(include_directories : incdir,
					   link_with : gnuradio_scheduler_nbt_lib,
                       dependencies : scheduler_nbt_deps )
