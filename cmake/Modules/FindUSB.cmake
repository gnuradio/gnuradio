if(NOT LIBUSB_FOUND)
  pkg_check_modules (LIBUSB_PKG libusb-1.0)
  find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h
    PATHS
    ${LIBUSB_PKG_INCLUDE_DIRS}
    /usr/include/libusb-1.0
    /usr/include
    /usr/local/include
  )

  # FreeBSD LibUSB implementation fixes and quirks.
  if(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    # Search for LibUSB
    find_library(LIBUSB_LIBRARIES NAMES usb
      PATHS
      ${LIBUSB_PKG_LIBRARY_DIRS}
      /usr/lib
    )
    # Check if LibUSB has libusb_get_string_descriptor() 
    check_library_exists(
      ${LIBUSB_LIBRARIES}
      "libusb_get_string_descriptor"
      "/usr/lib"
      FREEBSD_HAVE_LIBUSB_GET_STRING_DESCRIPTOR
    )
    if(NOT FREEBSD_HAVE_LIBUSB_GET_STRING_DESCRIPTOR)
      set(FREEBSD_HAVE_NO_LIBUSB_GET_STRING_DESCRIPTOR TRUE CACHE INTERNAL "")
      add_definitions(-DFREEBSD_HAVE_NO_LIBUSB_GET_STRING_DESCRIPTOR)
    endif(NOT FREEBSD_HAVE_LIBUSB_GET_STRING_DESCRIPTOR)
    # Check if LibUSB has libusb_handle_events_completed()
    check_library_exists(
      ${LIBUSB_LIBRARIES}
      "libusb_handle_events_completed"
      "/usr/lib"
      FREEBSD_HAVE_LIBUSB_HANDLE_EVENTS_COMPLETED
    )
    if(NOT FREEBSD_HAVE_LIBUSB_HANDLE_EVENTS_COMPLETED)
      set(FREEBSD_HAVE_NO_LIBUSB_HANDLE_EVENTS_COMPLETED TRUE CACHE INTERNAL "")
      add_definitions(-DFREEBSD_HAVE_NO_LIBUSB_HANDLE_EVENTS_COMPLETED)
    endif(NOT FREEBSD_HAVE_LIBUSB_HANDLE_EVENTS_COMPLETED)

  else(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    find_library(LIBUSB_LIBRARIES NAMES usb
      PATHS
      ${LIBUSB_PKG_LIBRARY_DIRS}
      /usr/lib
      /usr/local/lib
    )
  endif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")

if(LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)
  set(LIBUSB_FOUND TRUE CACHE INTERNAL "libusb-1.0 found")
  message(STATUS "Found libusb-1.0: ${LIBUSB_INCLUDE_DIR}, ${LIBUSB_LIBRARIES}")
else(LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)
  set(LIBUSB_FOUND FALSE CACHE INTERNAL "libusb-1.0 found")
  message(STATUS "libusb-1.0 not found.")
endif(LIBUSB_INCLUDE_DIR AND LIBUSB_LIBRARIES)

mark_as_advanced(LIBUSB_INCLUDE_DIR LIBUSB_LIBRARIES)

endif(NOT LIBUSB_FOUND)
