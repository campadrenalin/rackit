#include "core.c"
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

void print_stack(lua_State *L) {
    int n = lua_gettop(L); //number of arguments
    SDL_Log("Printing stack (%d items)", n);
    for (int i=1; i <= n; i++) {
        int t = lua_type(L, i);
        SDL_Log("%i => <%s> %s", i, lua_typename(L, t), lua_tostring(L, i));
    }
    SDL_Log("---");
}

static int lua_sdl_init(lua_State *L)   { sdl_init();   return 0; }
static int lua_sdl_finish(lua_State *L) { sdl_finish(); return 0; }
static int lua_sdl_play(lua_State *L) {
    struct Module *mod = lua_touserdata(L, 1);
    double ms = luaL_checknumber(L, 2);
    sdl_play(mod, (int)ms);
    return 0;
}

static int lua_Module_new(lua_State *L) {
    // mt | callback | port...
    ModuleCallback callback = lua_touserdata(L, 2);
    int num_ports = lua_gettop(L) - 2;

    // Create userdata and set metatable
    Module *m = lua_newuserdata(L, ModuleSize(num_ports));
    lua_pushvalue(L, 1);
    lua_setmetatable(L, -2);
    lua_rotate(L, 1, 1);

    m->process_cb = callback;
    m->num_ports = num_ports;
    for (int i=num_ports-1; i>0; i--) // Set in reverse order, eating from top of stack
        lua_seti(L, 1, i);
    lua_pop(L, 3);
    return 1;
}

#define STP_CASE(name, result) \
    if (strcmp(index, #name) == 0) return result;
int str_to_pos(lua_State *L, int ind) {
    const char *index = luaL_checkstring(L, ind);
    STP_CASE(out, 0);
    STP_CASE(in, 1);
    STP_CASE(freq, 1);
    STP_CASE(pw, 2);
    STP_CASE(center, 3);
    STP_CASE(offset, 3);
    STP_CASE(scale, 4);
    return 0;
}

static int lua_Module___newindex(lua_State *L) {
    struct Module *m = lua_touserdata(L, 1);
    int pos = lua_isnumber(L, 2) ? luaL_checknumber(L, 2) : str_to_pos(L, 2);
    Sample value = luaL_checknumber(L, 3);
    lua_pop(L, 3);

    Port_set_constant(&m->ports[pos], value);
    return 0;
}

static const struct luaL_Reg TopLevel[] = {
    {"sdl_init",   lua_sdl_init},
    {"sdl_play",   lua_sdl_play},
    {"sdl_finish", lua_sdl_finish},
    {NULL,NULL} // Sentinel value
};

#define STORE_MC(name) \
    lua_pushlightuserdata(L, & MC(name)); \
    lua_setfield(L, -2, #name)

#define STORE_TABLE(name, ...) \
    lua_newtable(L); \
    __VA_ARGS__ \
    lua_setfield(L, -2, #name)

#define STORE_METATABLE(CLASSNAME, ...) \
    luaL_newmetatable(L, #CLASSNAME); \
    __VA_ARGS__ \
    lua_pushcfunction(L, lua_ ## CLASSNAME ## _new); \
    lua_setfield(L, -2, "__call"); \
    lua_pushvalue(L, -1); \
    lua_setmetatable(L, -2); \
    lua_setfield(L, -2, #CLASSNAME)

#define STORE_METHOD(class, name) \
    lua_pushcfunction(L, lua_ ## class ## _ ## name); \
    lua_setfield(L, -2, #name)

int luaopen_rackit (lua_State *L) {
    luaL_newlib(L, TopLevel);

    // TODO: mod.freq = fma.out
    // TODO: rk.Sine(fma.out)
    // TODO: GC Tests
    // TODO: rk.Sine(220) | rk.LFO(440, 80) | rk.Sine() | rk.Master
    // TODO: mod:play(2000)
    STORE_TABLE(modules,
        STORE_TABLE(types, 
            STORE_MC(Sine);
            STORE_MC(Saw);
            STORE_MC(Square);
            STORE_MC(FMA);
        );
    );
    STORE_METATABLE(Module,
        STORE_METHOD(Module, __newindex);
    );

    return 1;
}
