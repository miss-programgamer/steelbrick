#include "color.hpp"


#include "../luax.hpp"
#include "../hashmap.hpp"


static int call_constructor(lua_State* lua);

static int meta_index(lua_State* lua);

static int meta_newindex(lua_State* lua);


int luaopen_color(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "__index",    meta_index },
		{ "__newindex", meta_newindex },
		{ "__metatable", nullptr },
		{ nullptr, nullptr },
	};

	static const luaL_Reg callable[]
	{
		{ "__call", call_constructor },
		{ "__metatable", nullptr },
		{ nullptr, nullptr },
	};

	if (luaL_newmetatable(lua, "Color"))
	{
		// Fill metatable.
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");

		// Make metatable callable.
		luaL_newlib(lua, callable);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");
		lua_setmetatable(lua, -2);
	}

	return 1;
}


SDL_FColor& lua_checkcolor(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_FColor>(lua, arg, "Color");
}


static int call_constructor(lua_State* lua)
{
	auto& color = *lua_newudata<SDL_FColor>(lua);
	luaL_setmetatable(lua, "Color");
	SDL_zero(color);

	if (lua_type(lua, 2) == LUA_TTABLE)
	{
		lua_getfield(lua, 2, "r");
		color.r = luaL_optnumber(lua, -1, 0.0);

		lua_getfield(lua, 2, "g");
		color.g = luaL_optnumber(lua, -1, 0.0);

		lua_getfield(lua, 2, "b");
		color.b = luaL_optnumber(lua, -1, 0.0);

		lua_getfield(lua, 2, "a");
		color.a = luaL_optnumber(lua, -1, 1.0);

		lua_pop(lua, 4);
	}
	else
	{
		switch (lua_gettop(lua) - 2)
		{
			case 0:
				break;

			case 1:
			case 2:
			{
				auto v = (float)luaL_checknumber(lua, 2);
				auto a = (float)luaL_optnumber(lua, 3, 1.0);
				color = SDL_FColor{ .r = v, .g = v, .b = v, .a = a };
				break;
			}

			case 3:
			case 4:
			default:
			{
				color = SDL_FColor
				{
					.r = (float)luaL_checknumber(lua, 2),
					.g = (float)luaL_checknumber(lua, 3),
					.b = (float)luaL_checknumber(lua, 4),
					.a = (float)luaL_optnumber(lua, 5, 1.0),
				};
				break;
			}
		}
	}

	return 1;
}


static void get_r(lua_State* lua, SDL_FColor& color)
{ lua_pushnumber(lua, color.r); }

static void get_g(lua_State* lua, SDL_FColor& color)
{ lua_pushnumber(lua, color.g); }

static void get_b(lua_State* lua, SDL_FColor& color)
{ lua_pushnumber(lua, color.b); }

static void get_a(lua_State* lua, SDL_FColor& color)
{ lua_pushnumber(lua, color.a); }


static int meta_index(lua_State* lua)
{
	static const Game::HashMap<std::string, lua_Prop<SDL_FColor>> getters
	{
		{ "r", get_r },
		{ "g", get_g },
		{ "b", get_b },
		{ "a", get_a },
	};

	auto key = lua_checkstringview(lua, 2);

	if (auto it = getters.find(key); it != getters.end())
	{ it->second(lua, lua_checkcolor(lua, 1)); }
	else
	{ lua_pushnil(lua); }

	return 1;
}


static void set_r(lua_State* lua, SDL_FColor& color)
{ color.r = luaL_checknumber(lua, 3); }

static void set_g(lua_State* lua, SDL_FColor& color)
{ color.g = luaL_checknumber(lua, 3); }

static void set_b(lua_State* lua, SDL_FColor& color)
{ color.b = luaL_checknumber(lua, 3); }

static void set_a(lua_State* lua, SDL_FColor& color)
{ color.a = luaL_checknumber(lua, 3); }


static int meta_newindex(lua_State* lua)
{
	static const Game::HashMap<std::string, lua_Prop<SDL_FColor>> setters
	{
		{ "r", set_r },
		{ "g", set_g },
		{ "b", set_b },
		{ "a", set_a },
	};

	auto key = lua_checkstringview(lua, 2);

	if (auto it = setters.find(key); it != setters.end())
	{ it->second(lua, lua_checkcolor(lua, 1)); }

	return 0;
}