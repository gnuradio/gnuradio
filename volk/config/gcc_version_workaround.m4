AC_DEFUN([LV_GCC_VERSION_WORKAROUND], 
[
    case "${host_os}" in
      *cygwin*)
      ;;
      *)
	AC_REQUIRE([LF_CONFIGURE_CXX])
	
	cxx_version=`$CXX --version`

	cxx_major_version=`echo $cxx_version | sed 's/[[^)]]*) \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\).*/\1/'`
	cxx_minor_version=`echo $cxx_version | sed 's/g++ [[^)]]*) \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\).*/\2/'`
	cxx_micro_version=`echo $cxx_version | sed 's/g++ [[^)]]*) \([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\).*/\3/'`
    	
	if test ["$cxx_minor_version" -lt "3"] -o ["$cxx_major_version" -lt "4"]; then
		cxx_proper_version="$cxx_major_version.$cxx_minor_version.$cxx_micro_version"
		

		my_arch=`uname -m`

		if test "${my_arch}" = i686; then
		   my_arch="${my_arch} i586 i486 i386"
		fi
		if test "${my_arch}" = i586; then
		   my_arch = "${my_arch} i686 i386 i486"
		fi
		if test "${my_arch}" = i486; then
		   my_arch = "${my_arch} i686 i386 i586"
		fi
		if test "${my_arch}" = i386; then
		   my_arch = "${my_arch} i686 i586 i486"
		fi
		
		for i in $my_arch
		do
		  if test -n "`ls /usr/include/c++/$cxx_proper_version | grep $i`"; then
                    mystery_dir=`ls /usr/include/c++/$cxx_proper_version | grep $i`
		    echo `ls /usr/include/c++/$cxx_proper_version | grep $i`
		  fi
		done
		echo "${mystery_dir}"
		
		LV_CXXFLAGS="${LV_CXXFLAGS} -nostdinc++ -I/usr/include/c++/$cxx_proper_version -I/usr/include/c++/$cxx_proper_version/$mystery_dir"
		
		CXXFLAGS="${CXXFLAGS} -nostdinc++ -I/usr/include/c++/$cxx_proper_version -I/usr/include/c++/$cxx_proper_version/$mystery_dir"
		

	
	fi
    	
    esac

	])