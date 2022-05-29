cbuffer CBuf
{	
	matrix transform; //Matrix implies 4x4 by default
};

float4 main(float3 pos : Position) : SV_Position
{
	return mul(float4(pos.xyz, 1.0f), transform);
}