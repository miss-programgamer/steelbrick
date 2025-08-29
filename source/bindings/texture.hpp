#ifndef GAME_TEXTURE_HEADER
#define GAME_TEXTURE_HEADER


#include <SDL3/SDL.h>
#include <lua.hpp>


int luaopen_texture(lua_State* lua);

SDL_GPUTexture* lua_testtexture(lua_State* lua, int arg);

SDL_GPUTexture*& lua_checktexture(lua_State* lua, int arg);

Uint32 lua_gettexturewidth(lua_State* lua, int index);

Uint32 lua_gettextureheight(lua_State* lua, int index);


#endif // GAME_TEXTURE_HEADER