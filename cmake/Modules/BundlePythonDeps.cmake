
# Download, setup and install a copy of Python
set(PYTHON_VERSION "3.11.4" CACHE STRING "Version of Python to build.")
set(PY_DOWNLOAD_LINK "https://www.paraview.org/files/dependencies/python-for-wheels")
set(PY_FILENAME "Python-${PYTHON_VERSION}-windows-x86_64.zip")
set(PYTHON_DIR "Python-${PYTHON_VERSION}")

# download python
file(DOWNLOAD "${PY_DOWNLOAD_LINK}/${PY_FILENAME}"
  "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
  STATUS download_status
  EXPECTED_HASH "SHA256=f6aeebc6d1ff77418678ed5612b64ce61be6bc9ef3ab9c291ac557abb1783420"
)
list(GET download_status 0 res)
if(res)
  list(GET download_status 1 err)
  message(FATAL_ERROR "Failed to download ${PY_FILENAME} ${err}")
endif()
message(STATUS "Successfully downloaded ${PY_FILENAME}")

# extract
execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz
  "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  RESULT_VARIABLE res)
if(NOT res EQUAL 0)
message(FATAL_ERROR "Extraction of ${PY_FILENAME} failed.")
endif()
message(STATUS "Extracted ${PY_FILENAME}.")

get_filename_component(PY_DIR ${PY_FILENAME} NAME_WE)
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PY_DIR} DESTINATION Python311/)
set(PYTHON_BUNDLE_DIR Python311)
configure_file(${CMAKE_SOURCE_DIR}/release/win32/launcher.wxs.in ${CMAKE_BINARY_DIR}/launcher.wxs.in)

# install python module dependencies
get_property(GR_PYTHON_DEPS_SET GLOBAL PROPERTY GR_PYTHON_VENDOR_DEPS SET)
if(GR_PYTHON_DEPS_SET AND ENABLE_PYTHON)
  get_property(GR_PYTHON_DEPS GLOBAL PROPERTY GR_PYTHON_VENDOR_DEPS)
  foreach(module ${GR_PYTHON_DEPS})
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                    "import importlib
import inspect
import os
os.add_dll_directory('${GTK_BIN_DIR}')
mod = importlib.import_module(\"${module}\")
print(os.path.dirname(inspect.getfile(mod)))
"
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE MODULE_DIR
    )
    file(TO_CMAKE_PATH ${MODULE_DIR} MODULE_DIR)
    message(STATUS "Installing ${module} to ${GR_PYTHON_DIR}/${module}")
    install(DIRECTORY ${MODULE_DIR} DESTINATION ${GR_PYTHON_DIR}/${module})
  endforeach()
endif()
