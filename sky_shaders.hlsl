TextureCube cube0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;//Note the spelling of this and all instances below
	float3 texcoord : TEXCOORD;
};

cbuffer CB1
{
	matrix WVPMatrix;				 //64 bytes
};

//Vertex Shader
VOut VS(float4 position : POSITION, float3 texcoord : TEXCOORD, float4 color:COLOR)
{
	VOut output;

	output.position = mul(WVPMatrix, position);

	output.color = color;

	output.texcoord = position.xyz;

	return output;
}

//Pixel Shader
float4 PS(float4 position : SV_POSITION, float4 color : COLOR, float3 texcoord : TEXCOORD) : SV_TARGET
{
	return cube0.Sample(sampler0,texcoord);
}

