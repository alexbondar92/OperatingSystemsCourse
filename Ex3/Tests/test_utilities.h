#ifndef TEST_UTILITIES_H_
#define TEST_UTILITIES_H_

#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

/**
 * These macros are here to help you create tests more easily and keep them
 * clear
 *
 * The basic idea with unit-testing is create a test function for every real
 * function and inside the test function declare some variables and execute the
 * function under test.
 *
 * Use the ASSERT_TEST to verify correctness of values.
 */

/**
 * Evaluates b and continues if b is true.
 * If b is false, ends the test by returning false and prints a detailed
 * message about the failure.
 */
#define ASSERT_TEST(b) do { \
        if (!(b)) { \
                printf("\nAssertion failed at %s:%d %s\n",__FILE__,__LINE__,#b); \
                return false; \
        } \
} while (0)

/**
 * Macro used for running a test from the main function
 */
#define TEST_EQUALS(result, a, b) if ((result) && ((a) != (b))) { \
								result = false; \
							}

#define TEST_DIFFERENT(result, a, b) if ((result) && ((a) == (b))) { \
								result = false; \
							}

#define TEST_TRUE(result, bool) if ((result) && !(bool)) { \
								result = false; \
								assert(result == true); \
							}

#define TEST_FALSE(result, bool) if ((result) && (bool)) { \
								result = false; \
							}

#define RUN_TEST(name)  printf("Running %s... \n", #name); \
						if (!name()) { \
							printf("%s [FAILED]\n", #name);		\
							return false; \
						}								\
						printf("%s [SUCCESS]\n", #name)
						
#define RUN_TEST_CHILD(test_child, name)  test_child = fork(); \
						if (test_child == 0) { \
							RUN_TEST(name); \
							return true; \
						} \
						wait(NULL)
/**
 * These two macros are to help you initialize a set of examples. Look at
 * list_example_test.h for an example of how they can be used to save a lot of code
 */
#define SET_UP(Typename) Typename examples = setUp()
#define TEAR_DOWN() tearDown(examples)

#endif /* TEST_UTILITIES_H_ */
