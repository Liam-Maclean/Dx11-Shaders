Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer weightBuffer : register(cb0)
{
	float weight0;
	float weight1;
	float weight2;
	float padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 texCoord1 : TEXCOORD1;
	float2 texCoord2 : TEXCOORD2;
	float2 texCoord3 : TEXCOORD3;
	float2 texCoord4 : TEXCOORD4;
	float2 texCoord5 : TEXCOORD5;
};


float4 main(InputType input) : SV_TARGET
{
	float weight0, weight1, weight2;
	float4 colour;

	weight0 = 0.4062f;
	weight1 = 0.2442f;
	weight2 = 0.0545f;

	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	colour += shaderTexture.Sample(SampleType, input.texCoord1) * weight2;
	colour += shaderTexture.Sample(SampleType, input.texCoord2) * weight1;
	colour += shaderTexture.Sample(SampleType, input.texCoord3) * weight0;
	colour += shaderTexture.Sample(SampleType, input.texCoord4) * weight1;
	colour += shaderTexture.Sample(SampleType, input.texCoord5) * weight2;

	colour.a = 1.0f;

	return colour;
}