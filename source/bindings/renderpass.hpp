#ifndef GAME_RENDERPASS_HEADER
#define GAME_RENDERPASS_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


/**
 * Library loading function for render pass type.
 * 
 * @param lua Lua state.
 * @return Number of returned values.
 * 
 * @note Meant to be used in conjunction with [`luaL_requiref`](https://www.lua.org/manual/5.4/manual.html#luaL_requiref).
 */
int luaopen_renderpass(lua_State* lua);

/**
 * [-0, +0, m]
 * 
 * Test whether the function argument arg is a render pass, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to test.
 * @return A pointer to a render pass, or `nullptr` on failure.
 */
SDL_GPURenderPass* lua_testrenderpass(lua_State* lua, int arg);

/**
 * [-0, +0, v]
 * 
 * Check whether the function argument arg is a render pass, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A pointer to a render pass.
 */
SDL_GPURenderPass*& lua_checkrenderpass(lua_State* lua, int arg);


#endif // GAME_RENDERPASS_HEADER