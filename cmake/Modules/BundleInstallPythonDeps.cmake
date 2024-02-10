
set(PYTHON_VERSION "3.11.0" CACHE STRING "Version of Python to build.")
set(PY_DOWNLOAD_LINK "https://www.paraview.org/files/dependencies")
set(PY_FILENAME "Python-${PYTHON_VERSION}-win64.tar.xz")
set(PYTHON_DIR "Python-${PYTHON_VERSION}")

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

execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz
  "${CMAKE_CURRENT_BINARY_DIR}/${PY_FILENAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
  RESULT_VARIABLE res)
if(NOT res EQUAL 0)
message(FATAL_ERROR "Extraction of ${PY_FILENAME} failed.")
endif()
message(STATUS "Extracted ${PY_FILENAME}.")

configure_file(${PROJECT_SOURCE_DIR}/release/win32/bundle.wxs.in ${CMAKE_BINARY_DIR}/bundle.wxs)

include(FetchWheels.cmake)
