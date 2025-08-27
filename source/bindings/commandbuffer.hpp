#ifndef GAME_COMMANDBUFFER_HEADER
#define GAME_COMMANDBUFFER_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_commandbuffer(lua_State* lua);

SDL_GPUCommandBuffer*& lua_checkcommandbuffer(lua_State* lua, int index);


#endif // GAME_COMMANDBUFFER_HEADER