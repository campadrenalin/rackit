#include "unity.h"
#include "../rackit/core.c"

void test_Module_new(void) {
    // Size is dominated by fallback/output buffer
    Module *m = Module_new(5);
    TEST_ASSERT_NULL(m->process_cb);
    TEST_ASSERT_EQUAL(m->time, 0);
    TEST_ASSERT_EQUAL(m->data.phase, 0);
    TEST_ASSERT_EQUAL(m->num_ports, 5);
    for (int i=0; i<5; i++) {
        Port *p = &(m->ports[i]);
        TEST_ASSERT_FALSE(p->is_patched);
        TEST_ASSERT_EQUAL(p->buf.buf[6], 0);
    }
}

void test_Module(void) {
    RUN_TEST(test_Module_new);
}
