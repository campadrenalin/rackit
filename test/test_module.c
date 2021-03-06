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

void test_Module_Sine(void) {
    Module *sine = NEW_MODULE(Sine, 2); // out, freq
    Port_set_constant(&sine->ports[1], 440);
    Module_process(sine, 100, sine->time+100);
    Buffer *out = Module_buffer(sine, 0);

    int cursor = 1;
    for (int i=0; i<100; i++, cursor++)
        TEST_ASSERT_EQUAL_FLOAT(sin(cursor * 440 * TAU / SR), (*out)[i]);

    // Compute again, and verify that we pick up where we left off
    Module_process(sine, 100, sine->time+100);
    for (int i=0; i<100; i++, cursor++)
        TEST_ASSERT_EQUAL_FLOAT(sin(cursor * 440 * TAU / SR), (*out)[i]);
}

void test_Module_FMA(void) {
    Module *src = NEW_MODULE(Count, 1);
    Module *fma = NEW_MODULE(FMA, 4);
    Port_set_patch(&fma->ports[1], src, 0); // in: src.out
    Port_set_constant(&fma->ports[2], 100); // center: 100
    Port_set_constant(&fma->ports[3], 2);   // scale: 2

    Buffer *out = Module_buffer(fma, 0);
    Module_process(fma, 10, fma->time+100);
    for (int i=0; i<10; i++)
        TEST_ASSERT_EQUAL_FLOAT(i*2+100, OUT);
}

void test_Module(void) {
    RUN_TEST(test_Module_new);
    RUN_TEST(test_Module_process);
    RUN_TEST(test_Module_Sine);
    RUN_TEST(test_Module_FMA);
}
