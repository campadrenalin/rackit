#import "unity.h"
#import "../rackit/sdl.c"
#import "test_buffer.c"
#import "test_port.c"
#import "test_module.c"

int main(void) {
    UNITY_BEGIN();
    test_Buffer();
    test_Port();
    test_Module();
    return UNITY_END();
}
