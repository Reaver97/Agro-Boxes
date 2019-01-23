cbuffer CB0
{
	matrix WVPMatrix; //64 B
	float4 directional_light_vector; //16 bytes
	float4 directional_light_colour; //16 bytes
	float4 ambient_light_colour;	 //16 bytes
	float4 point_light_position;	 //16 bytes
	float4 point_light_colour;		 //16 bytes
};

Texture2D texture0;
SamplerState sampler0;

struct VOut
{
	float4 position : SV_POSITION;
	float4 color	: COLOR;
	float2 texcoord : TEXCOORD;
};

//Vertex Shader with input parameters
VOut ModelVS(float4 position: POSITION, float2 texcoord : TEXCOORD, float3 normal : NORMAL)
{
	//Vertex output
	VOut output;
	
	output.position			= mul(WVPMatrix, position);

	float diffuse_amount = dot((float3)directional_light_vector.xyz, (float3)normal.xyz);
	diffuse_amount = saturate(diffuse_amount);

	float4 lightvector = point_light_position - position;
	float point_amount = dot(normalize(lightvector.xyz), normal);
	point_amount = saturate(point_amount);

	output.color = ambient_light_colour + (point_light_colour * point_amount);

	output.texcoord	= texcoord;

	
	return output;
}

float4 ModelPS(float4 position: SV_POSITION, float4 color: COLOR, float2 texcoord : TEXCOORD) : SV_TARGET
{
	return texture0.Sample(sampler0,texcoord) * color;
}