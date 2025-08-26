#ifndef GAME_BINDINGS_SHADER_HEADER
#define GAME_BINDINGS_SHADER_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_shader(lua_State* lua);

SDL_GPUShader* lua_checkshader(lua_State* lua, int index);


#endif // GAME_BINDINGS_SHADER_HEADER