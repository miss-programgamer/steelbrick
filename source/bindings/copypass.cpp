#include "copypass.hpp"


#include <cstring>

#include "../sdlx.hpp"
#include "../luax.hpp"
#include "../program.hpp"
#include "texture.hpp"
#include "commandbuffer.hpp"


static int call_destructor(lua_State* lua);

static int call_finalizer(lua_State* lua);

static int call_upload(lua_State* lua);


int luaopen_copypass(lua_State* lua)
{
	static const luaL_Reg metatable[]
	{
		{ "upload",  call_upload },
		{ "__close", call_destructor },
		{ "__gc",    call_finalizer },
		{ "__metatable", nullptr },
		{ "__newindex", nullptr },
		{ "__index", nullptr },
		{ nullptr, nullptr },
	};

	if (luaL_newmetatable(lua, "CopyPass"))
	{
		// Fill metatable
		luaL_setfuncs(lua, metatable, 0);
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__metatable");
		lua_pushvalue(lua, -1);
		lua_setfield(lua, -2, "__index");
	}

	return 1;
}


SDL_GPUCopyPass* lua_testcopypass(lua_State* lua, int arg)
{
	if (auto ptr = luaL_testudata(lua, arg, "CopyPass"))
	{ return *(SDL_GPUCopyPass**)ptr; }
	else
	{ return nullptr; }
}


SDL_GPUCopyPass*& lua_checkcopypass(lua_State* lua, int arg)
{
	return *lua_checkudata<SDL_GPUCopyPass*>(lua, arg, "CopyPass");
}


static int call_destructor(lua_State* lua)
{
	auto& pass = lua_checkcopypass(lua, 1);

	SDL_EndGPUCopyPass(pass);
	pass = nullptr;

	return 0;
}


static int call_finalizer(lua_State* lua)
{
	auto& pass = lua_checkcopypass(lua, 1);

	if (pass != nullptr)
	{
		SDL_EndGPUCopyPass(pass);
		pass = nullptr;

		return luaL_error(lua, "CopyPass %p was not closed properly (did you forget to add <close>)", lua_topointer(lua, 1));
	}

	return 0;
}


static int call_upload(lua_State* lua)
{
	auto& program = *lua_getprogram(lua);

	auto pass = lua_checkcopypass(lua, 1);

	if (auto texture = lua_testtexture(lua, 3))
	{
		auto width = lua_gettexturewidth(lua, 3);
		auto height = lua_gettextureheight(lua, 3);

		SDL_GPUTextureRegion region
		{
			.texture = texture,
			.w = width,
			.h = height,
			.d = 1,
		};

		auto surface = IMG_LoadFormat(luaL_checkstring(lua, 2), SDL_PIXELFORMAT_RGBA128_FLOAT);

		if (surface == nullptr)
		{ return luaL_error(lua, "%s", SDL_GetError()); }

		SDL_GPUTransferBufferCreateInfo info
		{
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = surface->w * surface->h * sizeof(float[4]),
		};
		auto buffer = SDL_CreateGPUTransferBuffer(program, &info);

		if (buffer == nullptr)
		{
			SDL_DestroySurface(surface);
			return luaL_error(lua, "%s", SDL_GetError());
		}

		if (auto data = SDL_MapGPUTransferBuffer(program, buffer, false))
		{
			std::memcpy(data, surface->pixels, info.size);
			SDL_UnmapGPUTransferBuffer(program, buffer);
		}
		else
		{
			SDL_DestroySurface(surface);
			SDL_ReleaseGPUTransferBuffer(program, buffer);
			return luaL_error(lua, "%s", SDL_GetError());
		}

		SDL_DestroySurface(surface);

		SDL_GPUTextureTransferInfo transfer
		{
			.transfer_buffer = buffer,
			.pixels_per_row = width,
			.rows_per_layer = height,
		};
		SDL_UploadToGPUTexture(pass, &transfer, &region, false);

		SDL_ReleaseGPUTransferBuffer(program, buffer);
	}
	else
	{}

	return 0;
}