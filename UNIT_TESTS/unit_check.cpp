#include "unit_check.h"

#ifdef UNIT_CHECK_MODULE

static int total_tests = 0;
static int passed_tests = 0;

static inline void print_log(const char *ptr){
    Serial.printf("%s", ptr);
}

static inline void print_func(const char *func_name){
    print_log("Function: ");
    print_log(func_name);
    print_log("Status: ");
}

bool unit_test_mod_init(void){
    total_tests = 0; 
    passed_tests = 0;

    return true;
}

bool unit_testcase_begin(char *testcase_name){
    print_log("\n======================================\n");
    print_log("Test Case Begin: ");
    print_log(testcase_name);
    print_log("\n======================================\n");

    return true;
}

static char out_log[32];
bool unit_testcase_end(void){
    print_log("\n======================================\n");
    sprintf(out_log, "%d/%d", passed_tests, total_tests);
    print_log(out_log);
    print_log(" tests passed\n");
    print_log("Test Case End");
    print_log("\n======================================\n");

    return true;
}

bool assert_testcase_in_range(const char *func_name, int x, int high, int low){
    print_func(func_name);

    if((x <= high) && (x >= low)){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}

bool assert_testcase_equal(const char *func_name, int a, int b){
    print_func(func_name);

    if(a == b){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}

bool assert_testcase_not_equal(const char *func_name, int a, int b){
    print_func(func_name);

    if(a != b){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}

bool assert_testcase_null(const char *func_name, void *ptr){
    print_func(func_name);


    if(ptr == NULL){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}

bool assert_testcase_not_null(const char *func_name, void *ptr){
    print_func(func_name);


    if(ptr != NULL){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}

bool assert_testcase_bit_set(const char *func_name, int value, int bitmask){
    print_func(func_name);

    int applied_bitmask = value & bitmask;
    // If we applied the entire bitmask and all bits are still set
    if(applied_bitmask == bitmask){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}

bool assert_testcase_bit_clear(const char *func_name, int value, int bitmask){
    print_func(func_name);

    int applied_bitmask = value & bitmask;
    // If we applied the entire bitmask and all bits are still set
    if(applied_bitmask == 0){
        print_log("Passed\n");
        return true;
    }

    print_log("Failed\n");
    return false;
}
#endif