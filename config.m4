$Id
dnl config.m4 for extension ustring

PHP_ARG_ENABLE(ustring, for ustring support,
[  --enable-ustring           Include ustring support])

if test "$PHP_USTRING" != "no"; then
  PHP_SETUP_ICU(USTRING_SHARED_LIBADD)
  PHP_SUBST(USTRING_SHARED_LIBADD)

  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++,,USTRING_SHARED_LIBADD)
  
  dnl should be configured with some switch when more than one backend exists
  
  PHP_USTRING_SOURCE="ustring.cpp backend/icu.cpp"
  
  PHP_NEW_EXTENSION(ustring, $PHP_USTRING_SOURCE, $ext_shared,, -D__STDC_LIMIT_MACROS)
fi
