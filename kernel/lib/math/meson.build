#### Generate the Sin/Cos Tables
srcs = custom_target('gen_sin_table',
                        input : [],
                        output : ['sine_table.h'], 
                        command : ['python3', join_paths(meson.current_source_dir(),'gen_sine_table.py'),
                            '--output', '@OUTPUT0@'],  
                        install : false)

kernel_deps += declare_dependency(sources : [srcs[0]] )

