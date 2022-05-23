/****************************************************************
 *
 * ======== lily-test ========
 *
 * a midsize C unit testing library
 * copyright (c) 2022 kate swanson (sanine)
 *
 * This is anti-capitalist software, released for free use by individuals and
 * organizations that do not operate by capitalist principles.
 *
 * Permission is hereby granted, free of charge, to any person or
 * organization (the "User") obtaining a copy of this software and associated
 * documentation files (the "Software"), to use, copy, modify, merge,
 * distribute, and/or sell copies of the Software, subject to the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be included
 *    in all copies or modified versions of the Software.
 *
 * 2. The User is one of the following:
 *    a. An individual person, laboring for themselves
 *    b. A non-profit organization
 *    c. An educational institution
 *    d. An organization that seeks shared profit for all of its members, and
 *       allows non-members to set the cost of their labor
 *
 * 3. If the User is an organization with owners, then all owners are workers
 *    and all workers are owners with equal equity and/or equal vote.
 *
 * 4. If the User is an organization, then the User is not law enforcement or
 *    military, or working for or under either.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT EXPRESS OR IMPLIED WARRANTY OF
 * ANY KIND, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://anticapitalist.software/
 * https://sanine.net
 *
 ****************************************************************/


#ifndef LILY_TEST_H
#define LILY_TEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

#define STR_IMP(x) #x
#define STR(x) STR_IMP(x)
/* define SOURCE_PATH_SIZE to strip away the
   leading parts of the full compilation path */
#ifndef SOURCE_PATH_SIZE
#define LILY_LOCATION (__FILE__ ":" STR(__LINE__))
#else
#define LILY_LOCATION ((__FILE__ ":" STR(__LINE__)) + SOURCE_PATH_SIZE)
#endif

/** a few nasty globals that make everything clean for the end user */
struct lily_globals_t {
   jmp_buf env;
   size_t error_msg_len;
   char *error_msg;
   const char *error_location;
};
extern struct lily_globals_t _lily_globals;

typedef void (*lily_test_t)(void);

/** run a single test */
#define lily_run_test(test) _lily_run_test(#test, test)
void _lily_run_test(const char *name, lily_test_t test);

/** run a suite */
#define lily_run_suite(suite) _lily_run_suite(#suite, suite)
void _lily_run_suite(const char *name, lily_test_t suite);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * assertions
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/** basic assertion function, mostly used by the other assertions */
void lily_assert_msg(bool statement, const char *location,
		     const char *format_string, ...);

#define lily_assert_true(statement) _lily_assert_true(#statement, statement, LILY_LOCATION)
void _lily_assert_true(const char *statement, bool value, const char *location);


#define lily_assert_false(statement) _lily_assert_false(#statement, statement, LILY_LOCATION)
void _lily_assert_false(const char *statement, bool value, const char *location);


#define lily_assert_not_null(ptr) _lily_assert_not_null(#ptr, ptr, LILY_LOCATION)
void _lily_assert_not_null(const char *name, void *ptr, const char *location);


#define lily_assert_null(ptr) _lily_assert_null(#ptr, ptr, LILY_LOCATION)
void _lily_assert_null(const char *name, void *ptr, const char *location);


#define lily_assert_ptr_equal(a, b) _lily_assert_ptr_equal(#a, #b, a, b, LILY_LOCATION)
void _lily_assert_ptr_equal(const char *name_a, const char *name_b,
			    void *a, void *b, const char *location);


#define lily_assert_ptr_not_equal(a, b) _lily_assert_ptr_not_equal(#a, #b, a, b, LILY_LOCATION)
void _lily_assert_ptr_not_equal(const char *name_a, const char *name_b,
				void *a, void *b, const char *location);


#define lily_assert_int_equal(a, b) _lily_assert_int_equal(#a, #b, a, b, LILY_LOCATION)
void _lily_assert_int_equal(const char *name_a, const char *name_b,
			    intmax_t a, intmax_t b, const char *location);


#define lily_assert_int_not_equal(a, b) _lily_assert_int_not_equal(#a, #b, a, b, LILY_LOCATION)
void _lily_assert_int_not_equal(const char *name_a, const char *name_b,
				intmax_t a, intmax_t b, const char *location);


#define lily_assert_float_equal(a, b, epsilon)				\
   _lily_assert_float_equal(#a, #b, a, b, epsilon, LILY_LOCATION)
void _lily_assert_float_equal(const char *name_a, const char *name_b,
			      double a, double b, double epsilon, const char *location);


#define lily_assert_float_not_equal(a, b, epsilon)			\
   _lily_assert_float_not_equal(#a, #b, a, b, epsilon, LILY_LOCATION)
void _lily_assert_float_not_equal(const char *name_a, const char *name_b,
				  double a, double b, double epsilon, const char *location);


#define lily_assert_string_equal(a, b) _lily_assert_string_equal(#a, #b, a, b, LILY_LOCATION)
void _lily_assert_string_equal(const char *name_a, const char *name_b,
			       char *a, char *b, const char *location);


#define lily_assert_string_not_equal(a, b) _lily_assert_string_not_equal(#a, #b, a, b, LILY_LOCATION)
void _lily_assert_string_not_equal(const char *name_a, const char *name_b,
				   char *a, char *b, const char *location);


#define lily_assert_memory_equal(a, b, size)			\
   _lily_assert_memory_equal(#a, #b, a, b, size, LILY_LOCATION)
void _lily_assert_memory_equal(const char *name_a, const char *name_b,
			       void *a, void *b, size_t size, const char *location);


#define lily_assert_memory_not_equal(a, b, size)			\
   _lily_assert_memory_not_equal(#a, #b, a, b, size, LILY_LOCATION)
void _lily_assert_memory_not_equal(const char *name_a, const char *name_b,
				   void *a, void *b, size_t size, const char *location);


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * mocks
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


/** queue structure capable of containing arbitrary data */
typedef struct lily_queue_t {
   size_t buf_size;
   uint8_t *buf;
   uint8_t *front, *back;
} lily_queue_t;


/** create a queue */
lily_queue_t* lily_queue_create();

/** destroy a queue */
void lily_queue_destroy(lily_queue_t *q);


/** enqueue a value
 *
 * q - the queue to append to
 * type - the type of the value to append
 * value - the value to append
 */
#define lily_enqueue(q, type, value)				\
   do {								\
      type _var = value;					\
      _lily_enqueue(q, sizeof(type), (uint8_t*)(&_var));	\
   } while(0)
void _lily_enqueue(lily_queue_t *q, size_t size, uint8_t *data);


/** pop a value from the queue
 *
 * q - the queue to pop from
 * type - the type of the value to pop
 * ptr - the location to store the popped value
 */
#define lily_dequeue(q, type, ptr)			\
   _lily_dequeue(q, sizeof(type), (uint8_t*) ptr)
void _lily_dequeue(lily_queue_t *q, size_t size, uint8_t *ptr);


struct lily_mock_arg_t {
   size_t size;
   void *var;
};

#define LILY_NARGS(args) (sizeof(args)/sizeof(struct lily_mock_arg_t))

/** structure to store data for mock functions */
typedef struct lily_mock_t {
   unsigned int n_calls;
   lily_queue_t *arguments;
   lily_queue_t *values;
} lily_mock_t;

/** setup mock function storage */
lily_mock_t * lily_mock_create();
/** tear down mock function storage */
void lily_mock_destroy(lily_mock_t *m);


/** store a call to a mock function */
#define lily_mock_call(m, args)			\
   _lily_mock_call(m, args, LILY_NARGS(args))
void _lily_mock_call(lily_mock_t *m, struct lily_mock_arg_t *args, size_t n_args);

/** retrieve a call to a mock function */
#define lily_get_call(m, args, call_num)		\
   _lily_get_call(m, args, LILY_NARGS(args), call_num)
void _lily_get_call(lily_mock_t *m,
		    struct lily_mock_arg_t *args,
		    size_t n_args,
		    unsigned int call_num);

/** store a value in a mock structure */
#define lily_store_value(m, type, value)	\
   lily_enqueue(m->values, type, value)
/** retrieve a value from a mock structure */
#define lily_get_value(m, type, ptr)		\
   lily_dequeue(m->values, type, ptr)

#endif
