#include "pch.h"
#include "LightColorChanger.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"




BAKKESMOD_PLUGIN(LightColorChanger, "Rocket League/HomeAsisstant Integration", plugin_version, PLUGINTYPE_BOTAI)



std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::string haurlen = "None";

void LightColorChanger::onLoad()
{

	//Enable Cvars
	cvarManager->registerCvar("ha_enabled", "1", "Enable Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("teams_enabled", "1", "Enable Team Colors", true, true, 0, true, 1);
	cvarManager->registerCvar("demos_enabled", "1", "Enable Demos Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("freeplay_enabled", "1", "Enable Freeplay Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("mainmenu_enabled", "1", "Enable Mainemenu Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("exit_enabled", "1", "Enable Mainemenu Webhook", true, true, 0, true, 1);

	//URL Cvars
	cvarManager->registerCvar("ha_home", "http://192.168.1.256:8123/api/webhook/webhook-light-example-home");
	cvarManager->registerCvar("ha_away", "http://192.168.1.256:8123/api/webhook/webhook-light-example-away");
	cvarManager->registerCvar("ha_demos", "http://192.168.1.256:8123/api/webhook/webhook-light-example-demos");
	cvarManager->registerCvar("ha_freeplay", "http://192.168.1.256:8123/api/webhook/webhook-light-example-freeplay");
	cvarManager->registerCvar("ha_mainmenu", "http://192.168.1.256:8123/api/webhook/webhook-light-example-mainmenu");
	cvarManager->registerCvar("ha_exit", "http://192.168.1.256:8123/api/webhook/webhook-light-example-exit");

	//Prep for possibly using tokens in the future for requests over https
	cvarManager->registerCvar("ha_token", "Generated Token", "Home Assistant URL");

	//Log plugin started
	this->Log("LightColorChanger has started");
	
	//call LoadHooks method
	this->LoadHooks();

	_globalCvarManager = cvarManager;
	

}

void LightColorChanger::onUnload()
{

	LOG("Force Closed Game Test");

}


void LightColorChanger::LoadHooks()
{
	//Load GameHooks
	gameWrapper->HookEvent("Function Engine.Pawn.GetTeam", std::bind(&LightColorChanger::LoadTeams, this, std::placeholders::_1));


	//Demo Feature
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ActorWrapper caller, void* params, std::string eventname) {
			DemosHook(params);
		});

	//Main Menu
	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&LightColorChanger::MainMenuHook, this, std::placeholders::_1));

	//On Game Exit
	gameWrapper->HookEvent("Function ProjectX.GFxShell_X.ExitGame", std::bind(&LightColorChanger::ExitHook, this, std::placeholders::_1));
}

void LightColorChanger::SendCommands(std::string reqUrl)
{

	CurlRequest req;
	req.url = reqUrl;
	req.body = R"T({
				"Authorization": "Bearer " ,
				"content-type": "application/json",
				})T";

	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
		});
}

void LightColorChanger::LoadTeams(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();
	
	if (!enabled) { LOG("LightColorChanger is not enabled"); return; }
	
	//Get player team and primary color

	if (gameWrapper->IsInFreeplay()) {

		CVarWrapper freeplayEnabledCvar = cvarManager->getCvar("freeplay_enabled");
		bool freeplayEnabled = freeplayEnabledCvar.getBoolValue();

		if (freeplayEnabled == true) {
			LOG("Player in freeplay, using freeplay color's");
			FreeplayHook();
			return;
		}
	
	}

	if (!gameWrapper->IsInFreeplay()) {

		cvarManager->log("Player in game, using team colors");

		ServerWrapper server = gameWrapper->GetCurrentGameState();
		if (!server) { LOG("Server nullcheck failed");   return; }

		auto primary = server.GetLocalPrimaryPlayer();
		if (!primary) { LOG("Server primary nullcheck failed");   return; }

		auto teams = server.GetTeams();
		if (teams.Count() < 2) { return; }

		auto pris = primary.GetPRI();
		if (!pris) { LOG("Server pri's nullcheck failed");   return; }

		//Get player's team number - 0 = Blue, 1 = Orange
		int teamnum = pris.GetTeamNum();
		if (teamnum > 1) { LOG("teamNum check failed");   return; }

		TeamWrapper myTeam = teams.Get(teamnum);

		if (teamnum > 1) { LOG("teamNum check failed");   return; }


		//Get player's team's linear color
		//LinearColor primaryColor = myTeam.GetPrimaryColor();

		//Prep for custom color values to Home Assistant
		//ConvertLinearColor(primaryColor.R, primaryColor.G, primaryColor.B);

		//get ha url cvars	
		CVarWrapper haHomeCVar = cvarManager->getCvar("ha_home");
		if (!haHomeCVar) { return; }
		CVarWrapper haAwayCVar = cvarManager->getCvar("ha_away");
		if (!haAwayCVar) { return; }
		//

		//this may be redundant?
		auto reqUrlHome = cvarManager->getCvar("ha_home");
		auto reqUrlAway = cvarManager->getCvar("ha_away");
		//

		//convert haurlcvars to string
		std::string reqUrlHomeString = reqUrlHome.getStringValue();
		std::string reqUrlAwayString = reqUrlAway.getStringValue();
		//

		//Send based on home or away team
		if (teamnum <= 1) {

			if (teamnum == 0) {

				std::string reqUrlTeam = reqUrlHomeString;
				SendCommands(reqUrlTeam);
				LOG("Using Home Team Colors");
			}   

			if (teamnum == 1) {

				std::string reqUrlTeam = reqUrlAwayString;
				SendCommands(reqUrlTeam);
				LOG("Using Away Team Colors");
			}

		}
	}
}
		
void LightColorChanger::ConvertLinearColor(float red, float green, float blue) // NOT IN USE
{
	//
	// NOT IN USE
	// 
	//convert linear color 0.1-1 to RGB 255
	
	//convert primaryColor.R - (red)
	auto var_tempR = red * 255;
	auto var_R = (int)var_tempR;
	std::string R = std::to_string(var_R);

	//convert primaryColor.G - (green)
	auto var_tempG = green * 255;
	auto var_G = (int)var_tempG;
	std::string G = std::to_string(var_G);

	//convert primaryColor.B - (blue)
	auto var_tempB = blue * 255;
	auto var_B = (int)var_tempB;
	std::string B = std::to_string(var_B);

	//cvarManager->log(R + ", " + G + ", " + B);

}


void LightColorChanger::DemosHook(void* params)
{
	
	//See if demos are enabled
	CVarWrapper demosEnabledCvar = cvarManager->getCvar("demos_enabled");
	bool demosEnabled = demosEnabledCvar.getBoolValue();
	if (!demosEnabled) { LOG("Demos Automations are not enabled"); return; }
	
	//Get demos automation url, transform, and convert to string
	CVarWrapper haDemosCVar = cvarManager->getCvar("ha_demos");
	if (!haDemosCVar) { return; }
	auto reqUrlDemos = cvarManager->getCvar("ha_demos");
	std::string reqUrlDemosString = reqUrlDemos.getStringValue();

	//Get a demo's info
	struct StatTickerParams {
		uintptr_t Receiver;
		uintptr_t Victim;
		uintptr_t StatEvent;
	};

	StatTickerParams* pStruct = (StatTickerParams*)params;
	PriWrapper receiver = PriWrapper(pStruct->Receiver);
	PriWrapper victim = PriWrapper(pStruct->Victim);

	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);

	//


	if (statEvent.GetEventName() == "Demolish") {
		
		CVarWrapper demosEnabledCvar = cvarManager->getCvar("demos_enabled");
		bool demosEnabled = demosEnabledCvar.getBoolValue();
		if (!demosEnabled) { LOG("Demos Automations are not enabled"); return; }

		if (!receiver) { LOG("Null reciever PRI"); return; }
		if (!victim) { LOG("Null victim PRI"); return; }

		// Find the primary player's PRI
		PlayerControllerWrapper playerController = gameWrapper->GetPlayerController();
		if (!playerController) { LOG("Null controller"); return; }
		PriWrapper playerPRI = playerController.GetPRI();
		if (!playerPRI) { LOG("Null player PRI"); return; }

		// Compare the primary player to the victim
		if (playerPRI.memory_address != victim.memory_address) {
			LOG("Hah you got demoed get good {}", victim.GetPlayerName().ToString());
			

			SendCommands(reqUrlDemosString);


		}
	}

}

void LightColorChanger::FreeplayHook()
{

	//May be redundant, but good to check

	if (gameWrapper->IsInFreeplay()) {

		//Get freeplay automation url, transform, and convert to string
		CVarWrapper haFreeplayCVar = cvarManager->getCvar("ha_freeplay");
		if (!haFreeplayCVar) { return; }
		auto reqUrlFreeplay = cvarManager->getCvar("ha_freeplay");
		std::string reqUrlFreeplayString = reqUrlFreeplay.getStringValue();

		SendCommands(reqUrlFreeplayString);

	}
}

void LightColorChanger::MainMenuHook(std::string name)
{

		//See if main menu hook is enabled
	CVarWrapper mainmenuEnabledCvar = cvarManager->getCvar("mainmenu_enabled");
	bool mainmenuEnabled = mainmenuEnabledCvar.getBoolValue();
	if (!mainmenuEnabled) { LOG("Main Menu Automations are not enabled"); return; }

	//Get mainmenu automation url, transform, and convert to string
	CVarWrapper haMainMenuCVar = cvarManager->getCvar("ha_mainmenu");
	if (!haMainMenuCVar) { return; }
	auto reqUrlmainmenu = cvarManager->getCvar("ha_mainmenu");
	std::string reqUrlMainMenuString = reqUrlmainmenu.getStringValue();

	LOG("Using Main Menu Hook");
	SendCommands(reqUrlMainMenuString);

}

void LightColorChanger::ExitHook(std::string name)
{

	{

		//See if exit hook is enabled
		CVarWrapper exitEnabledCvar = cvarManager->getCvar("exit_enabled");
		bool exitEnabled = exitEnabledCvar.getBoolValue();
		if (!exitEnabled) { LOG("Exit Automations are not enabled"); return; }

		//Get mainmenu automation url, transform, and convert to string
		CVarWrapper haExitCVar = cvarManager->getCvar("ha_exit");
		if (!haExitCVar) { return; }
		auto reqUrlexit = cvarManager->getCvar("ha_exit");
		std::string reqUrlExitString = reqUrlexit.getStringValue();

		LOG("Using Exit Hook");
		SendCommands(reqUrlExitString);
		  
	}

}

void LightColorChanger::Log(std::string msg)
{
	//Send logs to BakkesMod console
	cvarManager->log(msg);
}
