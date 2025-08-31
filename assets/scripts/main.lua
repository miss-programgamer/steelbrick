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
			pitch = 20,
			rate = "vertex",
		},
	},
	inputs = {
		{
			location = 0,
			buffer = 0,
			format = "float3",
			offset = 0,
		},
		{
			location = 1,
			buffer = 0,
			format = "float2",
			offset = 12,
		},
	},
}

local sampler = Sampler{
	min = "linear",
	mag = "nearest",
	mipmap = "linear",
	umode = "repeat",
	vmode = "repeat",
	wmode = "repeat",
	anisotropy = true,
}

local texture = Texture(32, 32)

do local commands <close> = CommandBuffer()
	do local pass <close> = commands:copypass()
		pass:upload("assets/textures/brick.png", texture)
	end
end

---@type DrawEvent
function draw(delta)
	local commands <close> = CommandBuffer "display"

	do local pass <close> = commands:renderpass(Color "black")
		pipeline:bind(pass)
		-- actually make draw calls
	end
end