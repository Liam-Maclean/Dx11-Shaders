// Colour pixel/fragment shader
// Basic fragment shader outputting a colour

#define NUM_SAMPLES 64
texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);


cbuffer LightBuffer : register(cb0)
{
	float4 diffuseColor;
	float2 ScreenLightPos;
	float decay;
	float exposure;
	float3 lightDirection;
	float weight;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(float2 texCoord : TEXCOORD0) : SV_TARGET
{
    half Density = 1.0f;
	
	// Calculate vector from pixel to light source in screen space.
	half2 deltaTexCoord = (texCoord.xy - ScreenLightPos.xy);

	// Divide by number of samples and scale by control factor.
	deltaTexCoord *= 1.0f / NUM_SAMPLES * Density;

	// Store initial sample.
	half3 color = shaderTexture.Sample(SampleType, texCoord);

	// Set up illumination decay factor.
	half illuminationDecay = 1.0f;


	// Evaluate summation from Equation 3 NUM_SAMPLES iterations.
	for (int i = 0; i < 100; i++)
	{
		// Step sample location along ray.
		texCoord -= deltaTexCoord;

		// Retrieve sample at new location.
		half3 sample = shaderTexture.Sample(SampleType, texCoord);

		// Apply sample attenuation scale/decay factors.
		sample *= illuminationDecay * weight;

		// Accumulate combined color.
		color += sample;

		// Update exponential decay factor.
		illuminationDecay *= decay;
	}
	// Output final color with a further scale control factor.
	//return float4(color * exposure, 1);

	return float4 (color * exposure, 1);
}
