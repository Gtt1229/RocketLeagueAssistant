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
	ImGui::TextUnformatted("RocketLeagueAssistant plugin settings");


	//Get URL related Cvars
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");

	CVarWrapper teamsEnableCvar = cvarManager->getCvar("teams_enabled");
	CVarWrapper haHomeCvargui = cvarManager->getCvar("ha_home");
	CVarWrapper haAwayCvargui = cvarManager->getCvar("ha_away");

	CVarWrapper goalScoredEnableCvar = cvarManager->getCvar("goalScored_enabled");
	CVarWrapper haGoalScoredCvargui = cvarManager->getCvar("ha_goalScored");

	CVarWrapper mainmenuEnableCvar = cvarManager->getCvar("mainmenu_enabled");
	CVarWrapper haMainMenuCvargui = cvarManager->getCvar("ha_mainmenu");

	CVarWrapper demosEnableCvar = cvarManager->getCvar("demos_enabled");
	CVarWrapper haDemosCvargui = cvarManager->getCvar("ha_demos");

	CVarWrapper overtimeEnableCvar = cvarManager->getCvar("overtime_enabled");
	CVarWrapper haOvertimeCvargui = cvarManager->getCvar("ha_overtime");

	CVarWrapper freeplayEnableCvar = cvarManager->getCvar("freeplay_enabled");
	CVarWrapper haFreeplayCvargui = cvarManager->getCvar("ha_freeplay");

	CVarWrapper exitEnableCvar = cvarManager->getCvar("exit_enabled");
	CVarWrapper haExitCvargui = cvarManager->getCvar("ha_exit");




	if (!enableCvar) { return; }
	if (!teamsEnableCvar) { return; }
	if (!goalScoredEnableCvar) { return; }
	if (!overtimeEnableCvar) { return; }
	if (!demosEnableCvar) { return; }
	if (!freeplayEnableCvar) { return; }
	if (!mainmenuEnableCvar) { return; }




	bool enabled = enableCvar.getBoolValue();

	//Enable plugin checkbox

	if (ImGui::Checkbox("Enable plugin", &enabled)) {
		enableCvar.setValue(enabled);
	}
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("Toggle Plugin");
	}


	
	//char const* currentUrl = reqhomeUrlex.data();

	if (enabled == true) {

		//Team color hook Gui

		bool teamsEnabled = teamsEnableCvar.getBoolValue();

				
		if (ImGui::Checkbox("Enable Team Colors Webhook", &teamsEnabled)) {
			teamsEnableCvar.setValue(teamsEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}


		if (teamsEnabled == true) {

			if (!haHomeCvargui) { return; }
			std::string reqhomeUrlex = haHomeCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL for Home Team (Blue)", &reqhomeUrlex)) {

				haHomeCvargui.setValue(reqhomeUrlex);

			}


			if (!haAwayCvargui) { return; }
			std::string reqawayUrlex = haAwayCvargui.getStringValue();


			//char const* currentUrl = reqawayUrlex.data();

			if (ImGui::InputText("Home Assistant Web Hook URL For Away Team (Orange)", &reqawayUrlex)) {

				haAwayCvargui.setValue(reqawayUrlex);

			}
		}
		 

		//Goal Scored hook Gui

		bool goalScoredEnabled = goalScoredEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable goalScored Webhook", &goalScoredEnabled)) {
			goalScoredEnableCvar.setValue(goalScoredEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		if (goalScoredEnabled == true) {

			if (!haGoalScoredCvargui) { return; }
			std::string reqgoalScoredUrlex = haGoalScoredCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL For Goals", &reqgoalScoredUrlex)) {

				haGoalScoredCvargui.setValue(reqgoalScoredUrlex);

			}
		}

		//Demos hook Gui

		bool demosEnabled = demosEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Demos Webhook", &demosEnabled)) {
			demosEnableCvar.setValue(demosEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		if (demosEnabled == true) {

			if (!haDemosCvargui) { return; }
			std::string reqdemosUrlex = haDemosCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL For Demos", &reqdemosUrlex)) {

				haDemosCvargui.setValue(reqdemosUrlex);

			}
		}


		//Overtime hook GUI

		bool overtimeEnabled = overtimeEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Overtime Webhook", &overtimeEnabled)) {
			overtimeEnableCvar.setValue(overtimeEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		if (overtimeEnabled == true) {

			if (!haOvertimeCvargui) { return; }
			std::string reqovertimeUrlex = haOvertimeCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL For Overtime", &reqovertimeUrlex)) {

				haOvertimeCvargui.setValue(reqovertimeUrlex);

			}
		}

		//Freeplay hook Gui

		bool freeplayEnabled = freeplayEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Freeplay Webhook", &freeplayEnabled)) {
			freeplayEnableCvar.setValue(freeplayEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		if (freeplayEnabled == true) {

			if (!haFreeplayCvargui) { return; }
			std::string reqFreeplayUrlex = haFreeplayCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL For Freeplay", &reqFreeplayUrlex)) {

				haFreeplayCvargui.setValue(reqFreeplayUrlex);

			}
		}

		//Main Menu hook Gui

		bool mainmenuEnabled = mainmenuEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Main Menu Webhook", &mainmenuEnabled)) {
			mainmenuEnableCvar.setValue(mainmenuEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		if (mainmenuEnabled == true) {

			if (!haMainMenuCvargui) { return; }
			std::string reqMainMenuUrlex = haMainMenuCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL For Main Menu", &reqMainMenuUrlex)) {

				haMainMenuCvargui.setValue(reqMainMenuUrlex);

			}
		}

		//Exit hook Gui

		bool exitEnabled = exitEnableCvar.getBoolValue();

		if (ImGui::Checkbox("Enable Game Exit Webhook", &exitEnabled)) {
			exitEnableCvar.setValue(exitEnabled);
		}
		if (ImGui::IsItemHovered()) {
			ImGui::SetTooltip("Toggle Webhook");
		}

		if (exitEnabled == true) {

			if (!haExitCvargui) { return; }
			std::string reqExitUrlex = haExitCvargui.getStringValue();

			if (ImGui::InputText("Home Assistant Web Hook URL For Game Exit", &reqExitUrlex)) {

				haExitCvargui.setValue(reqExitUrlex);

			}
		}
	
	
	}


	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.2f, 1.0f), "Made by GTT1229"); ImGui::SameLine(); ImGui::TextColored(ImVec4(0.2f, 0.6f, 1.0f, 1.0f), "Sponsored by Skordy");


	//Prep for possibly using tokens in the future for requests over https

	//static char hatokensize[128] = "";
	//CVarWrapper htokenCvarGui = cvarManager->getCvar("ha_token");
	//if (!htokenCvarGui) { return; }
	//std::string reqTokenex = htokenCvarGui.getStringValue();


	//std::string usertoken = "Home Assistant Token";
	
	
		//if (ImGui::InputText("Home Assistant Token", &reqTokenex)) {

	//	htokenCvarGui.setValue(reqTokenex);

	//}



	//ImGui::Button("Apply Token");
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

