// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//light information passed in
cbuffer LightBuffer : register(cb0)
{
    float4 diffuseColor;
	float4 ambientColor;
    float3 lightTarget;
	float lightRange;
	float3 lightPosition;
	float coneAngle;
	float3 padding;
	float lightAttenuation;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
	float3 position3D : TEXCOORD1;
};

//main pixel shader
float4 main(InputType input) : SV_TARGET
{
	//Light intensity for diffuse
	float lightIntensity;

	//Light attenuation
	float attenuation = lightAttenuation;

	//inverted light position (for correct directions for light)
	float3 invrLightPos = -lightPosition;
	
	//Find out the vector from the light position, to the 3D point on the mesh.
	float3 toLight = normalize(invrLightPos - input.position3D);

	//figure out the length between the light position and the 3D point on the mesh
	float distanceToLight = length(invrLightPos - input.position3D);

	//diffuse for later
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 1.0f);

	//store the direction from the spot lights target, to the light
	float3 lightDir = normalize(lightTarget - invrLightPos);

	//Height attenuation (further away, less color)
	float heightAttenuation = smoothstep(lightRange, 0.0f, distanceToLight);

	//cut off angle of the radius of the light
	float cutoffAngle = radians(30.0f);

	//calculate the light intensity for diffuse
	lightIntensity = saturate(dot(input.normal, lightDir));

	

	//if there is light on the object
	if (lightIntensity > 0)
	{
		//find out diffuse color using light color and intensity
		diffuse += (diffuseColor * lightIntensity);
		diffuse = saturate(diffuse);
	}
	
	//find out if the lights to be lit within the cone
	float angle = acos(dot(-toLight, lightDir));
	if (angle > cutoffAngle)
	{
		//if not cut attenuation to 0 
		attenuation = 0.0f;
	}

	//return color
	return ambientColor + attenuation*(diffuse)*heightAttenuation ;
}

