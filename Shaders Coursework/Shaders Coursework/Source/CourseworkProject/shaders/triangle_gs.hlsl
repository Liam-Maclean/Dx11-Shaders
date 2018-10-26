// Example geometry shader
// Receives a point and outputs a triangle


cbuffer MatrixBuffer : register(cb0)
{
    matrix worldMatrix;
    matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer lightDirectionBuffer : register(cb1)
{
	matrix normalWorld;
	float4 lightColor;
	float3 lightPosition;
	float padding1;
	float3 lightTarget;
	float padding2;
}

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

// pixel input type
struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

// gs function
[maxvertexcount(3)]
void main(point InputType input[1], inout LineStream<OutputType> triStream)
{
	OutputType output;

	float4 f4LightDirection = float4(lightTarget.x, lightTarget.y, lightTarget.z, 1.0f);
	f4LightDirection = mul(-f4LightDirection, worldMatrix);


	float4 target = f4LightDirection - input[0].position;

	target = normalize(target);
	// Change the position vector to be 4 units for proper matrix calculations.

	//f4LightDirection *=  100;

	input[0].position = float4(0.0f,0.0f,0.0f,1.0f);

	//First vertex is on the light
	output.position = input[0].position;
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);

	//Second Vertex along the midpoint between the end of the directional vector and the light.
	input[0].position = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.position = input[0].position + (target/2);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
	output.tex = input[0].tex;
    output.normal = mul(input[0].normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);
	triStream.Append(output);

	//Third Vertex At the end of the directional vector 
	input[0].position = float4(0.0f, 0.0f, 0.0f, 1.0f);
	output.position = input[0].position + (target);
	output.position = mul(output.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = input[0].tex;
	output.normal = mul(input[0].normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);
	triStream.Append(output);

}