include(GrPython)

find_package(pybind11 REQUIRED)

macro(GR_PYBIND_MAKE name updir filter files)

    configure_file(${PROJECT_SOURCE_DIR}/docs/doxygen/pydoc_macros.h
                   ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

    pybind11_add_module(${name}_python ${files})

    set(MODULE_NAME ${name})
    if(${name} STREQUAL gr)
        set(MODULE_NAME "runtime")
    endif()

    if(ENABLE_DOXYGEN)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "sub" "--json_path"
                ${PROJECT_BINARY_DIR}/docs/doxygen/gnuradio_docstrings.json
                "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings "--output_dir"
                ${CMAKE_CURRENT_BINARY_DIR} "--filter" ${filter}
            COMMENT "Adding docstrings into ${name} pybind headers ..."
            DEPENDS gnuradio_docstrings)
        add_custom_target(${name}_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
    else(ENABLE_DOXYGEN)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "copy" "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings
                "--output_dir" ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Copying ${name} docstring templates as pybind headers ...")
        add_custom_target(${name}_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
    endif(ENABLE_DOXYGEN)

    target_include_directories(
        ${name}_python
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/lib
                ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include)
    target_link_libraries(
        ${name}_python PRIVATE ${Boost_LIBRARIES} pybind11::pybind11 Python::Module
                               Python::NumPy gnuradio-${MODULE_NAME})
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${name}_python PRIVATE
            -Wno-unused-variable
            -Wno-error=deprecated-declarations
        ) # disable warnings for docstring templates
    endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_dependencies(${name}_python ${name}_docstrings)

endmacro(GR_PYBIND_MAKE)

macro(GR_PYBIND_MAKE_CHECK_HASH name updir filter files)

    configure_file(${PROJECT_SOURCE_DIR}/docs/doxygen/pydoc_macros.h
                   ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

    list(APPEND regen_targets "")
    foreach(file ${files})

        execute_process(
            COMMAND
                "${PYTHON_EXECUTABLE}"
                ${PROJECT_SOURCE_DIR}/gr-utils/bindtool/scripts/header_utils.py "all"
                ${CMAKE_CURRENT_SOURCE_DIR}/${file}
            OUTPUT_VARIABLE flags_all)
        list(GET flags_all 0 flag_auto)
        list(GET flags_all 1 flag_pygccxml)
        list(GET flags_all 2 header_filename)
        list(GET flags_all 3 header_file_hash)
        # message(STATUS ${file} ":" ${flag_auto} ":" ${flag_pygccxml} ":" ${header_filename} ":" ${header_file_hash})

        if(NOT ${header_filename} STREQUAL "None"
        )# If no header filename is specified, don't bother checking for a rebuild
            if(${name} STREQUAL gr)
                set(header_full_path
                    ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include/gnuradio/${header_filename}
                )
            else()
                set(header_full_path
                    ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include/gnuradio/${name}/${header_filename}
                )
            endif()

            file(MD5 ${header_full_path} calc_hash)
            # message(STATUS ${ii} " " ${calc_hash} " " ${saved_hash})
            if(NOT ${calc_hash} STREQUAL ${header_file_hash})
                # check the file and see if bindings should be automatically regenerated
                if(flag_auto STREQUAL "False"
                )# the regex match was not found, manual bindings
                    # if (NOT bindtool_use_pygccxml STREQUAL )
                    message(FATAL_ERROR "Python bindings for " ${header_filename}
                                        " are out of sync")
                else()
                    if(flag_pygccxml STREQUAL "True")
                        if(NOT PYGCCXML_FOUND)
                            message(
                                FATAL_ERROR "Python bindings for " ${header_filename}
                                            " require pygccxml for automatic regeneration"
                            )
                        endif()
                    endif()

                    message(STATUS "Regenerating Bindings in-place for "
                                   ${header_filename})

                    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${file}.regen_status)
                    # Automatically regenerate the bindings
                    add_custom_command(
                        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}}/${file}
                        COMMAND
                            "${PYTHON_EXECUTABLE}"
                            ${PROJECT_SOURCE_DIR}/gr-utils/bindtool/scripts/bind_intree_file.py
                            "--output_dir" ${CMAKE_CURRENT_SOURCE_DIR}/.. "--prefix"
                            ${CMAKE_INSTALL_PREFIX} "--module" ${name} "--filename"
                            ${header_full_path} "--status"
                            ${CMAKE_CURRENT_BINARY_DIR}/${file}.regen_status
                            "--flag_automatic" ${flag_auto} "--flag_pygccxml"
                            ${flag_pygccxml} "--include"
                            ${extra_includes} # Use additional includes, if available
                        COMMENT "Automatic generation of pybind11 bindings for "
                                ${header_full_path})
                    add_custom_target(${file}_regen_bindings ALL
                                      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}}/${file})
                    list(APPEND regen_targets ${file}_regen_bindings)
                endif()

            endif()
        endif()
    endforeach()

    pybind11_add_module(${name}_python ${files})

    set(MODULE_NAME ${name})
    if(${name} STREQUAL gr)
        set(MODULE_NAME "runtime")
    endif()
    if(${name} STREQUAL video_sdl)
        set(MODULE_NAME "video-sdl")
    endif()

    if(ENABLE_DOXYGEN)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "sub" "--json_path"
                ${PROJECT_BINARY_DIR}/docs/doxygen/gnuradio_docstrings.json
                "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings "--output_dir"
                ${CMAKE_CURRENT_BINARY_DIR} "--filter" ${filter}
            COMMENT "Adding docstrings into ${name} pybind headers ..."
            DEPENDS gnuradio_docstrings)
        add_custom_target(${name}_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
    else(ENABLE_DOXYGEN)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "copy" "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings
                "--output_dir" ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Copying ${name} docstring templates as pybind headers ...")
        add_custom_target(${name}_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
    endif(ENABLE_DOXYGEN)

    target_include_directories(
        ${name}_python
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/lib
                ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include)

    target_link_libraries(
        ${name}_python PRIVATE ${Boost_LIBRARIES} pybind11::pybind11 Python::Module
                               Python::NumPy gnuradio-${MODULE_NAME})
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${name}_python PRIVATE
            -Wno-unused-variable
            -Wno-error=deprecated-declarations
        ) # disable warnings for docstring templates
    endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(NOT SNDFILE_FOUND AND ${name} STREQUAL blocks)
        target_compile_options(${name}_python PRIVATE -DNO_WAVFILE)
    endif()
    add_dependencies(${name}_python ${name}_docstrings ${regen_targets})

endmacro(GR_PYBIND_MAKE_CHECK_HASH)

macro(GR_PYBIND_MAKE_OOT name updir filter files)

    list(APPEND regen_targets "")
    foreach(file ${files})

        execute_process(
            COMMAND "${PYTHON_EXECUTABLE}" ${CMAKE_CURRENT_SOURCE_DIR}/header_utils.py
                    "all" ${CMAKE_CURRENT_SOURCE_DIR}/${file} OUTPUT_VARIABLE flags_all)
        list(GET flags_all 0 flag_auto)
        list(GET flags_all 1 flag_pygccxml)
        list(GET flags_all 2 header_filename)
        list(GET flags_all 3 header_file_hash)

        # message(STATUS ${file} ":" ${flag_auto} ":" ${flag_pygccxml} ":" ${header_filename} ":" ${header_file_hash})

        # If no header filename is specified (set to empty or "None")
        #  OR If hash is set to 0, then ignore the check as well
        if(NOT ${header_filename} STREQUAL "None" AND NOT ${header_file_hash} STREQUAL
                                                      "0")
            set(header_full_path
                ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include/${name}/${header_filename}
            )# NOTE OOT version does not have gnuradio/ here
            if(NOT EXISTS ${header_full_path})
                set(header_full_path
                    ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include/gnuradio/${name}/${header_filename}
                )
            endif()

            file(MD5 ${header_full_path} calc_hash)
            # message(STATUS ${ii} " " ${calc_hash} " " ${saved_hash})
            if(NOT ${calc_hash} STREQUAL ${header_file_hash})
                # check the file and see if bindings should be automatically regenerated
                if(flag_auto STREQUAL "False"
                )# the regex match was not found, manual bindings
                    # if (NOT bindtool_use_pygccxml STREQUAL )
                    message(FATAL_ERROR "Python bindings for " ${header_filename}
                                        " are out of sync")
                else()
                    if(flag_pygccxml STREQUAL "True")
                        if(NOT PYGCCXML_FOUND)
                            message(
                                FATAL_ERROR "Python bindings for " ${header_filename}
                                            " require pygccxml for automatic regeneration"
                            )
                        endif()
                    endif()

                    message(STATUS "Regenerating Bindings in-place for "
                                   ${header_filename})

                    file(REMOVE ${CMAKE_CURRENT_BINARY_DIR}/${file}.regen_status)
                    # Automatically regenerate the bindings
                    string(REPLACE ";" "," extra_include_list "${extra_includes}"
                    )#Convert ';' separated extra_includes to ',' separated list format
                    string(REPLACE ";" "," defines "${define_symbols}"
                    )#Convert ';' separated define_symbols to ',' separated list format
                    add_custom_command(
                        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}}/${file}
                        COMMAND
                            "${PYTHON_EXECUTABLE}"
                            ${CMAKE_CURRENT_SOURCE_DIR}/bind_oot_file.py "--output_dir"
                            ${CMAKE_CURRENT_SOURCE_DIR}/.. "--prefix"
                            ${CMAKE_INSTALL_PREFIX} "--module" ${name} "--filename"
                            ${header_full_path} "--status"
                            ${CMAKE_CURRENT_BINARY_DIR}/${file}.regen_status
                            "--flag_automatic" ${flag_auto} "--flag_pygccxml"
                            ${flag_pygccxml} "--defines"
                            ${defines} #Add preprocessor defines
                            "--include"
                            ${extra_include_list} #Some oots may require additional includes
                        COMMENT "Automatic generation of pybind11 bindings for "
                                ${header_full_path})
                    add_custom_target(${file}_regen_bindings ALL
                                      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}}/${file})
                    list(APPEND regen_targets ${file}_regen_bindings)
                endif()

            endif()
        endif()
    endforeach()

    configure_file(${PROJECT_SOURCE_DIR}/docs/doxygen/pydoc_macros.h
                   ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

    pybind11_add_module(${name}_python ${files})

    set(MODULE_NAME ${name})
    if(${name} STREQUAL gr)
        set(MODULE_NAME "runtime")
    endif()

    if(ENABLE_DOXYGEN)

        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/extracted_docstrings.json
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "scrape" "--xml_path" ${PROJECT_BINARY_DIR}/docs/doxygen/xml "--json_path"
                ${CMAKE_CURRENT_BINARY_DIR}/extracted_docstrings.json
            COMMENT "Scraping generated documentation for docstrings ..."
            DEPENDS gnuradio-${MODULE_NAME} doxygen_target)

        add_custom_target(extracted_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/extracted_docstrings.json)

        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "sub" "--json_path" ${CMAKE_CURRENT_BINARY_DIR}/extracted_docstrings.json
                "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings "--output_dir"
                ${CMAKE_CURRENT_BINARY_DIR} "--filter" ${filter}
            COMMENT "Adding docstrings into ${name} pybind headers ..."
            DEPENDS extracted_docstrings)
        add_custom_target(${name}_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
    else(ENABLE_DOXYGEN)
        add_custom_command(
            OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/docstring_status
            COMMAND
                ${PYTHON_EXECUTABLE} ${PROJECT_SOURCE_DIR}/docs/doxygen/update_pydoc.py
                "copy" "--bindings_dir" ${CMAKE_CURRENT_SOURCE_DIR}/docstrings
                "--output_dir" ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Copying ${name} docstring templates as pybind headers ...")
        add_custom_target(${name}_docstrings ALL
                          DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/docstring_status)
    endif(ENABLE_DOXYGEN)

    target_include_directories(
        ${name}_python
        PRIVATE ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/lib
                ${CMAKE_CURRENT_SOURCE_DIR}/${updir}/include)
    target_link_libraries(
        ${name}_python PRIVATE ${Boost_LIBRARIES} pybind11::pybind11 Python::Module
                               Python::NumPy gnuradio-${MODULE_NAME})
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${name}_python PRIVATE
            -Wno-unused-variable
            -Wno-error=deprecated-declarations
        ) # disable warnings for docstring templates
    endif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_dependencies(${name}_python ${name}_docstrings ${regen_targets})

endmacro(GR_PYBIND_MAKE_OOT)
