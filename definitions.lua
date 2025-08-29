---@meta


---Called every frame with the elapsed time since the last frame.
---@alias DrawEvent fun(delta: number)


---Represents a color where each component is a value from 0 to 1.
---@class Color
---@field r number Red component of a color.
---@field g number Green component of a color.
---@field b number Blue component of a color.
---@field a number Alpha component of a color.
Color = {}

---Represents color components that can be passed to the color constructor.
---@alias ColorParts { r: number?, g: number?, b: number?, a: number? }


---Construct a color from RGBA values.
---@param v number RGB components of a color.
---@param a number? Alpha component of a color.
---@return Color # The resulting color.
function Color(v, a) end

---Construct a color from RGBA values.
---@param r number Red component of a color.
---@param g number Green component of a color.
---@param b number Blue component of a color.
---@param a number? Alpha component of a color.
---@return Color # The resulting color.
function Color(r, g, b, a) end

---Construct a color from its name.
---@param name ColorName The name of a predefined color.
---@return Color # The resulting color.
function Color(name) end

---Construct a color from RGBA values.
---@param parts ColorParts Color components.
---@return Color # The resulting color.
function Color(parts) end


---Information necessary to create a shader.
---@class ShaderInfo
---@field entry string Entry point of this shader program.
---@field stage ShaderStage Stage to assign to this shader.
local ShaderInfo

---Shader metatable & constructor.
---@class Shader
Shader = {}

---Construct a shader instance.
---@param filename string Name of a file containing shader source code.
---@param info ShaderInfo Table containing additional information about our shader.
---@return Shader # Newly constructed shader instance.
function Shader(filename, info) end


---Desribes a buffer for a graphics pipeline.
---@class BufferDesc
---@field slot integer Which slot this buffer will be bound to.
---@field pitch integer Number of bytes between elements of this buffer.
---@field rate VertexRate Rate at which buffer elements advance (per vertex or per instance).
local BufferDesc

---Describes the inputs of a vertex buffer.
---@class VertexInputDesc
---@field location integer Index of the shader input.
---@field buffer integer Index of the buffer this descriptor applies to.
---@field format VertexFormat One of many possible vertex formats.
---@field offset integer Byte offset from the start of the current element.
local VertexInputDesc

---Information necessary to create a graphics pipeline.
---@class PipelineInfo
---@field vertex Shader Vertex shader instance to use in our pipeline.
---@field fragment Shader fragment shader instance to use in our pipeline.
---@field primitive PrimitiveType Primitive type used by our pipeline.
---@field buffers BufferDesc[] Table of buffer descriptors.
---@field inputs VertexInputDesc[] Table of vertex input descriptors.
local PipelineInfo

---Graphics pipeline metatable & constructor.
---@class Pipeline
Pipeline = {}

---Bind this graphics pipeline to the given render pass.
---@param pass RenderPass Render pass onto which to bind this pipeline.
function Pipeline:bind(pass) end

---Construct a graphics pipeline instance.
---@param info PipelineInfo Information necessary to create a graphics pipeline.
---@return Pipeline # Newly constructed graphics pipeline instance.
function Pipeline(info) end


---Copy pass on a command buffer for uploading data to the GPU.
---@class CopyPass
local CopyPass


---Render pass on a command buffer for rendering graphics on the GPU.
---@class RenderPass
local RenderPass


---Command buffer for batching various graphical operations together.
---@class CommandBuffer
CommandBuffer = {}

---Begin a [copy pass](lua://CopyPass) on this command buffer.
---@return CopyPass
function CommandBuffer:copypass() end

---Begin a [render pass](lua://RenderPass) on this command buffer.
---@param color (Color|boolean)? The clear color used by this render pass.
---@return RenderPass
function CommandBuffer:renderpass(color) end

---@return CommandBuffer
function CommandBuffer() end


---Name of a predefined color.
---@alias ColorName
---| "black"
---| "white"

---Stage of a shader (one of "vertex" or "fragment").
---@alias ShaderStage
---| "vertex"			# Vertex shader stage.
---| "fragment"			# Fragment shader stage.

---Primitive types which are available to draw with a pipeline.
---@alias PrimitiveType
---| "trianglelist"		# List of triangles.
---| "trianglestrip"	# Strip of triangles.
---| "linelist"			# List of lines.
---| "linestrip"		# Strip of lines.
---| "pointlist"		# List of points.

---Rate at which buffer elements advance in a shader.
---@alias VertexRate
---| "vertex"			# Elements advance on a per-vertex basis.
---| "instance"			# Elements advance on a per-instance basis.

---Possible formats which vertex inputs can have.
---@alias VertexFormat
---| "int" 				#
---| "int2" 			#
---| "int3" 			#
---| "int4" 			#
---| "uint" 			#
---| "uint2" 			#
---| "uint3" 			#
---| "uint4" 			#
---| "float" 			#
---| "float2" 			#
---| "float3" 			#
---| "float4" 			#
---| "byte2" 			#
---| "byte4" 			#
---| "ubyte2" 			#
---| "ubyte4" 			#
---| "byte2norm" 		#
---| "byte4norm" 		#
---| "ubyte2norm" 		#
---| "ubyte4norm" 		#
---| "short2" 			#
---| "short4" 			#
---| "ushort2" 			#
---| "ushort4" 			#
---| "short2norm" 		#
---| "short4norm" 		#
---| "ushort2norm" 		#
---| "ushort4norm" 		#