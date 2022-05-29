cbuffer CBuf
{
	float4 face_colours[6];
};
//Tells system to assign an id to each unique primitive (triangle) 
float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	//There are 2 triangles per face on the cube, so need to /2 to get the correct lookup colour
	//e.g. there are 6 colours for 6 faces, but 12 triangles, 2 per face
	return face_colours[tid / 2];
}