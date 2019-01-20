#include "unity.h"
#include "../rackit/rework.c"

void test_Buffer_size(void) {
    TEST_ASSERT_EQUAL_INT_MESSAGE(sizeof(Buffer), sizeof(Sample)*2048, "Buffer is 2048 samples long.");
}

void test_Buffer_fill(void) {
    Buffer buf;
    Buffer_fill(&buf, 16.5);
    for (int i=0; i<BUFFER_SIZE; i++)
        TEST_ASSERT_EQUAL_FLOAT(buf[i], 16.5);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_Buffer_size);
    RUN_TEST(test_Buffer_fill);
    return UNITY_END();
}
