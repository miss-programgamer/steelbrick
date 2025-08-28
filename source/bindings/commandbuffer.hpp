#ifndef GAME_COMMANDBUFFER_HEADER
#define GAME_COMMANDBUFFER_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


/**
 * @brief Library loading function for command buffer type.
 * 
 * @note Meant to be used in conjunction with [`luaL_requiref`](https://www.lua.org/manual/5.4/manual.html#luaL_requiref).
 * 
 * @param lua Lua state.
 * @return Number of returned values.
 */
int luaopen_commandbuffer(lua_State* lua);

/**
 * [-0, +0, v]
 * 
 * Check whether the function argument arg is a command buffer, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A pointer to a command buffer.
 */
SDL_GPUCommandBuffer*& lua_checkcommandbuffer(lua_State* lua, int arg);


#endif // GAME_COMMANDBUFFER_HEADER