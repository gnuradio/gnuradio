find_package(Sphinx REQUIRED)

set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR})
set(SPHINX_BUILD ${CMAKE_CURRENT_BINARY_DIR})

add_custom_target(sphinx ALL
                COMMAND
                ${SPHINX_EXECUTABLE} -b html
                -Dbreathe_projects.gnuradio=${PROJECT_BINARY_DIR}/docs/doxygen/xml
                ${SPHINX_SOURCE} ${SPHINX_BUILD}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                COMMENT "Generating documentation with Sphinx")