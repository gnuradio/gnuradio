# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_plot.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt

find_path (QWT_INCLUDE_DIRS
	NAMES qwt_plot.h
	PATHS
	/usr/local/include/qwt-qt4
	/usr/local/include/qwt
	/usr/include/qwt-qt4
	/usr/include/qwt
	/opt/local/include/qwt
    /opt/local/lib/qwt.framework/Headers
	/sw/include/qwt
)

find_library (QWT_LIBRARIES
	NAMES qwt-qt4 qwt
	PATHS
	/usr/local/lib
	/usr/lib
	/opt/local/lib
    /opt/local/lib/qwt.framework
	/sw/lib
)

# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS )
MARK_AS_ADVANCED(QWT_LIBRARIES QWT_INCLUDE_DIRS)
