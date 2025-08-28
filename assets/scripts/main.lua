local vshader = Shader("assets/shaders/default.hlsl", {
	entry = "vMain",
	stage = "vertex",
})

local fshader = Shader("assets/shaders/default.hlsl", {
	entry = "fMain",
	stage = "fragment",
})

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
			pitch = 24,
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

local color1 = Color(0.5, 0, 0, 1)
color1.r = 0.75

local color2 = Color{ r = 1 }
color2.b = 0.25

---@type DrawEvent
function draw(delta)
	local commands <close> = CommandBuffer()

	do local pass <close> = commands:copypass()
		-- copy data to GPU
	end

	do local pass <close> = commands:renderpass(pipeline)
		-- actually make draw calls
	end
end