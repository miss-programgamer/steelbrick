#ifndef GAME_COLOR_HEADER
#define GAME_COLOR_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


/**
 * Library loading function for color type.
 * 
 * @param lua Lua state.
 * @return Number of returned values.
 * 
 * @note Meant to be used in conjunction with [`luaL_requiref`](https://www.lua.org/manual/5.4/manual.html#luaL_requiref).
 */
int luaopen_color(lua_State* lua);

/**
 * [-0, +0, m]
 * 
 * Test whether the function argument arg is a color, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to test.
 * @return A pointer to a color, or `nullptr` on failure.
 * 
 * @see #luaL_testudata
 */
SDL_FColor* lua_testcolor(lua_State* lua, int arg);

/**
 * [-0, +0, v]
 * 
 * Check whether the function argument arg is a color, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A pointer to a color.
 */
SDL_FColor& lua_checkcolor(lua_State* lua, int arg);


#endif // GAME_COLOR_HEADER