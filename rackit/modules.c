#include "buffer.h"
#include "sdl_player.h"
const double PI = 3.141592653589793238463;
const double TAU = 2*PI;
const int AMPLITUDE = 13000;
const int SAMPLE_RATE = 44100;
const double sr = SAMPLE_RATE;
const SDL_AudioFormat FORMAT_WANTED = AUDIO_S16SYS;
typedef Sint16 FORMAT;

void print_stack(lua_State *L) {
    int n = lua_gettop(L); //number of arguments
    SDL_Log("Printing stack (%d items)", n);
    for (int i=1; i <= n; i++) {
        int t = lua_type(L, i);
        SDL_Log("%i => <%s> %s", i, lua_typename(L, t), lua_tostring(L, i));
    }
    SDL_Log("---");
}

void* new_class_table(lua_State *L, const char *name, size_t udsize) {
    lua_newtable(L); // +table
    luaL_getmetatable(L, name); // +metatable
    lua_setmetatable(L, -2); // -metatable

    void *ud = lua_newuserdata(L, udsize); // +osc_struct
    lua_setfield(L, -2, "_userdata"); // -osc_struct
    return ud; // Returns pointer to userdata, leaves table on the stack
}

void * get_table_userdata(lua_State *L, int pos) {
    lua_getfield(L, pos, "_userdata"); // +struct
    void *p = lua_touserdata(L, -1);
    lua_pop(L, 1); // -struct
    return p;
}

typedef struct {
    const char* name;
    const char* tr;
    void(*setter)(lua_State*, void*);
} NativeField;

const NativeField* find_native_field(const char* input, const NativeField options[]) {
    for (const NativeField *opt=options; opt->name != NULL; opt++) {
        if (strcmp(input, opt->name) == 0)
            return opt;
    }
    return NULL;
}

int native_index(lua_State *L, const NativeField fields[]) {
    // table | index
    const char *index = luaL_checkstring(L, 2);
    const NativeField *f = find_native_field(index, fields);
    if (f != NULL) {
        lua_pop(L, 1); // -index
        lua_pushstring(L, f->tr); // +tr
    }
    lua_rawget(L, 1);
    return 1;
}

int native_newindex(lua_State *L, const NativeField fields[]) {
    // osc_table | index | value
    const char *index = luaL_checkstring(L, 2);
    const NativeField *f = find_native_field(index, fields);
    if (f != NULL) {
        lua_pushstring(L, f->tr); // +tr
        lua_replace(L, 2); // -index, put tr in place
        f->setter(L, get_table_userdata(L, 1));
    }
    lua_rawset(L, 1);
    return 0;
}

#include "modules/actor.c"
#include "modules/mixer.c"

Buffer buf_master;
Mixer mix_master = { 0, {}, &buf_master };

#include "modules/sine.c"
