$Id
dnl config.m4 for extension ustring

PHP_ARG_WITH(ustring, for ustring support,
[  --with-ustring[=DIR]             Include ustring support])

if test "$PHP_USTRING" != "no"; then
  PHP_REQUIRE_CXX()
  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="include/unicode/unistr.h"  # you most likely want to change this
  if test -r $PHP_USTRING/$SEARCH_FOR; then # path given as parameter
    USTRING_DIR=$PHP_USTRING
  else # search default path list
    AC_MSG_CHECKING([for libicu files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        USTRING_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  dnl
  if test -z "$USTRING_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the ustring distribution])
  fi

  # --with-ustring -> add include path
  PHP_ADD_INCLUDE($USTRING_DIR/include)

  # --with-ustring -> check for lib and symbol presence
  LIBNAME=icuuc # you may want to change this
  LIBSYMBOL=u_versionToString_50 # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $USTRING_DIR/lib64, USTRING_SHARED_LIBADD)
    PHP_ADD_LIBRARY(stdc++, 1, USTRING_SHARED_LIBADD)
    AC_DEFINE(HAVE_USTRINGLIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong libicu version or libicu not found])
  ],[
    -L$USTRING_DIR/lib -lm
  ])

  PHP_SUBST(USTRING_SHARED_LIBADD)

  PHP_NEW_EXTENSION(ustring, ustring.cc, $ext_shared)
fi
