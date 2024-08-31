
# Download, setup and install a copy of Python
set(PYTHON_VERSION "3.11.4" CACHE STRING "Version of Python to build." FORCE)
set(PY_DOWNLOAD_LINK "https://www.paraview.org/files/dependencies/python-for-wheels/")
set(PYTHON_DIR "Python-${PYTHON_VERSION}")
if(WIN32)
  set(PY_FILENAME "python-${PYTHON_VERSION}-windows-x86_64.zip")
  set(PYTHON_PLAT_EXT "-windows-x86_64")
elseif(APPLE)
  # This says arm64, but the bianry bundled is a macosx universal binary
  # so this will run on x86_64 as well
  set(PY_FILENAME "python-${PYTHON_VERSION}-macos-arm64.tar.xz")
  set(PYTHON_PLAT_EXT "-macos-arm64")
endif()

if(APPLE)
  set(PY_DOWNLOAD_HASH fdb882b53b18675811ca04ad6f5279a586f6f705abd6971c7732c49d214e91b9)
elseif(WIN32)
  set(PY_DOWNLOAD_HASH f6aeebc6d1ff77418678ed5612b64ce61be6bc9ef3ab9c291ac557abb1783420)
endif()
# download python
file(DOWNLOAD "${PY_DOWNLOAD_LINK}/${PY_FILENAME}"
  "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
  STATUS download_status
  EXPECTED_HASH "SHA256=${PY_DOWNLOAD_HASH}"
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

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PYTHON_DIR}${PYTHON_PLAT_EXT}/ DESTINATION Python${PYTHON_SHORT_VER} COMPONENT pythonapi)

if(WIN32)
  configure_file(${PROJECT_SOURCE_DIR}/release/win32/bundle.wxs.in ${CMAKE_BINARY_DIR}/bundle.wxs)
endif()
# install python module dependencies
get_property(GR_PYTHON_DEPS_SET GLOBAL PROPERTY GR_PYTHON_VENDOR_DEPS SET)
if(GR_PYTHON_DEPS_SET AND ENABLE_PYTHON)
  get_property(GR_PYTHON_DEPS GLOBAL PROPERTY GR_PYTHON_VENDOR_DEPS)
  foreach(module ${GR_PYTHON_DEPS})
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c
                    "import importlib
import inspect
import os
mod = importlib.import_module(\"${module}\")
print(os.path.dirname(inspect.getfile(mod)))
"
      OUTPUT_STRIP_TRAILING_WHITESPACE
      OUTPUT_VARIABLE MODULE_DIR
    )
    file(TO_CMAKE_PATH ${MODULE_DIR} MODULE_DIR)
    message(STATUS "Installing ${module} to ${GR_PYTHON_DIR}/${module}")
    install(DIRECTORY ${MODULE_DIR} DESTINATION ${GR_PYTHON_DIR} COMPONENT pythonapi)
    if(${module} STREQUAL "gi" AND WIN32)
      # Gi requires the GTK c++ binaries to function, install them with the python module
      # for a self contained, relocatable Python${PYTHON_SHORT_VER} distro
      message(STATUS "Installing GTK to ${GR_PYTHON_DIR}/${module}")
      install(DIRECTORY ${GTK_ROOT} DESTINATION ${GR_PYTHON_DIR}/${module} COMPONENT pythonapi)
    endif()
    install(DIRECTORY ${MODULE_DIR} DESTINATION ${GR_PYTHON_DIR})
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


configure_file(${CMAKE_SOURCE_DIR}/release/resources/make_py_link.py ${CMAKE_BINARY_DIR}/make_py_link.py)
# We also want to install a symlink of our custom Python to the bin dir
install(CODE [=[
execute_process(
  COMMAND ${PYTHON_EXECUTABLE}
    ${CMAKE_BINARY_DIR}/make_py_link.py
  WORKING_DIRECTORY ${CMAKE_INSTALL_PREFIX}
  )
]=])