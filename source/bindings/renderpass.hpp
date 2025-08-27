#ifndef GAME_RENDERPASS_HEADER
#define GAME_RENDERPASS_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_renderpass(lua_State* lua);

SDL_GPURenderPass*& lua_checkrenderpass(lua_State* lua, int index);


#endif // GAME_RENDERPASS_HEADER