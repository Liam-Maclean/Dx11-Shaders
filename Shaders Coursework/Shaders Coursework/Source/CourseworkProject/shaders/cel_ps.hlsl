// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//information from lights
cbuffer lightBuffer : register (cb0)
{
	float4 diffuseColor;	//light diffuse
	float3 direction;		//light direction
	float padding;			//padding
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(InputType input) : SV_TARGET
{
	//calculate color with 
	float4 color;
		
	//Take the color from the texture and combine with diffuse color
	color = shaderTexture.Sample(SampleType, input.tex) * diffuseColor;

	//make sure alpha is 1 and visible
	color.a = 1;
	
	//calculate light intensity
	float intensity = dot(normalize(direction), input.normal);

	if (intensity < 0)
	{
		intensity = 0;
	}

	//Do the light based on light drop off points
	if (intensity > 0.95)
	{
		color = float4(1.0, 1, 1, 1.0) * color;
	}
	//70% of color
	else if (intensity > 0.5)
	{
		color = float4(0.7, 0.7, 0.7, 1.0) * color;
	}
	//35% of color
	else if (intensity > 0.05)
	{
		color = float4(0.35, 0.35, 0.35, 1.0) * color;
	}
	//30% of color
	else
	{
		color = float4(0.2, 0.2, 0.2, 1.0) * color;
	}
	
	return color;
}
