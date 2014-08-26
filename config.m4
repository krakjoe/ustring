$Id
dnl config.m4 for extension ustring

PHP_ARG_ENABLE(ustring, for ustring support,
[  --enable-ustring           Include ustring support])

if test "$PHP_USTRING" != "no"; then
  PHP_SETUP_ICU(USTRING_SHARED_LIBADD)
  PHP_SUBST(USTRING_SHARED_LIBADD)

  PHP_REQUIRE_CXX()

  PHP_NEW_EXTENSION(ustring, ustring.cpp, $ext_shared,, -D__STDC_LIMIT_MACROS)
fi
