
# Download, setup and install a copy of Python
set(PYTHON_VERSION "3.11.4")
set(PY_DOWNLOAD_LINK "https://www.paraview.org/files/dependencies/python-for-wheels")
set(PY_FILENAME "python-${PYTHON_VERSION}-windows-x86_64.zip")
set(PYTHON_DIR "Python-${PYTHON_VERSION}")
set(PYTHON_PLAT_EXT "-windows-x86_64")

# download python
file(DOWNLOAD "${PY_DOWNLOAD_LINK}/${PY_FILENAME}"
  "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
  STATUS download_status
  EXPECTED_HASH "SHA256=699df2d656c7227c3ba93d640255cd875e3d92e1a475f5c59408c6125515165f"
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

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}/ DESTINATION Python311 COMPONENT pythonapi)

configure_file(${CMAKE_SOURCE_DIR}/release/win32/launcher.wxs.in ${CMAKE_BINARY_DIR}/launcher.wxs)

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
    install(DIRECTORY ${MODULE_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
    if(${module} STREQUAL "gi")
      # Gi requires the GTK c++ binaries to function, install them with the python module
      # for a self contained, relocatable Python311 distro
      message(STATUS "Installing GTK to ${GR_PYTHON_DIR}/${module}")
      install(DIRECTORY ${GTK_ROOT} DESTINATION ${GR_PYTHON_DIR}/${module} COMPONENT pythonapi)
    endif()
  endforeach()
  # Need to install numpy.libs as well as numpy
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
  "import site
import os
print(os.path.join(site.getsitepackages()[1], \"numpy.libs\"))
"
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE NUMPY_LIB_DIR
  )
  file(TO_CMAKE_PATH ${NUMPY_LIB_DIR} NUMPY_LIB_DIR)
  install(DIRECTORY ${NUMPY_LIB_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
endif()

# We also want to install a symlink of our custom Python to the bin dir
install(CODE [=[
execute_process(
  COMMAND ${PYTHON_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/release/resources/make_py_link.py
  WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
  )
]=])