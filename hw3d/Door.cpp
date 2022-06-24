#include "Door.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"

Door::Door( Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist )
	:
	r( rdist( rng ) ),
	droll( ddist( rng ) ),
	dpitch( ddist( rng ) ),
	dyaw( ddist( rng ) ),
	dphi( odist( rng ) ),
	dtheta( odist( rng ) ),
	dchi( odist( rng ) ),
	chi( adist( rng ) ),
	theta( adist( rng ) ),
	phi( adist( rng ) )
{
	namespace dx = DirectX;
	//Is statically initialised?
	if (!IsStaticInitialised())
	{
		//Create indexed triangle list of vertices
		struct Vertex {
			dx::XMFLOAT3 pos;
		};
		const auto doorModel = Cube::Make<Vertex>();

		//Bind vertex buffer
		AddStaticBind( std::make_unique<VertexBuffer>( gfx, doorModel.vertices ) );

		//Bind vertex shader
		auto pVs = std::make_unique<VertexShader>( gfx, L"ColorIndexVS.cso" );
		auto pVsbc = pVs->GetBytecode();
		AddStaticBind( std::move( pVs ) ); //Moves ptr from pVs to the static bind

		//Bind pixel shader
		AddStaticBind( std::make_unique<PixelShader>( gfx, L"ColorIndexPS.cso" ) );

		//Bind index buffer
		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx, doorModel.indices ) );

		//Bind ps constant buff
		struct FaceColour
		{
			struct {
				float r;
				float g;
				float b;
				float a;
			} channels[8];
		};
		const FaceColour cb =
		{
			{
				{ 1.0f,1.0f,1.0f },
				{ 1.0f,0.0f,0.0f },
				{ 0.0f,1.0f,0.0f },
				{ 1.0f,1.0f,0.0f },
				{ 0.0f,0.0f,1.0f },
				{ 1.0f,0.0f,1.0f },
				{ 0.0f,1.0f,1.0f },
				{ 0.0f,0.0f,0.0f },
			}
		};
		AddStaticBind( std::make_unique<PixelConstantBuffer<FaceColour>>( gfx, cb ) );

		//Bind input layout
		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }		
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx, ied, pVsbc ) );

		//Bind Topology
		AddStaticBind( std::make_unique<Topology>( gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	//Set index buffer ptr if not static
	else
	{
		SetIndexFromStatic();
	}

	//Bind per instance transform buffer
	AddBind( std::make_unique<TransformCbuf>( gfx, *this ) );

	//Store per istance model scaling transform
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling( 1.0f, 2.6f, 0.3f )
	);
}

void Door::Update( float dt ) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Door::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3( &mt ) *
		dx::XMMatrixRotationRollPitchYaw( pitch, yaw, roll ) *
		dx::XMMatrixTranslation( r, 0.0f, 0.0f ) *
		dx::XMMatrixRotationRollPitchYaw( theta, phi, chi );
}