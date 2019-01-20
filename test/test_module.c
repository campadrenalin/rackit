#include "unity.h"
#include "../rackit/core.c"

void test_Module_new(void) {
    // Size is dominated by fallback/output buffer
    Module *m = Module_new(NULL, 5);
    TEST_ASSERT_NULL(m->process_cb);
    TEST_ASSERT_EQUAL(0, m->time);
    TEST_ASSERT_EQUAL(0, m->data.phase);
    TEST_ASSERT_EQUAL(5, m->num_ports);
    for (int i=0; i<5; i++) {
        Port *p = &(m->ports[i]);
        TEST_ASSERT_FALSE(p->is_patched);
        TEST_ASSERT_EQUAL(0, p->buf.buf[6]);
    }
}

// Simple ModuleCallbacks for testing purposes
DEF_MC(Count) {
    Buffer *out = Module_buffer(m, 0);
    for (int i=0; i<length; i++)
        (*out)[i] = (Sample)i;
}
DEF_MC(Triple) {
    Buffer *out = Module_buffer(m, 0);
    Buffer *in  = Module_buffer(m, 1);
    for (int i=0; i<length; i++)
        (*out)[i] = 3*(*in)[i];
}

void test_Module_process(void) {
    Module *src = NEW_MODULE(Count,  1);
    Module *dst = NEW_MODULE(Triple, 2);
    Port_set_patch(&dst->ports[1], src, 0); // Output of src patched to input of dst
    TEST_ASSERT_EQUAL_MESSAGE(0, src->ports[0].buf.buf[12], "Not processed yet");

    Module_process(dst, 20, 1000); // Cause recomputes
    TEST_ASSERT_EQUAL_MESSAGE(12, src->ports[0].buf.buf[12], "Src was processed");
    TEST_ASSERT_EQUAL_MESSAGE(36, dst->ports[0].buf.buf[12], "Dst was processed");
}

void test_Module(void) {
    RUN_TEST(test_Module_new);
    RUN_TEST(test_Module_process);
}
