local vshader = Shader("assets/shaders/default.hlsl", { entry = "vMain", stage = "vertex" })
local fshader = Shader("assets/shaders/default.hlsl", { entry = "fMain", stage = "fragment" })

local pipeline = Pipeline{
	vertex = vshader,
	fragment = fshader,
	primitive = "trianglelist",
	targets = {
		{ format = "display" }
	},
	buffers = {
		{
			slot = 0,
			pitch = 0,
			rate = "vertex",
		},
	},
	inputs = {
		{
			location = 0,
			buffer = 0,
			format = "float4",
			offset = 0,
		},
		{
			location = 1,
			buffer = 0,
			format = "float2",
			offset = 16,
		},
	},
}