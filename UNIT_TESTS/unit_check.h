#ifndef _UNIT_CHECK_H
#define _UNIT_CHECK_H
#include <Arduino.h>
#include "enabled_modules.h"

#ifdef UNIT_CHECK_MODULE

/**
 * @brief Initializes our unit testing framework
 * @returns the status of the module
*/
bool unit_test_mod_init(void);

/**
 * @brief Begins unit test for specific module
 * @param Name of testscase
 * @returns Whether or not we were able to start our testcase
*/
bool unit_testcase_begin(char *testcase_name);

/**
 * @brief Completes testcase for specific module
*/
bool unit_testcase_end(void);

/**
 * @brief Checks if a value is inside/equal to a range of values
 * @param const char *func_name name of tested function
 * @param int x value
 * @param int high upper bounds
 * @param int low lower bounds
 * @return true if x is equal or inside to the high or low bounds, false otherwise
*/
bool assert_testcase_in_range(const char *func_name, int x, int high, int low);

/**
 * @brief Runs a test case, checks if is true, then logs
 * @param const char *func_name name of tested function
 * @param int a 
 * @param int b
 * @returns true if a and b are equal, false otherwise
*/
bool assert_testcase_equal(const char *func_name, int a, int b);

/**
 * @brief Runs a test case, checks if is true, then logs
 * @param const char *func_name name of tested function
 * @param int a 
 * @param int b
 * @returns true if a and b are not equal, false otherwise
*/
bool assert_testcase_not_equal(const char *func_name, int a, int b);

/**
 * @brief Checks if a testcase has returned null, then logs
 * @param const char *func_name name of tested function
 * @param void *ptr parameter that we are checking if is null
 * @returns true if null, false otherwise
*/
bool assert_testcase_null(const char *func_name, void *ptr);

/**
 * @brief Checks if a testcase has returned null, then logs
 * @param const char *func_name name of tested function
 * @param void *ptr parameter that we are checking if is null
 * @returns true if null, false otherwise
*/
bool assert_testcase_not_null(const char *func_name, void *ptr);

/**
 * @brief Checks if specific bits are set from a parameterized value, the logs
 * @param const char *func_name name of tested function
 * @param int value 
 * @param int bitmask
 * @return if bits are set, return true, false otherwise
*/
bool assert_testcase_bit_set(const char *func_name, int value, int bitmask);

/**
 * @brief Checks if specific bits are set from a parameterized value, the logs
 * @param const char *func_name name of tested function
 * @param int value 
 * @param int bitmask
 * @return if bits are set, return true, false otherwise
*/
bool assert_testcase_bit_clear(const char *func_name, int value, int bitmask);

#endif
#endif