dnl Available from the GNU Autoconf Macro Archive at:
dnl http://www.gnu.org/software/ac-archive/htmldoc/bnv_have_qt.html
dnl
AC_DEFUN([BNV_HAVE_QT],
[
  dnl THANKS! This code includes bug fixes by:
  dnl Tim McClarren.

  AC_REQUIRE([AC_PROG_CXX])
  AC_REQUIRE([AC_PATH_X])
  AC_REQUIRE([AC_PATH_XTRA])

  AC_MSG_CHECKING(for Qt)

  AC_ARG_WITH([Qt-dir],
    [  --with-Qt-dir=DIR       DIR is equal to \$QTDIR if you have followed the
                          installation instructions of Trolltech. Header
                          files are in DIR/include, binary utilities are
                          in DIR/bin and the library is in DIR/lib])
  AC_ARG_WITH([Qt-include-dir],
    [  --with-Qt-include-dir=DIR
                          Qt header files are in DIR])
  AC_ARG_WITH([Qt-bin-dir],
    [  --with-Qt-bin-dir=DIR   Qt utilities such as moc and uic are in DIR])
  AC_ARG_WITH([Qt-lib-dir],
    [  --with-Qt-lib-dir=DIR   The Qt library is in DIR])
  AC_ARG_WITH([Qt-lib],
    [  --with-Qt-lib=LIB       Use -lLIB to link with the Qt library])
  if test x"$with_Qt_dir" = x"no" ||
     test x"$with_Qt_include-dir" = x"no" ||
     test x"$with_Qt_bin_dir" = x"no" ||
     test x"$with_Qt_lib_dir" = x"no" ||
     test x"$with_Qt_lib" = x"no"; then
    # user disabled Qt. Leave cache alone.
    have_qt="User disabled Qt."
  else
    # "yes" is a bogus option
    if test x"$with_Qt_dir" = xyes; then
      with_Qt_dir=
    fi
    if test x"$with_Qt_include_dir" = xyes; then
      with_Qt_include_dir=
    fi
    if test x"$with_Qt_bin_dir" = xyes; then
      with_Qt_bin_dir=
    fi
    if test x"$with_Qt_lib_dir" = xyes; then
      with_Qt_lib_dir=
    fi
    if test x"$with_Qt_lib" = xyes; then
      with_Qt_lib=
    fi
    # No Qt unless we discover otherwise
    have_qt=no
    # Check whether we are requested to link with a specific version
    if test x"$with_Qt_lib" != x; then
      bnv_qt_lib="$with_Qt_lib"
    fi
    # Check whether we were supplied with an answer already
    if test x"$with_Qt_dir" != x; then
      have_qt=yes
      bnv_qt_dir="$with_Qt_dir"
      bnv_qt_include_dir="$with_Qt_dir/include"
      bnv_qt_bin_dir="$with_Qt_dir/bin"
      bnv_qt_lib_dir="$with_Qt_dir/lib"
      # Only search for the lib if the user did not define one already
      if test x"$bnv_qt_lib" = x; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Use cached value or do search, starting with suggestions from
      # the command line
      AC_CACHE_VAL(bnv_cv_have_qt,
      [
        # We are not given a solution and there is no cached value.
        bnv_qt_dir=NO
        bnv_qt_include_dir=NO
        bnv_qt_lib_dir=NO
        if test x"$bnv_qt_lib" = x; then
          bnv_qt_lib=NO
        fi
        BNV_PATH_QT_DIRECT
        if test "$bnv_qt_dir" = NO ||
           test "$bnv_qt_include_dir" = NO ||
           test "$bnv_qt_lib_dir" = NO ||
           test "$bnv_qt_lib" = NO; then
          # Problem with finding complete Qt.  Cache the known absence of Qt.
          bnv_cv_have_qt="have_qt=no"
        else
          # Record where we found Qt for the cache.
          bnv_cv_have_qt="have_qt=yes                  \
                       bnv_qt_dir=$bnv_qt_dir          \
               bnv_qt_include_dir=$bnv_qt_include_dir  \
                   bnv_qt_bin_dir=$bnv_qt_bin_dir      \
                      bnv_qt_LIBS=\"$bnv_qt_LIBS\""
        fi
      ])dnl
      eval "$bnv_cv_have_qt"
    fi # all $bnv_qt_* are set
  fi   # $have_qt reflects the system status
  if test x"$have_qt" = xyes; then
    QT_CXXFLAGS="-I$bnv_qt_include_dir"
    QT_DIR="$bnv_qt_dir"
    QT_LIBS="$bnv_qt_LIBS"
    # If bnv_qt_dir is defined, utilities are expected to be in the
    # bin subdirectory
    if test x"$bnv_qt_dir" != x; then
        if test -x "$bnv_qt_dir/bin/uic"; then
          QT_UIC="$bnv_qt_dir/bin/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
      QT_MOC="$bnv_qt_dir/bin/moc"
    else
      # Or maybe we are told where to look for the utilities
      if test x"$bnv_qt_bin_dir" != x; then
        if test -x "$bnv_qt_bin_dir/uic"; then
          QT_UIC="$bnv_qt_bin_dir/uic"
        else
          # Old versions of Qt don't have uic
          QT_UIC=
        fi
        QT_MOC="$bnv_qt_bin_dir/moc"
      else
      # Last possibility is that they are in $PATH
        QT_UIC="`which uic`"
        QT_MOC="`which moc`"
      fi
    fi
    # All variables are defined, report the result
    AC_MSG_RESULT([$have_qt:
    QT_CXXFLAGS=$QT_CXXFLAGS
    QT_DIR=$QT_DIR
    QT_LIBS=$QT_LIBS
    QT_UIC=$QT_UIC
    QT_MOC=$QT_MOC])
  else
    # Qt was not found
    QT_CXXFLAGS=
    QT_DIR=
    QT_LIBS=
    QT_UIC=
    QT_MOC=
    AC_MSG_RESULT($have_qt)
  fi
  AC_SUBST(QT_CXXFLAGS)
  AC_SUBST(QT_DIR)
  AC_SUBST(QT_LIBS)
  AC_SUBST(QT_UIC)
  AC_SUBST(QT_MOC)

  #### Being paranoid:
  if test x"$have_qt" = xyes; then
    AC_MSG_CHECKING(correct functioning of Qt installation)
    AC_CACHE_VAL(bnv_cv_qt_test_result,
    [
      cat > bnv_qt_test.h << EOF
#include <qobject.h>
class Test : public QObject
{
Q_OBJECT
public:
  Test() {}
  ~Test() {}
public slots:
  void receive() {}
signals:
  void send();
};
EOF

      cat > bnv_qt_main.$ac_ext << EOF
#include "bnv_qt_test.h"
#include <qapplication.h>
int main( int argc, char **argv )
{
  QApplication app( argc, argv );
  Test t;
  QObject::connect( &t, SIGNAL(send()), &t, SLOT(receive()) );
}
EOF

      bnv_cv_qt_test_result="failure"
      bnv_try_1="$QT_MOC bnv_qt_test.h -o moc_bnv_qt_test.$ac_ext >/dev/null 2>bnv_qt_test_1.out"
      AC_TRY_EVAL(bnv_try_1)
      bnv_err_1=`grep -v '^ *+' bnv_qt_test_1.out | grep -v "^bnv_qt_test.h\$"`
      if test x"$bnv_err_1" != x; then
        echo "$bnv_err_1" >&AC_FD_CC
        echo "configure: could not run $QT_MOC on:" >&AC_FD_CC
        cat bnv_qt_test.h >&AC_FD_CC
      else
        bnv_try_2="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o moc_bnv_qt_test.o moc_bnv_qt_test.$ac_ext >/dev/null 2>bnv_qt_test_2.out"
        AC_TRY_EVAL(bnv_try_2)
        bnv_err_2=`grep -v '^ *+' bnv_qt_test_2.out | grep -v "^bnv_qt_test.{$ac_ext}\$"`
        if test x"$bnv_err_2" != x; then
          echo "$bnv_err_2" >&AC_FD_CC
          echo "configure: could not compile:" >&AC_FD_CC
          cat bnv_qt_test.$ac_ext >&AC_FD_CC
        else
          bnv_try_3="$CXX $QT_CXXFLAGS -c $CXXFLAGS -o bnv_qt_main.o bnv_qt_main.$ac_ext >/dev/null 2>bnv_qt_test_3.out"
          AC_TRY_EVAL(bnv_try_3)
          bnv_err_3=`grep -v '^ *+' bnv_qt_test_3.out | grep -v "^bnv_qt_main.{$ac_ext}\$"`
          if test x"$bnv_err_3" != x; then
            echo "$bnv_err_3" >&AC_FD_CC
            echo "configure: could not compile:" >&AC_FD_CC
            cat bnv_qt_main.$ac_ext >&AC_FD_CC
          else
            bnv_try_4="$CXX $QT_LIBS $LIBS -o bnv_qt_main bnv_qt_main.o moc_bnv_qt_test.o >/dev/null 2>bnv_qt_test_4.out"
            AC_TRY_EVAL(bnv_try_4)
            bnv_err_4=`grep -v '^ *+' bnv_qt_test_4.out`
            if test x"$bnv_err_4" != x; then
              echo "$bnv_err_4" >&AC_FD_CC
            else
              bnv_cv_qt_test_result="success"
            fi
          fi
        fi
      fi
    ])dnl AC_CACHE_VAL bnv_cv_qt_test_result
    AC_MSG_RESULT([$bnv_cv_qt_test_result]);
    if test x"$bnv_cv_qt_test_result" = "xfailure"; then
      # working Qt was not found
      QT_CXXFLAGS=
      QT_DIR=
      QT_LIBS=
      QT_UIC=
      QT_MOC=
      have_qt=no
      AC_MSG_WARN([Failed to find matching components of a complete
                  Qt installation. Try using more options,
                  see ./configure --help.])
    fi

    rm -f bnv_qt_test.h moc_bnv_qt_test.$ac_ext moc_bnv_qt_test.o \
          bnv_qt_main.$ac_ext bnv_qt_main.o bnv_qt_main \
          bnv_qt_test_1.out bnv_qt_test_2.out bnv_qt_test_3.out bnv_qt_test_4.out
  fi
])

dnl Internal subroutine of BNV_HAVE_QT
dnl Set bnv_qt_dir bnv_qt_include_dir bnv_qt_bin_dir bnv_qt_lib_dir bnv_qt_lib
dnl Copyright 2001 Bastiaan N. Veelo <Bastiaan.N.Veelo@immtek.ntnu.no>
AC_DEFUN([BNV_PATH_QT_DIRECT],
[
  ## Binary utilities ##
  if test x"$with_Qt_bin_dir" != x; then
    bnv_qt_bin_dir=$with_Qt_bin_dir
  fi
  ## Look for header files ##
  if test x"$with_Qt_include_dir" != x; then
    bnv_qt_include_dir="$with_Qt_include_dir"
  else
    # The following header file is expected to define QT_VERSION.
    qt_direct_test_header=qglobal.h
    # Look for the header file in a standard set of common directories.
    bnv_include_path_list="
      /usr/include
      `ls -dr /usr/include/qt* 2>/dev/null`
      `ls -dr /usr/lib/qt*/include 2>/dev/null`
      `ls -dr /usr/local/qt*/include 2>/dev/null`
      `ls -dr /opt/qt*/include 2>/dev/null`
    "
    for bnv_dir in $bnv_include_path_list; do
      if test -r "$bnv_dir/$qt_direct_test_header"; then
        bnv_dirs="$bnv_dirs $bnv_dir"
      fi
    done
    # Now look for the newest in this list
    bnv_prev_ver=0
    for bnv_dir in $bnv_dirs; do
      bnv_this_ver=`egrep -w '#define QT_VERSION' $bnv_dir/$qt_direct_test_header | sed s/'#define QT_VERSION'//`
      if expr $bnv_this_ver '>' $bnv_prev_ver > /dev/null; then
        bnv_qt_include_dir=$bnv_dir
        bnv_prev_ver=$bnv_this_ver
      fi
    done
  fi dnl Found header files.

  # Are these headers located in a traditional Trolltech installation?
  # That would be $bnv_qt_include_dir stripped from its last element:
  bnv_possible_qt_dir=`dirname $bnv_qt_include_dir`
  if test -x $bnv_possible_qt_dir/bin/moc &&
     ls $bnv_possible_qt_dir/lib/libqt* > /dev/null; then
    # Then the rest is a piece of cake
    bnv_qt_dir=$bnv_possible_qt_dir
    bnv_qt_bin_dir="$bnv_qt_dir/bin"
    bnv_qt_lib_dir="$bnv_qt_dir/lib"
    # Only look for lib if the user did not supply it already
    if test x"$bnv_qt_lib" = xNO; then
      bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                   sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
    fi
    bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
  else
    # There is no valid definition for $QTDIR as Trolltech likes to see it
    bnv_qt_dir=
    ## Look for Qt library ##
    if test x"$with_Qt_lib_dir" != x; then
      bnv_qt_lib_dir="$with_Qt_lib_dir"
      # Only look for lib if the user did not supply it already
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib="`ls $bnv_qt_lib_dir/libqt* | sed -n 1p |
                     sed s@$bnv_qt_lib_dir/lib@@ | [sed s@[.].*@@]`"
      fi
      bnv_qt_LIBS="-L$bnv_qt_lib_dir -l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
    else
      # Normally, when there is no traditional Trolltech installation,
      # the library is installed in a place where the linker finds it
      # automatically.
      # If the user did not define the library name, try with qt
      if test x"$bnv_qt_lib" = xNO; then
        bnv_qt_lib=qt
      fi
      qt_direct_test_header=qapplication.h
      qt_direct_test_main="
        int argc;
        char ** argv;
        QApplication app(argc,argv);
      "
      # See if we find the library without any special options.
      # Don't add top $LIBS permanently yet
      bnv_save_LIBS="$LIBS"
      LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
      bnv_qt_LIBS="$LIBS"
      bnv_save_CXXFLAGS="$CXXFLAGS"
      CXXFLAGS="-I$bnv_qt_include_dir"
      AC_TRY_LINK([#include <$qt_direct_test_header>],
        $qt_direct_test_main,
      [
        # Success.
        # We can link with no special library directory.
        bnv_qt_lib_dir=
      ], [
        # That did not work. Try the multi-threaded version
        echo "Non-critical error, please neglect the above." >&AC_FD_CC
        bnv_qt_lib=qt-mt
        LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
        AC_TRY_LINK([#include <$qt_direct_test_header>],
          $qt_direct_test_main,
        [
          # Success.
          # We can link with no special library directory.
          bnv_qt_lib_dir=
        ], [
          # That did not work. Try the OpenGL version
          echo "Non-critical error, please neglect the above." >&AC_FD_CC
          bnv_qt_lib=qt-gl
          LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
          AC_TRY_LINK([#include <$qt_direct_test_header>],
            $qt_direct_test_main,
          [
            # Succes.
            # We can link with no special library directory.
            bnv_qt_lib_dir=
          ], [
            # That did not work. Maybe a library version I don't know about?
            echo "Non-critical error, please neglect the above." >&AC_FD_CC
            # Look for some Qt lib in a standard set of common directories.
            bnv_dir_list="
              `echo $bnv_qt_includes | sed ss/includess`
              /lib
              /usr/lib
              /usr/local/lib
              /opt/lib
              `ls -dr /usr/lib/qt* 2>/dev/null`
              `ls -dr /usr/local/qt* 2>/dev/null`
              `ls -dr /opt/qt* 2>/dev/null`
            "
            for bnv_dir in $bnv_dir_list; do
              if ls $bnv_dir/libqt*; then
                # Gamble that it's the first one...
                bnv_qt_lib="`ls $bnv_dir/libqt* | sed -n 1p |
                            sed s@$bnv_dir/lib@@ | sed s/[.].*//`"
                bnv_qt_lib_dir="$bnv_dir"
                break
              fi
            done
            # Try with that one
            LIBS="-l$bnv_qt_lib $X_PRE_LIBS $X_LIBS -lX11 -lXext -lXmu -lXt -lXi $X_EXTRA_LIBS"
            AC_TRY_LINK([#include <$qt_direct_test_header>],
              $qt_direct_test_main,
            [
              # Succes.
              # We can link with no special library directory.
              bnv_qt_lib_dir=
            ], [
              # Leave bnv_qt_lib_dir defined
            ])
          ])
        ])
      ])
      if test x"$bnv_qt_lib_dir" != x; then
        bnv_qt_LIBS="-l$bnv_qt_lib_dir $LIBS"
      else
        bnv_qt_LIBS="$LIBS"
      fi
      LIBS="$bnv_save_LIBS"
      CXXFLAGS="$bnv_save_CXXFLAGS"
    fi dnl $with_Qt_lib_dir was not given
  fi dnl Done setting up for non-traditional Trolltech installation
])
