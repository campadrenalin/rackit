#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_audio.h>
#include "time.h"

void audio_callback(void *user_data, Uint8 *raw_buffer, int bytes);

int sdl_err(lua_State *L, const char* fmt);
static int sdl_init (lua_State *L);
static int sdl_play(lua_State *L);
static int sdl_pause(lua_State *L);
static int sdl_finish(lua_State *L);
