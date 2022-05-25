struct VSOUT
{
	float3 colour : Colour; //colour first as PS will try to match input params by order in struct, since it only takes f3 colour this needs to be the first in struct
	float4 pos : SV_Position;
};
VSOUT main(float2 pos : Position, float3 col : Colour)
{
	VSOUT o;
	o.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	o.colour = col;
	return o;
}