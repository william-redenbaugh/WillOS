#include "os_unit_test.h"
#include "../unit_check.h"

#include "OS/OSThreadKernel.h"
#include "OS/OSMutexKernel.h"

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

    MutexLock test_lock;

    int n = test_lock.lock(100);
    assert_testcase_equal(__func__, n, 1);

    n = test_lock.lock(100);
    assert_testcase_equal(__func__, n, 0);

    test_lock.unlock();
    n = test_lock.lock(100);
    assert_testcase_equal(__func__, n, 1);
}

void os_init_testrun(void){
    Serial.begin(9600);
    os_thread_delay_s(5);
    char unit_test[] = "OS Unit Tests";
    unit_testcase_begin(unit_test);
    test_thread_spawn_success();
    test_mutex_success();
}