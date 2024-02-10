find_package(Python REQUIRED)

function(PYCHECK_MODULE module)
    execute_process(
        COMMAND ${Python_EXECUTABLE} -c
"try:\n  import ${module}\n  print('TRUE')\nexcept ImportError:\n  print('FALSE')"
    OUTPUT_VARIABLE IMPORTED)

    set(${module}_IMPORTED ${IMPORTED} PARENT_SCOPE)
endfunction()