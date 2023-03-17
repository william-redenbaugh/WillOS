#include "os_unit_test.h"
#include "../unit_check.h"
#include "OS/OSThreadKernel.h"

void test_thread_func(void *ptr){
    int *n = (int*)ptr;
    *n = 1;
    os_kill_self_thread();
}

void test_thread_spawn_success(void){
    int n = 0;
    os_thread_id_t id = os_add_thread(test_thread_func, (void*)&n, 64, -1, NULL);

    os_thread_delay_s(1);
    assert_testcase_not_equal(__func__, id, -1);
    assert_testcase_equal(__func__, n, 1);
}

void test_mutex_success(void){

}

void test_thread_init(void){
    char unit_test[] = "OS Unit Tests";
    unit_testcase_begin(unit_test);
    test_thread_spawn_success();
}