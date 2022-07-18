subdir('include/gnuradio')
subdir('include/moodycamel')
subdir('lib')

if (get_option('enable_python'))
    subdir('python/gr')
endif

install_data(sources : 'gr.conf.yml', install_dir : 'etc/gnuradio/conf.d')
