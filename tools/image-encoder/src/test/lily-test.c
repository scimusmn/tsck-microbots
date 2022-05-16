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
 * distribute, and/or sell copies of the Software, subject to the following 
 * conditions:
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


#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lily-test.h"

struct lily_globals_t _lily_globals = { {0}, 0, NULL, "" };

/* run an individual test */
void _lily_run_test(const char *name, lily_test_t test)
{
   printf("  %s: ", name);

   _lily_globals.error_msg = NULL;
   _lily_globals.error_location = "";
   int val = setjmp(_lily_globals.env);

   if (val) {
	/* test failed */
      printf("FAIL - %s (%s)\n",
	     _lily_globals.error_msg,
	     _lily_globals.error_location);
      free(_lily_globals.error_msg); /* error message was allocated! */
	  return;
   }

   test();

   /* test succeeded */
   printf("OK\n");
}

/* run a suite */
void _lily_run_suite(const char *name, lily_test_t suite)
{
   printf("=== %s ===\n", name);
   suite();
   printf("\n");
}


/* ======== ASSERTIONS ======== */

static void _lily_assert_msg(bool statement, const char *location,
			     const char *format_string, va_list args)
{
   if (statement) {
      va_end(args);
      return; // no error, return
   }

   _lily_globals.error_location = location;

   va_list args_len;
   va_copy(args_len, args);
   size_t length = vsnprintf(NULL, 0, format_string, args_len);
   va_end(args_len);

   if (_lily_globals.error_msg_len < length+1 ||
       _lily_globals.error_msg == NULL) {
      if (_lily_globals.error_msg != NULL)
	 free(_lily_globals.error_msg);

      char *msg = malloc((length+2) * sizeof(char));

      if (msg == NULL) {
	 fprintf(stderr, "WARNING: failed to allocate memory for failed test message!\n");
	 _lily_globals.error_msg = NULL;
	 va_end(args);
	 longjmp(_lily_globals.env, 1);
	 return;
      }
      else {
	 _lily_globals.error_msg = msg;
	 _lily_globals.error_msg_len = length+1;
      }
   }

   vsnprintf(_lily_globals.error_msg, length+1, format_string, args);

   va_end(args);
   longjmp(_lily_globals.env, 1);
}


void lily_assert_msg(bool statement, const char *location,
		     const char *format_string, ...)
{
   va_list args;
   va_start(args, format_string);
   // _lily_assert_msg may long jump, so it takes calls va_end(args) for you
   _lily_assert_msg(statement, location, format_string, args);
}


void _lily_assert_true(const char *statement, bool value, const char *location)
{
   lily_assert_msg(value, location,
		   "%s is not true",
		   statement);
}


void _lily_assert_false(const char *statement, bool value, const char *location)
{
   lily_assert_msg(!value, location,
		   "%s is not false",
		   statement);
}


void _lily_assert_not_null(const char *name, void *ptr, const char *location)
{
   lily_assert_msg(ptr != NULL, location,
		   "%s is NULL",
		   name);
}


void _lily_assert_null(const char *name, void *ptr, const char *location)
{
   lily_assert_msg(ptr == NULL, location,
		   "%s (%p) is not NULL",
		   name, ptr);
}


void _lily_assert_ptr_equal(const char *name_a, const char *name_b,
			    void *a, void *b, const char *location)
{
   lily_assert_msg(a == b, location,
		   "%s (%p) is not equal to %s (%p)",
		   name_a, a, name_b, b);
}


void _lily_assert_ptr_not_equal(const char *name_a, const char *name_b,
			    void *a, void *b, const char *location)
{
   lily_assert_msg(a != b, location,
		   "%s (%p) is equal to %s",
		   name_a, a, name_b);
}


void _lily_assert_int_equal(const char *name_a, const char *name_b,
			    intmax_t a, intmax_t b, const char *location)
{
   lily_assert_msg(a == b, location,
		   "%s (%d) is not equal to %s (%d)",
		   name_a, a, name_b, b);
}


void _lily_assert_int_not_equal(const char *name_a, const char *name_b,
			    intmax_t a, intmax_t b, const char *location)
{
   lily_assert_msg(a != b, location,
		   "%s (%d) is equal to %s",
		   name_a, a, name_b);
}


void _lily_assert_float_equal(const char *name_a, const char *name_b,
			      double a, double b, double epsilon, const char *location)
{
   lily_assert_msg(abs(a - b) <= epsilon,
		   "%s (%f) is not equal to %s (%f) (epsilon: %f)",
		   name_a, a, name_b, b, epsilon);
}


void _lily_assert_float_not_equal(const char *name_a, const char *name_b,
				  double a, double b, double epsilon, const char *location)
{
   lily_assert_msg(abs(a - b) > epsilon,
		   "%s (%f) is equal to %s (%f) (epsilon: %f)",
		   name_a, a, name_b, b, epsilon);
}


void _lily_assert_string_equal(const char *name_a, const char *name_b,
			       char *a, char *b, const char *location)
{
   lily_assert_msg(strcmp(a, b) == 0,
		   "%s ('%s') is not equal to %s ('%s')",
		   name_a, a, name_b, b);
}


void _lily_assert_string_not_equal(const char *name_a, const char *name_b,
				   char *a, char *b, const char *location)
{
   lily_assert_msg(strcmp(a, b) != 0,
		   "%s ('%s') is equal to %s",
		   name_a, a, name_b);
}


void _lily_assert_memory_equal(const char *name_a, const char *name_b,
			       void *a, void *b, size_t size, const char *location)
{
   lily_assert_msg(memcmp(a, b, size) == 0,
		   "%s and %s contain different data", name_a, name_b);
}


void _lily_assert_memory_not_equal(const char *name_a, const char *name_b,
				   void *a, void *b, size_t size, const char *location)
{
   lily_assert_msg(memcmp(a, b, size) == 0,
		   "%s contains the same data s %s", name_a, name_b);
}


/* ======== MOCKS ======== */

lily_queue_t* lily_queue_create()
{
   const size_t size = 256 * sizeof(uint8_t);

   lily_queue_t *q = malloc(sizeof(lily_queue_t));
   q->buf_size = size;
   q->buf = malloc(size);
   q->front = q->buf;
   q->back = q->front;

   return q;
}


void lily_queue_destroy(lily_queue_t *q)
{
   free(q->buf);
   free(q);
}


void _lily_enqueue(lily_queue_t *q, size_t size, uint8_t *data)
{
   size_t used = q->back - q->buf;
   size_t remaining = q->buf_size - used;

   if (remaining < size) {
      /* re-allocate bigger buffer */
      size_t size_new = 2 * q->buf_size;
      uint8_t *buf_new = realloc(q->buf, size_new);
      if (buf_new == NULL) {
	 fprintf(stderr, "failed to allocated %lu bytes for queue %p!\n",
		 size_new, q);
	 return;
      }
      q->buf = buf_new;
   }

   memcpy(q->back, data, size);
   q->back += size;
}


void _lily_dequeue(lily_queue_t *q, size_t size, uint8_t *ptr)
{
   size_t dist = q->back - q->front;
   if (dist < size) {
      fprintf(stderr, "attempted to read %lu bytes out of queue %p, "
	      "which has %lu bytes presently queued\n", size, q, dist);
      return;
   }

   memcpy(ptr, q->front, size);
   q->front += size;
}


lily_mock_t * lily_mock_create()
{
   lily_mock_t *m = malloc(sizeof(lily_mock_t));
   m->n_calls = 0;
   m->arguments = lily_queue_create();
   m->values = lily_queue_create();
   return m;
}

void lily_mock_destroy(lily_mock_t *m)
{
   lily_queue_destroy(m->arguments);
   lily_queue_destroy(m->values);
   free(m);
}

void _lily_mock_call(lily_mock_t *m, struct lily_mock_arg_t *args, size_t n_args)
{
   m->n_calls += 1;

   for (int i=0; i<n_args; i++) {
      _lily_enqueue(m->arguments, args[i].size, args[i].var);
   }
}

void _lily_get_call(lily_mock_t *m,
		    struct lily_mock_arg_t *args,
		    size_t n_args,
		    unsigned int call_num)
{
   size_t stride = 0;
   for (int i=0; i<n_args; i++) {
      stride += args[i].size;
   }

   m->arguments->front = m->arguments->buf + (call_num * stride);
   for (int i=0; i<n_args; i++) {
      _lily_dequeue(m->arguments, args[i].size, args[i].var);
   }
}
