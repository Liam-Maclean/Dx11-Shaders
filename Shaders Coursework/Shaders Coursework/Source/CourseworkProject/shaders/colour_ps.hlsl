// Colour pixel/fragment shader
// Basic fragment shader outputting a colour
cbuffer colourBuffer
{
	float4 bufferColour;
};


struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	float4 colour = bufferColour;

	return colour;
}