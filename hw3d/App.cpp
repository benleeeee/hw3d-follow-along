#include "App.h"
#include  "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "SkinnedBox.h"
#include "Sheet.h"
#include "Door.h"
//#include <iomanip>
#include <memory>
#include <algorithm>
#include "ChiliMath.h"
#include "Surface.h"
#include "GDIPlusManager.h"
#include "imgui/imgui.h"

namespace dx = DirectX;

GDIPlusManager gdipm; //Declare an instance so it Initialises GDIPlusManager

App::App()
	:
	wnd( 800, 600, "My Window" ) //Initialise the window w/ the app 
{
	class Factory
	{
	public:
		Factory( Graphics& gfx )
			:
			gfx( gfx )
		{}
		//() constructor operator overload - picks a random shape
		std::unique_ptr<Drawable> operator()()
		{
			switch (typedist( rng ))
			{
			case 0:
				return std::make_unique<Door>(
					gfx, rng, adist, ddist, odist, rdist, bdist );
				/*return std::make_unique<Pyramid>(
					gfx, rng, adist, ddist, odist, rdist
				);*/
			case 1:
				return std::make_unique<Door>(
					gfx, rng, adist, ddist, odist, rdist, bdist );
				/*return std::make_unique<Box>(
					gfx, rng, adist, ddist, odist, rdist, bdist
				);*/
			case 2:
				return std::make_unique<Door>(
					gfx, rng, adist, ddist, odist, rdist, bdist );
				/*return std::make_unique<Melon>(
					gfx, rng, adist, ddist, odist, rdist, longdist, latdist
				);*/
			case 3:
				return std::make_unique<Sheet>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			case 4:
				return std::make_unique<SkinnedBox>(
					gfx, rng, adist, ddist,
					odist, rdist
					);
			default:
				assert( false && "bad drawable type in factory" );
				return {};
			}
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,3.0f };
		std::uniform_int_distribution<int> latdist{ 5,20 };
		std::uniform_int_distribution<int> longdist{ 10,40 };
		std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	//Create factory
	Factory f( wnd.Gfx() );
	//Reserve space for max amount of drawables in scene
	drawables.reserve( nDrawables );
	//Call factory to generate max amount and fill drawables vector
	std::generate_n( std::back_inserter( drawables ), nDrawables, f );

	//Set camera projection
	wnd.Gfx().SetProjection( dx::XMMatrixPerspectiveLH( 1.0f, 3.0f / 4.0f, 0.5f, 40.0f ) );
	wnd.Gfx().SetCamera( dx::XMMatrixTranslation( 0.0f, 0.0f, -20.0f ) );
}

void App::DoFrame()
{
	//Tick
	const auto dt = timer.Mark() * sim_speed;

	wnd.Gfx().BeginFrame( 0.07f, 0.0f, 0.12f );
	wnd.Gfx().SetCamera( cam.GetMatrix() );

	//Draw drawables	
	for (auto& b : drawables)
	{
		b->Update( wnd.kbd.KeyIsPressed( VK_SPACE ) ? 0.0f : dt ); //Update while space is held
		b->Draw( wnd.Gfx() );
	}

	//Draw ImGui (draw data set up in graphics.cpp)
	static char buffer[1024];
	if (ImGui::Begin( "Simulation Speed" ))
	{
		ImGui::SliderFloat( "Speed Factor", &sim_speed, 0.0f, 4.0f );
		ImGui::Text( "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate );
		ImGui::Text( "Status: %s", wnd.kbd.KeyIsPressed( VK_SPACE ) ? "PAUSED" : "RUNNING (hold spacebar to pause)" );
	}
	ImGui::End();
	//ImGui window to control camera
	cam.SpawnControlWindow();



	//Present screen
	wnd.Gfx().EndFrame(); //MUST call EndFrame to present backbuffer
}

App::~App()
{}

int App::Go()
{
	//As an app that continually runs it is no longer an event based program like a normal window
	while (true)
	{
		//Process all messages pending, but to not block for new messages
		if (const auto ecode = Window::ProcessMessages())
		{
			//if return optional has value, means we're quitting so return exit code
			return *ecode;
		}
		DoFrame();
	}
}