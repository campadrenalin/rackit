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
    ModuleCallback callback = lua_touserdata(L, 1);
    int num_ports = lua_gettop(L) - 1;

    Module *m = lua_newuserdata(L, ModuleSize(num_ports));
    m->process_cb = callback;
    m->num_ports = num_ports;
    for (int i=0; i<num_ports; i++) {
        Sample value = luaL_checknumber(L, i+2);
        Port_set_constant(&m->ports[i], value); // TODO: Support setting patches
    }
    return 1;
}

static const struct luaL_Reg TopLevel[] = {
    {"sdl_init",   lua_sdl_init},
    {"sdl_play",   lua_sdl_play},
    {"sdl_finish", lua_sdl_finish},
    {"Module",     lua_Module_new},
    {NULL,NULL} // Sentinel value
};

#define STORE_MC(name) \
    lua_pushlightuserdata(L, & MC(name)); \
    lua_setfield(L, -2, #name);

#define STORE_TABLE(name, ...) \
    lua_newtable(L); \
    __VA_ARGS__ \
    lua_setfield(L, -2, #name);

int luaopen_rackit (lua_State *L) {
    luaL_newlib(L, TopLevel);

    STORE_TABLE(modules,
        STORE_TABLE(types, 
            STORE_MC(Sine)
            STORE_MC(Saw)
            STORE_MC(Square)
            STORE_MC(FMA)
        )
    )
    return 1;
}
