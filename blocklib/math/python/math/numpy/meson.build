
srcs = ['__init__.py']
# This is for testing without installing - just copy to the build dir
foreach s: srcs
configure_file(copy: true,
    input: s,
    output: s
)
endforeach