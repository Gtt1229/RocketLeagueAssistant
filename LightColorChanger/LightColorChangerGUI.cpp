#include "pch.h"
#include "LightColorChanger.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"


// Plugin Settings Window code here
std::string LightColorChanger::GetPluginName() {
	return "LightColorChanger";
}

void LightColorChanger::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> LightColorChanger
void LightColorChanger::RenderSettings() {
	ImGui::TextUnformatted("LightColorChanger plugin settings");

	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");

	if (!enableCvar) { return; }

	bool enabled = enableCvar.getBoolValue();

	//Enable plugin checkbox

	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Plugin");
	}

	static char haurlsize[128] = "";

	std::string test = "";

	CVarWrapper haCvargui = cvarManager->getCvar("h_a");
	if (!haCvargui) { return; }
	std::string reqUrlex = haCvargui.getStringValue();


	char const* currentUrl = reqUrlex.data();

	if (ImGui::InputText("Home Assistant Web Hook URL Without \"-color\"", &reqUrlex)) {

		haCvargui.setValue(reqUrlex);

	}

	
	//Prep for possibly using tokens in the future for requests over https

	static char hatokensize[128] = "";
	CVarWrapper htokenCvarGui = cvarManager->getCvar("ha_token");
	if (!htokenCvarGui) { return; }
	std::string reqTokenex = htokenCvarGui.getStringValue();


	std::string usertoken = "Home Assistant Token";
	
	
		//if (ImGui::InputText("Home Assistant Token", &reqTokenex)) {

	//	htokenCvarGui.setValue(reqTokenex);

	//}



	//ImGui::Button("Apply Token");
}



// Do ImGui rendering here
void LightColorChanger::Render()
{
	if (!ImGui::Begin(menuTitle_.c_str(), &isWindowOpen_, ImGuiWindowFlags_None))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		return;
	}

	ImGui::End();

	if (!isWindowOpen_)
	{
		cvarManager->executeCommand("togglemenu " + GetMenuName());
	}
}

// Name of the menu that is used to toggle the window.
std::string LightColorChanger::GetMenuName()
{
	return "LightColorChanger";
}

// Title to give the menu
std::string LightColorChanger::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
//void LightColorChanger::SetImGuiContext(uintptr_t ctx)
//{
//	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
//}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool LightColorChanger::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool LightColorChanger::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void LightColorChanger::OnOpen()
{
	isWindowOpen_ = true;
}

// Called when window is closed
void LightColorChanger::OnClose()
{
	isWindowOpen_ = false;
}

