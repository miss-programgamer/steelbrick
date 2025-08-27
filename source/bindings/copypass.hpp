#ifndef GAME_COPYPASS_HEADER
#define GAME_COPYPASS_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_copypass(lua_State* lua);

SDL_GPUCopyPass*& lua_checkcopypass(lua_State* lua, int index);


#endif // GAME_COPYPASS_HEADER