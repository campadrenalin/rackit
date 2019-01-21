#include "unity.h"
#include "../rackit/sdl.c"
#include "test_buffer.c"
#include "test_port.c"
#include "test_module.c"
#include "test_lua.c"

int main(void) {
    UNITY_BEGIN();
    test_Buffer();
    test_Port();
    test_Module();
    test_Lua();
    return UNITY_END();
}
