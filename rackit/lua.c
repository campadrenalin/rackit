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
    for (int i=num_ports-1; i>=0; i--) // Set in reverse order, eating from top of stack
        lua_seti(L, 1, i);
    lua_pop(L, 2);
    return 1;
}

struct ParamName {
    const char* name;
    int port_num;
    Sample default_value;
};
static struct ParamName std_params[] = {
    {"out", 0, 0},
    {"in1", 1, 0}, // "foo.in" is forbidden in Lua because "in" is a keyword
    {"in2", 2, 0},
    {"in3", 3, 0},
    {"freq", 1, 440}, // A4
    {"pw",   2, 0.5},
    {"center", 2, 0},
    {"offset", 2, 0},
    {"scale",  3, 1},
    {NULL, 0, 0}, // Sentinel value
};

struct ParamName *find_pn(const char* index) {
    struct ParamName *pn;
    for (pn = &std_params[0]; pn->name != NULL; pn++) {
        if (strcmp(index, pn->name) == 0)
            return pn;
    }
    return pn;
}

int str_to_pos(lua_State *L, int ind) {
    const char *index = luaL_checkstring(L, ind);
    return find_pn(index)->port_num;
}

#define P(name) \
    if (top < next++) \
        lua_pushnumber(L, find_pn(#name)->default_value);

#define MOD_CONSTRUCTOR(name, ...) \
static int lua_Module_ ## name (lua_State *L) { \
    int top = lua_gettop(L); \
    int next = 1; \
    __VA_ARGS__ \
    luaL_getmetatable(L, "Module"); \
    lua_pushlightuserdata(L, & MC(Sine)); \
    lua_pushnumber(L, 0); \
    lua_rotate(L, 1, 3); \
    /* mt | callback | 0 (out) | params */ \
    return lua_Module_new(L); \
}

MOD_CONSTRUCTOR(Sine,   P(freq));
MOD_CONSTRUCTOR(Saw,    P(freq));
MOD_CONSTRUCTOR(Square, P(freq));
MOD_CONSTRUCTOR(FMA,    P(in1); P(center); P(scale));
#undef P

static int lua_Module___newindex(lua_State *L) {
    struct Module *m = lua_touserdata(L, 1);
    int pos = lua_isnumber(L, 2) ? luaL_checknumber(L, 2) : str_to_pos(L, 2);
    Port* port = &m->ports[pos];
    if (lua_isnumber(L, 3)) {
        Sample value = luaL_checknumber(L, 3); lua_pop(L, 3);
        Port_set_constant(port, value);
    } else {
        lua_getfield(L, 3, "module"); // 4
        lua_getfield(L, 3, "pos"); // 5
        Port_set_patch(port, lua_touserdata(L, 4), luaL_checknumber(L, 5));
    }
    return 0;
}

static int lua_Port_new(lua_State *L);
static int lua_Module___index(lua_State *L) {
    return lua_Port_new(L);
}

static int lua_Port_new(lua_State *L) {
    struct Module *m = lua_touserdata(L, 1);
    int pos = lua_isnumber(L, 2) ? luaL_checknumber(L, 2) : str_to_pos(L, 2);
    lua_newtable(L);
    lua_rotate(L, 1, 1);
    // table | m | pos

    luaL_getmetatable(L, "Port"); // +mt
    lua_setmetatable(L, 1); // -mt

    // Fix position
    lua_pop(L, 1);
    lua_pushnumber(L, pos);

    lua_setfield(L, 1, "pos"); // -pos
    lua_setfield(L, 1, "module"); // -m
    return 1;
}
static int lua_Port_is_patched(lua_State *L) {
    lua_getfield(L, 1, "module"); // 2
    lua_getfield(L, 1, "pos"); // 3
    struct Module *m = lua_touserdata(L, 2);
    Port* port = &m->ports[(int)luaL_checknumber(L, 3)];

    lua_pushboolean(L, port->is_patched);
    return 1;
}
static int lua_Port_value(lua_State *L) {
    lua_getfield(L, 1, "module"); // 2
    lua_getfield(L, 1, "pos"); // 3
    struct Module *m = lua_touserdata(L, 2);
    Port* port = &m->ports[(int)luaL_checknumber(L, 3)];
    lua_pushnumber(L, port->buf.buf[0]);
    return 1;
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

#define SELF_INDEX \
    lua_pushvalue(L, -1); \
    lua_setfield(L, -2, "__index")

int luaopen_rackit (lua_State *L) {
    luaL_newlib(L, TopLevel);

    // TODO: Revive testing from lua script
    // TODO: rk.Sine(220) | rk.LFO(440, 80) | rk.Sine() | rk.Master
    // TODO: GC Tests
    STORE_METHOD(Module, Sine);
    STORE_METHOD(Module, Saw);
    STORE_METHOD(Module, Square);
    STORE_METHOD(Module, FMA);
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
        STORE_METHOD(Module, __index);
    );
    STORE_METATABLE(Port,
        STORE_METHOD(Port, is_patched);
        STORE_METHOD(Port, value);
        SELF_INDEX;
    );

    return 1;
}

/* LOVE2D planning notes:
 *
 * -- At init
 * buffercount = 8
 * source = love.audio.newQueueableSource(samplerate, bitdepth, channels, buffercount-1)
 * buffers = {} -- SoundData objects
 * for i=1,buffercount do
 *   buffers[i] = love.sound.newSoundData(samples, rate, bits, channels)
 * end
 * buf_next = 1
 *
 * -- In game loop
 * while source:getFreeBufferCount() > 1 do
 *   buffer = buffers[buf_next]
 *   buf_next = (buf_next % buffercount)+1
 *   rackit.fill_love_buffer(mod, buffer:getPointer())
 *   source:queue(buffer)
 * end
 */
