#ifndef GAME_LUAX_HEADER
#define GAME_LUAX_HEADER


#include <string_view>

#include <lua.hpp>


/**
 * [-0, +0, m]
 * 
 * Equivalent to calling [`lua_tolstring`](https://www.lua.org/manual/5.4/manual.html#lua_tolstring)
 *  except that the results are packed into a `std::string_view`.
 * 
 * @param lua Lua state.
 * @param index Stack index of the string to obtain.
 * @return A `std::string_view` of the C string at the given index.
 */
std::string_view lua_tostringview(lua_State* lua, int index);

/**
 * [-0, +1, e]
 * 
 * Equivalent to calling [`luaL_tolstring`](https://www.lua.org/manual/5.4/manual.html#luaL_tolstring)
 *  except that the results are packed into a `std::string_view`.
 * 
 * Notably, this function handles the `__tostring` metamethod, unlike `lua_tostringview`.
 * 
 * @param lua Lua state.
 * @param index Stack index of the value to stringify.
 * @return A `std::string_view` of the value at the given index.
 */
std::string_view lua_tostringviewex(lua_State* lua, int index);

/**
 * [-0, +0, v]
 * 
 * Equivalent to calling [`luaL_checklstring`](https://www.lua.org/manual/5.4/manual.html#luaL_checklstring)
 *  except that the results are packed into a `std::string_view`.
 * 
 * @param lua Lua state.
 * @param arg Function argument of the string to obtain.
 * @return A `std::string_view` of the argument at the given index.
 */
std::string_view lua_checkstringview(lua_State* lua, int arg);

/**
 * [-0, +0, e]
 * 
 * Get the length of the table at the given index.
 * 
 * @param lua Lua state.
 * @param index Stack index of the table to inspect.
 * @return The length of the table. 
 */
lua_Integer lua_getlen(lua_State* lua, int index);

/**
 * [-0, +1, e]
 * 
 * Push the function `debug.traceback` onto the stack and return its index (the stack top).
 * 
 * @param lua Lua state.
 * @return The stack index of the newly pushed traceback function.
 */
int lua_pushtraceback(lua_State* lua);

/**
 * [-0, +1, m]
 * 
 * Push a new full userdata of size `sizeof(T)` onto the stack and return a pointer to it.
 * 
 * @tparam T The type to be contained in the resulting full userdata.
 * @param lua Lua state.
 * @param nuvalue Number of uservalues to give our userdata.
 * @return A pointer to the newly allocated memory block.
 */
template<typename T>
T* lua_newudata(lua_State* lua, int nuvalue = 0)
{
	return (T*)lua_newuserdatauv(lua, sizeof(T), nuvalue);
}

/**
 * [-0, +0, v]
 * 
 * Check that the metatable of the userdata at the given index matches the one named by `tname` in the registry.
 * 
 * @tparam T The type of the userdata's memory block.
 * @param lua Lua state.
 * @param index Stack index of the value to check.
 * @param tname Name of the metatable in the Lua registry.
 * @return A pointer to the userdata's memory block.
 */
template<typename T>
T* lua_checkudata(lua_State* lua, int arg, const char* tname)
{
	return (T*)luaL_checkudata(lua, arg, tname);
}

/**
 * @brief Property getter/setter function signature.
 * 
 * @tparam T The type with properties being accessed.
 */
template<typename T>
using lua_Prop = void(*)(lua_State*, T&);


#endif // GAME_LUAX_HEADER