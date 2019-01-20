#include "unity.h"
#include "../rackit/core.c"

void test_Port_size(void) {
    // Size is dominated by fallback/output buffer
    TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(Port), sizeof(PortBuffer), "Port is big enough to handle largest case");
}

void test_Port_is_patched(void) {
    Port p = {0};
    TEST_ASSERT_FALSE(p.is_patched);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(p.buf.buf[5], 0, "Initialized with empty buffer");

    Module m;
    Port_set_patch(&p, &m, 3);
    TEST_ASSERT_TRUE(p.is_patched);
    TEST_ASSERT_EQUAL_MESSAGE(p.patch.module, &m, "p.patch.module was set correctly");
    TEST_ASSERT_EQUAL_MESSAGE(p.patch.port,    3, "p.patch.port was set correctly");

    Port_set_constant(&p, 2.5);
    TEST_ASSERT_FALSE(p.is_patched);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(p.buf.buf[5], 2.5, "Initialized with filled buffer");

    Port_set_patch(&p, &m, 3); // Give something to reset
    Port_reset(&p);
    TEST_ASSERT_FALSE(p.is_patched);
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(p.buf.buf[5], 0, "Back to zero");
}

void test_Port_find_buffer_raw(void) {
    Port p = {0};
    Buffer *got = Port_find_buffer(&p);
    TEST_ASSERT_NOT_NULL(got);
    TEST_ASSERT_EQUAL(got, &p.buf.buf);
}

// TODO: Exercise recursive case
void test_Port_find_buffer_patched(void) {
    Module *m = Module_new(4);
    Port p = {0};
    Port_set_patch(&p, m, 2);

    Buffer *got = Port_find_buffer(&p);
    TEST_ASSERT_NOT_NULL(got);
    TEST_ASSERT_EQUAL(got, &m->ports[2].buf.buf);
}

void test_Port(void) {
    RUN_TEST(test_Port_size);
    RUN_TEST(test_Port_is_patched);
    RUN_TEST(test_Port_find_buffer_raw);
    RUN_TEST(test_Port_find_buffer_patched);
}
