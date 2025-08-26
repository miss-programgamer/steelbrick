#include "bindings.hpp"


#include "bindings/shader.hpp"
#include "bindings/pipeline.hpp"


void luaopen_game(lua_State* lua)
{
	luaL_requiref(lua, "Shader", luaopen_shader, true);
	luaL_requiref(lua, "Pipeline", luaopen_pipeline, true);
}