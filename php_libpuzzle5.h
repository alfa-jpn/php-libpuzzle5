
#ifndef PHP_LIBPUZZLE5_H
#define PHP_LIBPUZZLE5_H

extern zend_module_entry libpuzzle5_module_entry;
#define phpext_libpuzzle5_ptr &libpuzzle5_module_entry

#ifdef PHP_WIN32
#	define PHP_LIBPUZZLE5_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LIBPUZZLE5_API __attribute__ ((visibility("default")))
#else
#	define PHP_LIBPUZZLE5_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(libpuzzle5);
PHP_MSHUTDOWN_FUNCTION(libpuzzle5);
PHP_RINIT_FUNCTION(libpuzzle5);
PHP_RSHUTDOWN_FUNCTION(libpuzzle5);
PHP_MINFO_FUNCTION(libpuzzle5);

PHP_FUNCTION(puzzle_set_max_width);
PHP_FUNCTION(puzzle_set_max_height);
PHP_FUNCTION(puzzle_set_lambdas);
PHP_FUNCTION(puzzle_set_noise_cutoff);
PHP_FUNCTION(puzzle_set_p_ratio);
PHP_FUNCTION(puzzle_set_contrast_barrier_for_cropping);
PHP_FUNCTION(puzzle_set_max_cropping_ratio);
PHP_FUNCTION(puzzle_set_autocrop);

PHP_FUNCTION(puzzle_fill_cvec_from_file);
PHP_FUNCTION(puzzle_compress_cvec);
PHP_FUNCTION(puzzle_uncompress_cvec);
PHP_FUNCTION(puzzle_vector_normalized_distance);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:
*/ 

ZEND_BEGIN_MODULE_GLOBALS(libpuzzle5)
	PuzzleContext  *global_context;
ZEND_END_MODULE_GLOBALS(libpuzzle5)


/* In every utility function you add that needs to use variables 
   in php_libpuzzle5_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as LIBPUZZLE5_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define LIBPUZZLE5_G(v) TSRMG(libpuzzle5_globals_id, zend_libpuzzle5_globals *, v)
#else
#define LIBPUZZLE5_G(v) (libpuzzle5_globals.v)
#endif


#endif	/* PHP_LIBPUZZLE5_H */

