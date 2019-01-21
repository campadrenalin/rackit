#include "unity.h"
#include "../rackit/core.c"
#include "../rackit/lua.c"

static lua_State *TestL;
lua_State *lua_test_setup() {
    lua_State *L = luaL_newstate();
    luaL_openlibs(L); // Standard libs

    luaopen_rackit(L); // Load this module and store it in the global "rk"
    lua_setglobal(L, "rk");

    return L;
}

#define LUA_SETUP     TestL = lua_test_setup()
#define LUA_TEARDOWN  lua_close(TestL)
#define RUN_LUA_TEST(test) \
    LUA_SETUP; RUN_TEST(test); LUA_TEARDOWN;

void lua_rackit_exec(lua_State *L, const char* program) {
    if (luaL_dostring(L, program)) // Returns true on error
        TEST_FAIL_MESSAGE(luaL_checkstring(L, -1)); // Error is top of stack
}

#define LUA_EXEC(program) lua_rackit_exec(TestL, program)
#define TEST_LUA_ASSERT(program) LUA_EXEC("assert(" program ")")

void test_Lua_module_loaded(void) {
    TEST_LUA_ASSERT("rk");
    TEST_LUA_ASSERT("rk.sdl_play");
    TEST_LUA_ASSERT("type(rk.sdl_play) == 'function'");
    TEST_LUA_ASSERT("type(rk.modules.types.Sine) == 'userdata'");
    TEST_LUA_ASSERT("type(rk.modules.types.FMA)  == 'userdata'");
}

void test_Lua_module_play_sin(void) {
    LUA_EXEC("mod = rk.Module(rk.modules.types.Sine, 0, 440)");
    TEST_LUA_ASSERT("type(mod) == 'userdata'");
    LUA_EXEC("rk.sdl_init()");
    LUA_EXEC("rk.sdl_play(mod, 200)");
    LUA_EXEC("mod[1] = 480");
    LUA_EXEC("rk.sdl_play(mod, 200)");
    LUA_EXEC("rk.sdl_finish()");
}

void test_Lua(void) {
    RUN_LUA_TEST(test_Lua_module_loaded);
    RUN_LUA_TEST(test_Lua_module_play_sin);
}
