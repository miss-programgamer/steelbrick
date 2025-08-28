#ifndef GAME_BINDINGS_HEADER
#define GAME_BINDINGS_HEADER


#include <lua.hpp>


/**
 * @brief Open the program's Lua bindings globally.
 * 
 * @param lua Lua state.
 */
void lua_openbindings(lua_State* lua);


#endif // GAME_BINDINGS_HEADER