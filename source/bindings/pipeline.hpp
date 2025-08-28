#ifndef GAME_BINDINGS_PIPELINE_HEADER
#define GAME_BINDINGS_PIPELINE_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


/**
 * @brief Library loading function for graphics pipeline type.
 * 
 * @note Meant to be used in conjunction with [`luaL_requiref`](https://www.lua.org/manual/5.4/manual.html#luaL_requiref).
 * 
 * @param lua Lua state.
 * @return Number of returned values.
 */
int luaopen_pipeline(lua_State* lua);

/**
 * [-0, +0, m]
 * 
 * Check whether the function argument arg is a graphics pipeline, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A pointer to a graphics pipeline, or `nullptr` on failure.
 */
SDL_GPUGraphicsPipeline* lua_testpipeline(lua_State* lua, int arg);

/**
 * [-0, +0, v]
 * 
 * Check whether the function argument arg is a graphics pipeline, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A pointer to a graphics pipeline.
 */
SDL_GPUGraphicsPipeline*& lua_checkpipeline(lua_State* lua, int arg);


#endif // GAME_BINDINGS_PIPELINE_HEADER