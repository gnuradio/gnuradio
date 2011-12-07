# - try to find Qwt libraries and include files
# QWT_INCLUDE_DIR where to find qwt_plot.h, etc.
# QWT_LIBRARIES libraries to link against
# QWT_FOUND If false, do not try to use Qwt

find_path ( QWT_INCLUDE_DIRS qwt_plot.h /usr/include/qwt-qt4 /usr/include/qwt /opt/local/include/qwt )
	  
find_library ( QWT_LIBRARIES NAMES qwt-qt4 qwt)

# handle the QUIETLY and REQUIRED arguments and set QWT_FOUND to TRUE if 
# all listed variables are TRUE
include ( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Qwt DEFAULT_MSG QWT_LIBRARIES QWT_INCLUDE_DIRS )
