#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"



// Plugin Settings Window code here
std::string RocketLeagueAssistant::GetPluginName() {
	return "RocketLeagueAssistant";
}

void RocketLeagueAssistant::SetImGuiContext(uintptr_t ctx) {
	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
}

// Render the plugin settings here
// This will show up in bakkesmod when the plugin is loaded at
//  f2 -> plugins -> RocketLeagueAssistant
void RocketLeagueAssistant::RenderSettings() {

	//Get URL related Cvars
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");


	CVarWrapper teamsEnableCvar = cvarManager->getCvar("teams_enabled");

	CVarWrapper goalScoredEnableCvar = cvarManager->getCvar("goalScored_enabled");

	CVarWrapper mainmenuEnableCvar = cvarManager->getCvar("mainmenu_enabled");

	CVarWrapper demosEnableCvar = cvarManager->getCvar("demos_enabled");

	CVarWrapper overtimeEnableCvar = cvarManager->getCvar("overtime_enabled");

	CVarWrapper freeplayEnableCvar = cvarManager->getCvar("freeplay_enabled");

	CVarWrapper exitEnableCvar = cvarManager->getCvar("exit_enabled");

	CVarWrapper haJsonURLCvargui = cvarManager->getCvar("ha_jsonURL");

	CVarWrapper haHaBaseURLCvargui = cvarManager->getCvar("ha_haBaseURL");
	CVarWrapper hideURLCvar = cvarManager->getCvar("hideURL");
	
	CVarWrapper version7Cvar = cvarManager->getCvar("version7");
	if (!version7Cvar) { return; }

	bool version7 = version7Cvar.getBoolValue();

	if (!enableCvar) { return; }
	if (!teamsEnableCvar) { return; }
	if (!goalScoredEnableCvar) { return; }
	if (!overtimeEnableCvar) { return; }
	if (!demosEnableCvar) { return; }
	if (!freeplayEnableCvar) { return; }
	if (!mainmenuEnableCvar) { return; }




	bool enabled = enableCvar.getBoolValue();
	bool hideURL = hideURLCvar.getBoolValue();
	//Enable plugin checkbox

	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Plugin");
	}





	//char const* currentUrl = reqhomeUrlex.data();

	if (enabled == true) {

			
			if (!haJsonURLCvargui) { return; }
			std::string jsonUrlex = haJsonURLCvargui.getStringValue();
			ImGui::PushItemWidth(33.0f * ImGui::GetFontSize());
			
			if (hideURL == false) {
				if (ImGui::InputText("Home Assistant Web Hook URL", &jsonUrlex)) {

					haJsonURLCvargui.setValue(jsonUrlex);

				}
			}
				
		ImGui::Spacing();
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::PushItemWidth(33.0f * ImGui::GetFontSize());
		//Team color hook Gui
		bool teamsEnabled = teamsEnableCvar.getBoolValue();


		if (ImGui::Checkbox("Enable Team Colors Webhook", &teamsEnabled)) {
			teamsEnableCvar.setValue(teamsEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		
		//Goal Scored hook Gui

		bool goalScoredEnabled = goalScoredEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Goal Scored Webhook", &goalScoredEnabled)) {
			goalScoredEnableCvar.setValue(goalScoredEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}
		
		//Demos hook Gui

		bool demosEnabled = demosEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Demos Webhook", &demosEnabled)) {
			demosEnableCvar.setValue(demosEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		

		//Overtime hook GUI

		bool overtimeEnabled = overtimeEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Overtime Webhook", &overtimeEnabled)) {
			overtimeEnableCvar.setValue(overtimeEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

	
		//Freeplay hook Gui

		bool freeplayEnabled = freeplayEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Freeplay Webhook", &freeplayEnabled)) {
			freeplayEnableCvar.setValue(freeplayEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

	

		//Main Menu hook Gui

		bool mainmenuEnabled = mainmenuEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Main Menu Webhook", &mainmenuEnabled)) {
			mainmenuEnableCvar.setValue(mainmenuEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

	
		//Exit hook Gui

		bool exitEnabled = exitEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Game Exit Webhook", &exitEnabled)) {
			exitEnableCvar.setValue(exitEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}
			


	}


	ImGui::Spacing();
	
	if (ImGui::CollapsingHeader("Automation Template Generator")) {

		//Prep for possibly using tokens in the future for requests over https


		
		ImGui::PopItemWidth();
		if (!haHaBaseURLCvargui) { return; }
		std::string haBaseUrlex = haHaBaseURLCvargui.getStringValue();
		ImGui::PushItemWidth(15.0f * ImGui::GetFontSize());
		if (ImGui::InputText("Home Assistant Address", &haBaseUrlex)) {

			haHaBaseURLCvargui.setValue(haBaseUrlex);

		}
		ImGui::PopItemWidth();
		ImGui::PopItemWidth();
		std::string reqTokenex = "Long Lived Access Token";
		CVarWrapper htokenCvarGui = cvarManager->getCvar("ha_token");
		if (!htokenCvarGui) { return; }
		reqTokenex = htokenCvarGui.getStringValue();


		std::string usertoken = "Home Assistant Token";

		
		if (ImGui::InputText("Home Assistant Token", &reqTokenex)) {

			htokenCvarGui.setValue(reqTokenex);

		}



		//ImGui::Button("Apply Token");

		if (ImGui::Button("Generate Automation Base Template")) {

			RocketLeagueAssistant::GetHAVersion();

		}


		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), "IT IS RECOMMENDED TO DELETE THE TOKEN IN HOME ASSISTANT AFTER THE AUTOMATION HAS BEEN GENERATED");
		ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), "THE TOKEN IS DELETED FROM BAKKES WHEN THE GAME IS CLOSED");
	}
	ImGui::Spacing();

	ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);

	ImGui::Spacing();

	
	ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), "Made by GTT1229"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Sponsored by Skordy");

	if (ImGui::TreeNode("Extras")) {

		if (ImGui::Checkbox("Hide JSON URL", &hideURL)) {
			hideURLCvar.setValue(hideURL);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Hide JSON URL");
		}

	ImGui::TreePop();

	}

	
	ImGui::Text("Version:"); ImGui::SameLine(); ImGui::Text(plugin_version);
}




// Do ImGui rendering here
void RocketLeagueAssistant::Render()
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
std::string RocketLeagueAssistant::GetMenuName()
{
	return "RocketLeagueAssistant";
}

// Title to give the menu
std::string RocketLeagueAssistant::GetMenuTitle()
{
	return menuTitle_;
}

// Don't call this yourself, BM will call this function with a pointer to the current ImGui context
//void RocketLeagueAssistant::SetImGuiContext(uintptr_t ctx)
//{
//	ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(ctx));
//}

// Should events such as mouse clicks/key inputs be blocked so they won't reach the game
bool RocketLeagueAssistant::ShouldBlockInput()
{
	return ImGui::GetIO().WantCaptureMouse || ImGui::GetIO().WantCaptureKeyboard;
}

// Return true if window should be interactive
bool RocketLeagueAssistant::IsActiveOverlay()
{
	return true;
}

// Called when window is opened
void RocketLeagueAssistant::OnOpen()
{
	isWindowOpen_ = true;

}

// Called when window is closed
void RocketLeagueAssistant::OnClose()
{
	isWindowOpen_ = false;

}

