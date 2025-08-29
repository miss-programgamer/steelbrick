#ifndef GAME_BINDINGS_SHADER_HEADER
#define GAME_BINDINGS_SHADER_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


/**
 * Library loading function for shader type.
 * 
 * @param lua Lua state.
 * @return Number of returned values.
 * 
 * @note Meant to be used in conjunction with [`luaL_requiref`](https://www.lua.org/manual/5.4/manual.html#luaL_requiref).
 */
int luaopen_shader(lua_State* lua);

/**
 * [-0, +0, m]
 * 
 * Test whether the function argument arg is a shader, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to test.
 * @return A pointer to a shader, or `nullptr` on failure.
 */
SDL_GPUShader* lua_testshader(lua_State* lua, int arg);

/**
 * [-0, +0, v]
 * 
 * Check whether the function argument arg is a shader, then return it if so.
 * 
 * @param lua Lua state.
 * @param arg Argument index to check.
 * @return A pointer to a shader.
 */
SDL_GPUShader*& lua_checkshader(lua_State* lua, int arg);


#endif // GAME_BINDINGS_SHADER_HEADER