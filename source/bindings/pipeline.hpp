#ifndef GAME_BINDINGS_PIPELINE_HEADER
#define GAME_BINDINGS_PIPELINE_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_pipeline(lua_State* lua);

SDL_GPUGraphicsPipeline* lua_checkpipeline(lua_State* lua, int index);


#endif // GAME_BINDINGS_PIPELINE_HEADER