#include "bindings.hpp"


#include "bindings/shader.hpp"
#include "bindings/pipeline.hpp"
#include "bindings/commandbuffer.hpp"


void lua_openbindings(lua_State* lua)
{
	auto top = lua_gettop(lua);
	luaL_requiref(lua, "Shader", luaopen_shader, true);
	luaL_requiref(lua, "Pipeline", luaopen_pipeline, true);
	luaL_requiref(lua, "CommandBuffer", luaopen_commandbuffer, true);
	lua_settop(lua, top);
}