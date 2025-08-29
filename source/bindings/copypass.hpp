#ifndef GAME_COPYPASS_HEADER
#define GAME_COPYPASS_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


/**
 * Library loading function for copy pass type.
 * 
 * @param lua Lua state.
 * @return Number of returned values.
 * 
 * @note Meant to be used in conjunction with [`luaL_requiref`](https://www.lua.org/manual/5.4/manual.html#luaL_requiref).
 */
int luaopen_copypass(lua_State* lua);

/**
 * [-0, +0, m]
 * 
 * Test whether the function argument arg is a copy pass, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to test.
 * @return A pointer to a copy pass, or `nullptr` on failure.
 */
SDL_GPUCopyPass* lua_testcopypass(lua_State* lua, int arg);

/**
 * [-0, +0, v]
 * 
 * Check whether the function argument arg is a copy pass, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A reference to a pointer to a copy pass.
 */
SDL_GPUCopyPass*& lua_checkcopypass(lua_State* lua, int arg);


#endif // GAME_COPYPASS_HEADER