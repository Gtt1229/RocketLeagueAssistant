#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"


#include "version.h"
#include "pch.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);


class RocketLeagueAssistant: public BakkesMod::Plugin::BakkesModPlugin, public BakkesMod::Plugin::PluginSettingsWindow, public BakkesMod::Plugin::PluginWindow
{

	//std::shared_ptr<bool> enabled;
public:
	//Boilerplate
	virtual void onLoad();
	virtual void onUnload();

	//Custom methods
	void LoadHooks();
	void LoadTeams(std::string name);
	std::string ConvertLinearColor(float red, float green, float blue);
	void StatsHook(void* params);
	//void ChatHook(void* params);
	void FreeplayHook();
	void MainMenuHook(std::string name);
	void MatchCountdownHook(std::string name);
	void OvertimeHook(std::string name);
	//void GoalScoredHook(std::string name);
	void ExitHook(std::string name);
	void Replay(std::string name);
	void NotReplay(std::string name);
	void SendCommands(std::string event);
	std::string GenWebHook();
	std::string GenAutomationID();
	void CreateAutomation(bool version7bool);
	void UpdateModal();
	void modalClosed(std::string name);
	int GetScore(int teamNum);
	void GetHAVersion();
	//void OnInput(const std::string& input, bool was_closed);


	//GUI
	void RenderSettings() override;
	std::string GetPluginName() override;
	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "RocketLeagueAssistant";
	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;

private:

	//Logging method
	void Log(std::string msg);
	

	// Inherited via PluginSettingsWindow
	/*
	void RenderSettings() override;
	std::string GetPluginName() override;
	void SetImGuiContext(uintptr_t ctx) override;
	*/

	// Inherited via PluginWindow
	/*

	bool isWindowOpen_ = false;
	bool isMinimized_ = false;
	std::string menuTitle_ = "RocketLeagueAssistant";

	virtual void Render() override;
	virtual std::string GetMenuName() override;
	virtual std::string GetMenuTitle() override;
	virtual void SetImGuiContext(uintptr_t ctx) override;
	virtual bool ShouldBlockInput() override;
	virtual bool IsActiveOverlay() override;
	virtual void OnOpen() override;
	virtual void OnClose() override;
	
	*/


};


