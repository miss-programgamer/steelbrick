#ifndef GAME_SAMPLER_HEADER
#define GAME_SAMPLER_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_sampler(lua_State* lua);

SDL_GPUSampler* lua_testsampler(lua_State* lua, int arg);

SDL_GPUSampler*& lua_checksampler(lua_State* lua, int arg);


#endif // GAME_SAMPLER_HEADER