// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LightBuffer : register(cb0)
{
    float4 diffuseColor[4];
	float4 ambientColor[4];
    float4 lightDirection[4];
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD1;
};


float4 main(InputType input) : SV_TARGET
{
    float4 textureColor;
    float3 lightDir[4];
    float lightIntensity[4];
    float4 color;
	float spec;
	float spotEffect;
	float4 finalSpec;
	float distance;


	float3 halfway;
	float4 diffuseComponent;

	color = ambientColor[0];

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(SampleType, input.tex);
	
	// Calculate the amount of light on this pixel.
	for (int i = 0; i < 4; i++)
	{
		lightDir[i] = -lightDirection[i];
		lightIntensity[i] = saturate(dot(input.normal, lightDir[i]));
		if (lightIntensity[i] > 0)
		{
			diffuseComponent += (diffuseColor[i] * lightIntensity[i]);
			// Determine the final amount of diffuse color based on the diffuse color combined with the light intensity.
			//color = saturate(color);
		}
	}
   	
	color += diffuseComponent;
	color = saturate(color);
   	
	// Multiply the texture pixel and the final diffuse color to get the final pixel color result.
	color = color * textureColor;
   	
	return color;
}

