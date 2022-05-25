float4 main(float3 colour : Colour) : SV_Target
{
	return float4(colour.rgb, 1.0f);
}