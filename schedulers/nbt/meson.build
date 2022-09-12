subdir('include/gnuradio/schedulers/nbt')
subdir('lib')
# subdir('test') # tests should only be those that exercise the NBT scheduler specifically
# subdir('bench')
if (get_option('enable_python'))
    subdir('python/gnuradio/schedulers/nbt')
endif

install_data(sources : 'nbt.scheduler.conf.yml', install_dir : 'etc/gnuradio/conf.d')
