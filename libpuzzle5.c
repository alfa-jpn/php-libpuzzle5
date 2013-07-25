
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include <puzzle.h>
#include "php_libpuzzle5.h"


ZEND_DECLARE_MODULE_GLOBALS(libpuzzle5)

/* True global resources - no need for thread safety here */
static int le_libpuzzle5;
static PHP_GINIT_FUNCTION(libpuzzle5);
static PHP_GSHUTDOWN_FUNCTION(libpuzzle5);

/* {{{ libpuzzle5_functions[]
 *
 * Every user visible function must have an entry in libpuzzle5_functions[].
 */
const zend_function_entry libpuzzle5_functions[] = {
	PHP_FE(puzzle_set_max_width, NULL)
	PHP_FE(puzzle_set_max_height, NULL)
	PHP_FE(puzzle_set_lambdas, NULL)
	PHP_FE(puzzle_set_noise_cutoff, NULL)
	PHP_FE(puzzle_set_p_ratio, NULL)
	PHP_FE(puzzle_set_contrast_barrier_for_cropping, NULL)
	PHP_FE(puzzle_set_max_cropping_ratio, NULL)
	PHP_FE(puzzle_set_autocrop, NULL)
	
	PHP_FE(puzzle_fill_cvec_from_file, NULL)
	PHP_FE(puzzle_compress_cvec, NULL)
	PHP_FE(puzzle_uncompress_cvec, NULL)
	PHP_FE(puzzle_vector_normalized_distance, NULL)  
	
	PHP_FE_END	/* Must be the last line in libpuzzle5_functions[] */
};
/* }}} */

/* {{{ libpuzzle5_module_entry
 */
zend_module_entry libpuzzle5_module_entry = {
	STANDARD_MODULE_HEADER,
	"libpuzzle5",
	libpuzzle5_functions,
	PHP_MINIT(libpuzzle5),
	PHP_MSHUTDOWN(libpuzzle5),
	PHP_RINIT(libpuzzle5),
	PHP_RSHUTDOWN(libpuzzle5),
	PHP_MINFO(libpuzzle5),
	NO_VERSION_YET,
	PHP_MODULE_GLOBALS(libpuzzle5),
	PHP_GINIT(libpuzzle5),
	PHP_GSHUTDOWN(libpuzzle5),
	NULL,
	STANDARD_MODULE_PROPERTIES_EX
};
/* }}} */

#ifdef COMPILE_DL_LIBPUZZLE5
ZEND_GET_MODULE(libpuzzle5)
#endif


/* {{{ PHP_GINIT_FUNCTION
 */
PHP_GINIT_FUNCTION(libpuzzle5)
{
	libpuzzle5_globals->global_context = pemalloc(sizeof(PuzzleContext), 1);
}
/* }}} */

/* {{{ PHP_GSHUTDOWN_FUNCTION
 */
PHP_GSHUTDOWN_FUNCTION(libpuzzle5)
{
	pefree(libpuzzle5_globals->global_context, 1);
}
/* }}} */



/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(libpuzzle5)
{
    REGISTER_DOUBLE_CONSTANT("PUZZLE_CVEC_SIMILARITY_THRESHOLD",
                             PUZZLE_CVEC_SIMILARITY_THRESHOLD,
                             CONST_CS | CONST_PERSISTENT);
    REGISTER_DOUBLE_CONSTANT("PUZZLE_CVEC_SIMILARITY_HIGH_THRESHOLD",
                             PUZZLE_CVEC_SIMILARITY_HIGH_THRESHOLD,
                             CONST_CS | CONST_PERSISTENT);
    REGISTER_DOUBLE_CONSTANT("PUZZLE_CVEC_SIMILARITY_LOW_THRESHOLD",
                             PUZZLE_CVEC_SIMILARITY_LOW_THRESHOLD,
                             CONST_CS | CONST_PERSISTENT);
    REGISTER_DOUBLE_CONSTANT("PUZZLE_CVEC_SIMILARITY_LOWER_THRESHOLD",
                             PUZZLE_CVEC_SIMILARITY_LOWER_THRESHOLD,
                             CONST_CS | CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(libpuzzle5)
{
	return SUCCESS;
}
/* }}} */



/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(libpuzzle5)
{
	puzzle_init_context(LIBPUZZLE5_G(global_context));
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(libpuzzle5)
{
	puzzle_free_context(LIBPUZZLE5_G(global_context));
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(libpuzzle5)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "libpuzzle5 support", "enabled");
	php_info_print_table_end();
}
/* }}} */






/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */



/* {{{ proto string puzzle_fill_cvec_from_file(string filename)
 * Creates a signature out of an image file */
PHP_FUNCTION(puzzle_fill_cvec_from_file)
{    
    char *arg = NULL;
    int arg_len;
    PuzzleContext *context;
    PuzzleCvec cvec;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                              "s", &arg, &arg_len) == FAILURE ||
        arg_len <= 0) {
        RETURN_FALSE;
    }
    puzzle_init_cvec(context, &cvec);
    if (puzzle_fill_cvec_from_file(context, &cvec, arg) != 0) {
        puzzle_free_cvec(context, &cvec);
        RETURN_FALSE;
    }
    RETVAL_STRINGL(cvec.vec, cvec.sizeof_vec, 1);
    puzzle_free_cvec(context, &cvec);
}
/* }}} */

/* {{{ proto string puzzle_compress_cvec(string cvec)
 * Compress a signature to save storage space */
PHP_FUNCTION(puzzle_compress_cvec)
{    
    char *arg = NULL;
    int arg_len;
    PuzzleContext *context;
    PuzzleCompressedCvec compressed_cvec;
    PuzzleCvec cvec;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                              "s", &arg, &arg_len) == FAILURE ||
        arg_len <= 0) {
        RETURN_FALSE;
    }
    puzzle_init_compressed_cvec(context, &compressed_cvec);
    puzzle_init_cvec(context, &cvec);
    cvec.vec = arg;
    cvec.sizeof_vec = (size_t) arg_len;    
    if (puzzle_compress_cvec(context, &compressed_cvec, &cvec) != 0) {
        puzzle_free_compressed_cvec(context, &compressed_cvec);
        cvec.vec = NULL;
        puzzle_free_cvec(context, &cvec);        
        RETURN_FALSE;
    }
    RETVAL_STRINGL(compressed_cvec.vec,
                   compressed_cvec.sizeof_compressed_vec, 1);
    puzzle_free_compressed_cvec(context, &compressed_cvec);
    cvec.vec = NULL;
    puzzle_free_cvec(context, &cvec);    
}
/* }}} */

/* {{{ proto string puzzle_uncompress_cvec(string compressed_cvec)
 * Uncompress a compressed signature so that it can be used for computations */
PHP_FUNCTION(puzzle_uncompress_cvec)
{    
    char *arg = NULL;
    int arg_len;
    PuzzleContext *context;
    PuzzleCompressedCvec compressed_cvec;
    PuzzleCvec cvec;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
                              "s", &arg, &arg_len) == FAILURE ||
        arg_len <= 0) {
        RETURN_FALSE;
    }
    puzzle_init_compressed_cvec(context, &compressed_cvec);
    puzzle_init_cvec(context, &cvec);
    compressed_cvec.vec = arg;
    compressed_cvec.sizeof_compressed_vec = (size_t) arg_len;    
    if (puzzle_uncompress_cvec(context, &compressed_cvec, &cvec) != 0) {
        puzzle_free_cvec(context, &cvec);
        compressed_cvec.vec = NULL;
        puzzle_free_compressed_cvec(context, &compressed_cvec);
        RETURN_FALSE;
    }
    RETVAL_STRINGL(cvec.vec, cvec.sizeof_vec, 1);
    puzzle_free_cvec(context, &cvec);
    compressed_cvec.vec = NULL;
    puzzle_free_compressed_cvec(context, &compressed_cvec);    
}
/* }}} */

/* {{{ proto double puzzle_vector_normalized_distance(string cvec1, string cvec2 [, bool fix_for_texts])
 * Computes the distance between two signatures. Result is between 0.0 and 1.0 */
PHP_FUNCTION(puzzle_vector_normalized_distance)
{    
    char *vec1 = NULL, *vec2 = NULL;
    int vec1_len, vec2_len;
    PuzzleContext *context;
    PuzzleCvec cvec1, cvec2;
    double d;
    zend_bool fix_for_texts;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "ss|b",
         &vec1, &vec1_len, &vec2, &vec2_len, &fix_for_texts) == FAILURE ||
        vec1_len <= 0 || vec2_len <= 0) {
        RETURN_FALSE;
    }
    if (ZEND_NUM_ARGS() TSRMLS_CC < 3) {
        fix_for_texts = (zend_bool) 1;
    }
    puzzle_init_cvec(context, &cvec1);
    puzzle_init_cvec(context, &cvec2);    
    cvec1.vec = vec1;
    cvec1.sizeof_vec = (size_t) vec1_len;
    cvec2.vec = vec2;
    cvec2.sizeof_vec = (size_t) vec2_len;
    d = puzzle_vector_normalized_distance(context, &cvec1, &cvec2,
                                          (int) fix_for_texts);
    cvec1.vec = cvec2.vec = NULL;
    puzzle_free_cvec(context, &cvec1);
    puzzle_free_cvec(context, &cvec2);
    RETVAL_DOUBLE(d);
}
/* }}} */

/* {{{ proto bool puzzle_set_max_width(int width)
 * Set the maximum picture width */
PHP_FUNCTION(puzzle_set_max_width)
{
    PuzzleContext *context;
    long width;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "l", &width) == FAILURE ||
        width <= 0L || width > INT_MAX) {
        RETURN_FALSE;
    }
    if (puzzle_set_max_width(context, (unsigned int) width) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;    
}
/* }}} */

/* {{{ proto bool puzzle_set_max_height(int height)
 * Set the maximum picture height */
PHP_FUNCTION(puzzle_set_max_height)
{
    PuzzleContext *context;
    long height;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "l", &height) == FAILURE ||
        height <= 0L || height > INT_MAX) {
        RETURN_FALSE;
    }
    if (puzzle_set_max_height(context, (unsigned int) height) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;    
}
/* }}} */

/* {{{ proto bool puzzle_set_lambdas(int lambdas)
 * Set the size of the computation grid */
PHP_FUNCTION(puzzle_set_lambdas)
{
    PuzzleContext *context;
    long lambdas;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "l", &lambdas) == FAILURE ||
        lambdas <= 0L || lambdas > INT_MAX) {
        RETURN_FALSE;
    }
    if (puzzle_set_lambdas(context, (unsigned int) lambdas) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;    
}
/* }}} */

/* {{{ proto bool puzzle_set_noise_cutoff(double cutoff)
 * Set the noise cutoff level */
PHP_FUNCTION(puzzle_set_noise_cutoff)
{
    PuzzleContext *context;
    double cutoff;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "d", &cutoff) == FAILURE) {
        RETURN_FALSE;
    }
    if (puzzle_set_noise_cutoff(context, cutoff) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;    
}
/* }}} */

/* {{{ proto bool puzzle_set_p_ratio(double ratio)
 * Set the p_ratio */
PHP_FUNCTION(puzzle_set_p_ratio)
{
    PuzzleContext *context;
    double p_ratio;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "d", &p_ratio) == FAILURE) {
        RETURN_FALSE;
    }
    if (puzzle_set_p_ratio(context, p_ratio) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool puzzle_set_contrast_barrier_for_cropping(double barrier)
 * Set the tolerance level for cropping */
PHP_FUNCTION(puzzle_set_contrast_barrier_for_cropping)
{
    PuzzleContext *context;
    double barrier;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "d", &barrier) == FAILURE) {
        RETURN_FALSE;
    }
    if (puzzle_set_contrast_barrier_for_cropping(context, barrier) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool puzzle_set_max_cropping_ratio(double ratio)
 * Set the maximum ratio between the cropped area and the whole picture */
PHP_FUNCTION(puzzle_set_max_cropping_ratio)
{
    PuzzleContext *context;
    double ratio;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "d", &ratio) == FAILURE) {
        RETURN_FALSE;
    }
    if (puzzle_set_max_cropping_ratio(context, ratio) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;
}
/* }}} */

/* {{{ proto bool puzzle_set_autocrop(bool autocrop)
 * TRUE to enable autocropping, FALSE to disable */
PHP_FUNCTION(puzzle_set_autocrop)
{
    PuzzleContext *context;
    zend_bool autocrop;
    
    context = LIBPUZZLE5_G(global_context);
    if (zend_parse_parameters
        (ZEND_NUM_ARGS() TSRMLS_CC, "b", &autocrop) == FAILURE) {
        RETURN_FALSE;
    }
    if (puzzle_set_autocrop(context, (int) autocrop) != 0) {
        RETURN_FALSE;
    }
    RETVAL_TRUE;
}
/* }}} */



/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
