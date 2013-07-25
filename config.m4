
PHP_ARG_ENABLE(libpuzzle5, whether to enable libpuzzle5 support,
Make sure that the comment is aligned:
[  --enable-libpuzzle5           Enable libpuzzle5 support])

if test "$PHP_LIBPUZZLE5" != "no"; then

  GDLIB_LIBS=$($GDLIB_CONFIG --ldflags --libs)
  GDLIB_INCS=$($GDLIB_CONFIG --cflags)
  
  PHP_EVAL_LIBLINE($GDLIB_LIBS, LIBPUZZLE5_SHARED_LIBADD)
  PHP_EVAL_INCLINE($GDLIB_INCS)

  SEARCH_PATH="/usr/local /usr"     # you might want to change this
  SEARCH_FOR="/include/puzzle.h"  # you most likely want to change this
  if test -r $PHP_LIBPUZZLE5/$SEARCH_FOR; then # path given as parameter
    LIBPUZZLE_DIR=$PHP_LIBPUZZLE5
  else # search default path list
    AC_MSG_CHECKING([for libpuzzle5 files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        LIBPUZZLE_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi
  
  if test -z "$LIBPUZZLE_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([Please reinstall the libpuzzle distribution])
  fi

  dnl # --with-libpuzzle5 -> add include path
  PHP_ADD_INCLUDE($LIBPUZZLE_DIR/include)

  # --with-libpuzzle5 -> check for lib and symbol presence
  LIBNAME=puzzle # you may want to change this
  LIBSYMBOL=puzzle_global_context # you most likely want to change this 

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LIBPUZZLE_DIR/lib, LIBPUZZLE5_SHARED_LIBADD)
    AC_DEFINE(HAVE_LIBPUZZLE5LIB,1,[ ])
  ],[
    AC_MSG_ERROR([wrong $LIBNAME lib version or lib not found])
  ],[
    -L$LIBPUZZLE_DIR/lib -lm
  ])
  
  PHP_SUBST(LIBPUZZLE5_SHARED_LIBADD)

  PHP_NEW_EXTENSION(libpuzzle5, libpuzzle5.c, $ext_shared)
fi
