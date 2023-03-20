# TeensyOS Unit Testing Framework

A simple framework for testing code functionality inside the TeensyOS.

### Unit Testing Example
```

#include "UNIT_TESTS/unit_check.h"

void unit_test_equal(void){
    // A valid test case
    assert_testcase_equal(__func__, 0, 0);

    // An invalid test case
    assert_testcase_equal(__func__, 1, 0);
}

void unit_test_not_equal(void){
    // A valid test case
    assert_testcase_equal(__func__, 1, 0);

    // An invalid test case
    assert_testcase_equal(__func__, 0, 0);
}

void unit_test_null(void){
    // A valid test case
    assert_testcase_null(__func__, NULL);

    // An invalid test case
    assert_testcase_null(__func__, (void*)0xFF);
}

void unit_test_not_null(void){
    // A valid test case
    assert_testcase_null(__func__, (void*)0xFF);

    // An invalid test case
    assert_testcase_null(__func__, NULL);
}

void unit_test_bit_set(void){
    int bitmask = 0b01010;
    int value_valid  = 0b01111;
    // Valid test case
    assert_testcase_null(__func__,  value_valid, bitmask);

    // Invalid test case
    int value_invalid  = 0b01101;
    assert_testcase_null(__func__,  value, bitmask);
}

void unit_test_bit_not_set(void){
    int bitmask = 0b01010;
    int value_valid  = 0b0101;
    // Valid test case
    assert_testcase_null(__func__,  value_valid, bitmask);

    // Invalid test case
    int value_invalid  = 0b01111;
    assert_testcase_null(__func__,  value, bitmask);
}

void unit_test_all(void){
    unit_test_mod_init();
    unit_testcase_being("Example Testcases");

    unit_test_equal();
    unit_test_not_equal();
    unit_test_null();
    unit_test_not_null();
    unit_test_bit_set();
    unit_test_bit_not_set();
}

```