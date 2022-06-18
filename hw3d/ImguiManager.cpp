#include "ImguiManager.h"
#include "imgui/imgui.h"

ImguiManager::ImguiManager()
{
	IMGUI_CHECKVERSION();		
	ImGui::CreateContext();		//Needed to initialise
	ImGui::StyleColorsDark();	//Darkmode
}

ImguiManager::~ImguiManager()
{
	ImGui::DestroyContext();
}
