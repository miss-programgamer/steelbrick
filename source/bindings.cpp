#include "bindings.hpp"


#include "bindings/color.hpp"
#include "bindings/shader.hpp"
#include "bindings/texture.hpp"
#include "bindings/sampler.hpp"
#include "bindings/pipeline.hpp"
#include "bindings/copypass.hpp"
#include "bindings/renderpass.hpp"
#include "bindings/commandbuffer.hpp"


void lua_openbindings(lua_State* lua)
{
	auto top = lua_gettop(lua);
	luaL_requiref(lua, "Color", luaopen_color, true);
	luaL_requiref(lua, "Shader", luaopen_shader, true);
	luaL_requiref(lua, "Texture", luaopen_texture, true);
	luaL_requiref(lua, "Sampler", luaopen_sampler, true);
	luaL_requiref(lua, "Pipeline", luaopen_pipeline, true);
	luaL_requiref(lua, "CopyPass", luaopen_copypass, false);
	luaL_requiref(lua, "RenderPass", luaopen_renderpass, false);
	luaL_requiref(lua, "CommandBuffer", luaopen_commandbuffer, true);
	lua_settop(lua, top);
}