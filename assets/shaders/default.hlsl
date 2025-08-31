struct vInput
{
	float3 position : SV_Position;
	float2 texcoord : TEXCOORD0;
};

struct vOutput
{
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
};

struct fInput
{
	float4 position : SV_Position;
	float2 texcoord : TEXCOORD0;
};

vOutput vMain(vInput input)
{
	vOutput output;
	output.position = float4(input.position, 1);
	output.texcoord = input.texcoord;
	return output;
}

float4 fMain(fInput input) : SV_Target
{
	return float4(1, 1, 1, 1);
}