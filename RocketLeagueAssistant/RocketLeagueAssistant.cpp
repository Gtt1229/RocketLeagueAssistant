#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"




BAKKESMOD_PLUGIN(RocketLeagueAssistant, "Rocket League HomeAsisstant Integration" , plugin_version, PLUGINTYPE_THREADEDUNLOAD)


std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::string haurlen = "None";

void RocketLeagueAssistant::onLoad()
{

	cvarManager->registerNotifier("rlamodel", [this](std::vector<std::string> args) {
		 
		UpdateModal();
	
		}, "", PERMISSION_ALL);

	//Enable Cvars
	cvarManager->registerCvar("ha_enabled", "1", "Enable Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("teams_enabled", "1", "Enable Team Colors", true, true, 0, true, 1);
	cvarManager->registerCvar("demos_enabled", "1", "Enable Demos Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("goalScored_enabled", "1", "Enable Overtime Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("freeplay_enabled", "1", "Enable Freeplay Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("mainmenu_enabled", "1", "Enable Mainemenu Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("overtime_enabled", "1", "Enable Overtime Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("exit_enabled", "1", "Enable Exit Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("isReplay", "0", "Replay boolean", true, true, 0, true, 1);
	cvarManager->registerCvar("hideURL", "false", "HideURL boolean", true, true, 0, true, 1);

	//HA version CVAR, only used for automation creation to confirm if the new webhook methods are needed
	cvarManager->registerCvar("version7", "false", "Version 2023.7 and up", true, true, 0, true, 1);

	//URL Cvars
	cvarManager->registerCvar("ha_haBaseURL", "http://192.168.1.256:8123");
	cvarManager->registerCvar("ha_jsonURL", "http://192.168.1.256:8123/api/webhook/webhook-light-example-json");

	//Team CVARs
	cvarManager->registerCvar("ha_playersTeam", "2");
	cvarManager->registerCvar("ha_otherTeam", "2");
	cvarManager->registerCvar("ha_myTeamPrimaryRGBColor", "\"r\":\"255\", \"g\":\"0\", \"b\":\"0\"");
	cvarManager->registerCvar("ha_OtherTeamPrimaryRGBColor", "\"r\":\"255\", \"g\":\"0\", \"b\":\"0\"");

	//Plugin update CVAR
	cvarManager->registerCvar("updateModal_enabled", "false", "Enable Plugin", true, true, 0, true, 1);


	//Prep for possibly using tokens in the future for requests over https
	cvarManager->registerCvar("ha_token", "Long-Lived-Access-Token", "Long Lived Access Token Generated On Home Assistant");

	//Log plugin started
	this->Log("RocketLeagueAssistant has started");

	//call LoadHooks method
	this->LoadHooks();




	_globalCvarManager = cvarManager;



}

void RocketLeagueAssistant::onUnload()
{

	CVarWrapper htokenCvar = cvarManager->getCvar("ha_token");
	if (!htokenCvar) { return; }
	htokenCvar.ResetToDefault();
	cvarManager->executeCommand("writeconfig", false);

}


void RocketLeagueAssistant::LoadHooks()
{
	//Teamcolors
	gameWrapper->HookEvent("Function Engine.Pawn.GetTeam", std::bind(&RocketLeagueAssistant::LoadTeams, this, std::placeholders::_1));
	//

	//Stats Feature - Goals and Demos events
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ActorWrapper caller, void* params, std::string eventname) {
			StatsHook(params);
		});
	//
	
	
	/*/Chat Beta - Can be used to trigger automations based on "What a save!","Nice one!", etc,etc, not implemented:
	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.HUDBase_TA.OnChatMessage",
		[this](ActorWrapper caller, void* params, std::string eventName) {
			ChatHook(params);
		});
	/*/

	//Overtime
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&RocketLeagueAssistant::OvertimeHook, this, std::placeholders::_1));
	//


	//Main Menu
	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&RocketLeagueAssistant::MainMenuHook, this, std::placeholders::_1));
	//
	 
	//On Game Exit
	gameWrapper->HookEvent("Function ProjectX.GFxShell_X.ExitGame", std::bind(&RocketLeagueAssistant::ExitHook, this, std::placeholders::_1));
	//
	
	//Check if it is a replay
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", std::bind(&RocketLeagueAssistant::Replay, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", std::bind(&RocketLeagueAssistant::NotReplay, this, std::placeholders::_1));
	//
}

void RocketLeagueAssistant::LoadTeams(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();
	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }
	
	//See if team colors are enabled
	CVarWrapper teamsEnabledCvar = cvarManager->getCvar("teams_enabled");
	bool teamsEnabled = teamsEnabledCvar.getBoolValue();
	if (!teamsEnabled) { LOG("Team color Automations are not enabled"); return; }

	//Check if it is a replay (this is may be temporary to minimize log flooding on Home Assistant)
	CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	bool isReplay = replayCvar.getBoolValue();
	if (isReplay == true) { Log("It's a replay"); return; }



	//Get player team and primary color
	if (gameWrapper->IsInFreeplay()) {

		CVarWrapper freeplayEnabledCvar = cvarManager->getCvar("freeplay_enabled");
		bool freeplayEnabled = freeplayEnabledCvar.getBoolValue();

		if (freeplayEnabled == true) {
			LOG("Player in freeplay, using freeplay hook");
			FreeplayHook();
			return;
		}
	
	}

	if (!gameWrapper->IsInFreeplay()) {


		CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
		bool isReplay = replayCvar.getBoolValue();
		if (isReplay == true) { Log("It's a replay"); return; }

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
						
		int otherteamnum = 0;
		
		if (teamnum == 0) {
		
			otherteamnum = 1;
		
		}
		
		if (teamnum == 1) {
		
			otherteamnum = 0;
		
		}

		TeamWrapper otherTeam = teams.Get(otherteamnum);


		//set other team's number based on player's current team number
		if (teamnum == 0) {

			otherteamnum = 1;
			TeamWrapper otherTeam = teams.Get(otherteamnum);
		}

		if (teamnum == 1) {

			otherteamnum = 0;
			TeamWrapper otherTeam = teams.Get(otherteamnum);
		}
		
		//Get player's team's linear color
		LinearColor primaryColor = myTeam.GetPrimaryColor();
		LinearColor otherPrimaryColor = otherTeam.GetPrimaryColor();


		//Begin linear to RGB convertion
		float myTeamPrimaryRed = primaryColor.R;
		float myTeamPrimaryGreen = primaryColor.G;
		float myTeamPrimaryBlue = primaryColor.B;


		float otherTeamPrimaryRed = otherPrimaryColor.R;
		float otherTeamPrimaryGreen = otherPrimaryColor.G;
		float otherTeamPrimaryBlue = otherPrimaryColor.B;
		
		CVarWrapper haMyTeamPrimaryRGBColorCvar = cvarManager->getCvar("ha_myTeamPrimaryRGBColor");

		if (!haMyTeamPrimaryRGBColorCvar) { return; }

		CVarWrapper haOtherTeamPrimaryRGBColorCvar = cvarManager->getCvar("ha_OtherTeamPrimaryRGBColor");

		if (!haOtherTeamPrimaryRGBColorCvar) { return; }

		//call to convert player's team's linear color to RGB
		std::string myTeamPrimaryRGB = ConvertLinearColor(myTeamPrimaryRed, myTeamPrimaryGreen, myTeamPrimaryBlue);
		haMyTeamPrimaryRGBColorCvar.setValue(myTeamPrimaryRGB);

		//call to convert other team's linear color to RGB
		std::string otherTeamPrimaryRGB = ConvertLinearColor(otherTeamPrimaryRed, otherTeamPrimaryGreen, otherTeamPrimaryBlue);
		haOtherTeamPrimaryRGBColorCvar.setValue(otherTeamPrimaryRGB);
	
		
		//get current team cvar
		CVarWrapper ha_playersTeam = cvarManager->getCvar("ha_playersTeam");
		if (!ha_playersTeam) { return; }

		CVarWrapper ha_otherTeam = cvarManager->getCvar("ha_otherTeam");
		if (!ha_otherTeam) { return; }
		//




		

		//Send based on home or away team
		if (teamnum <= 1) {

			if (teamnum == 0) {

				std::string event = "home";
				ha_playersTeam.setValue(teamnum);
				ha_otherTeam.setValue(1);
				LOG("Using Home Team Colors");
				SendCommands(event);
			}   

			if (teamnum == 1) {
				std::string event = "away";
				ha_playersTeam.setValue(teamnum);
				ha_otherTeam.setValue(0);
				LOG("Using Away Team Colors");
				SendCommands(event);
			}

		}
	}
}
	
std::string RocketLeagueAssistant::ConvertLinearColor(float red, float green, float blue)
{

	//convert linear color 0.1-1 to RGB 255
	//
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


	//Formats to JSON
	std::string rgbColors = "\"r\":" + R + ", \"g\":" + G + ", \"b\":" + B;
	//LOG("These the RGB colors{}", rgbColors);
	return rgbColors;

}


void RocketLeagueAssistant::StatsHook(void* params)
{

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

	//See if demos are enabled
	CVarWrapper demosEnabledCvar = cvarManager->getCvar("demos_enabled");
	bool demosEnabled = demosEnabledCvar.getBoolValue();
	//if (!demosEnabled) { LOG("Demos Automations are not enabled"); return; }
	
	//See if it is a replay
	CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	bool isReplay = replayCvar.getBoolValue();
	if (isReplay == true) { Log("It's a replay"); return; }

	//Current team CVAR
	//get current team cvar
	CVarWrapper ha_playersTeam = cvarManager->getCvar("ha_playersTeam");
	if (!ha_playersTeam) { return; }
	//
	auto haplayersTeam = cvarManager->getCvar("ha_playersTeam");
	int haplayersTeam2 = haplayersTeam.getIntValue();

	//get current otherteam cvar
	CVarWrapper ha_otherTeam = cvarManager->getCvar("ha_otherTeam");
	if (!ha_otherTeam) { return; }
	//
	auto haotherTeam = cvarManager->getCvar("ha_otherTeam");
	int haotherTeam2 = haotherTeam.getIntValue();

	//setup teamwrapper

	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { LOG("Server nullcheck failed");   return; }

	auto primary = server.GetLocalPrimaryPlayer();
	if (!primary) { LOG("Server primary nullcheck failed");   return; }

	auto teams = server.GetTeams();
	if (teams.Count() < 2) { return; }



	//Get a StatEvent's info
	struct StatTickerParams {
		uintptr_t Receiver;
		uintptr_t Victim;
		uintptr_t StatEvent;
	};
	StatTickerParams* pStruct = (StatTickerParams*)params;
	PriWrapper receiver = PriWrapper(pStruct->Receiver);
	PriWrapper victim = PriWrapper(pStruct->Victim);
	StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);
	


	//LOG("StatEventOccured");
	if (statEvent.GetEventName() == "Goal") {

		//See if Goal Scored hook is enabled
		CVarWrapper goalScoredEnabledCvar = cvarManager->getCvar("goalScored_enabled");
		bool goalScoredEnabled = goalScoredEnabledCvar.getBoolValue();
		if (!goalScoredEnabled) { LOG("goalScored Automations are not enabled"); return; }
	
		//See if it is a replay
		CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
		bool isReplay = replayCvar.getBoolValue();
		if (isReplay == true) { Log("It's a replay"); return; }


		int tmpCounter = 0;
		int lastGoalScoredBy = receiver.GetTeamNum();

		//Compare team number to players team to decide who scored
		if (lastGoalScoredBy <= 1) {

			if (lastGoalScoredBy == haplayersTeam2) {
				std::string event = "teamScored";
				LOG("Your team scored");
				SendCommands(event);
			}

			if (lastGoalScoredBy != haplayersTeam2) {
				std::string event = "otherTeamScored";
				LOG("Other team scored");
				SendCommands(event);
			}

		}

		LOG("Other Team's Score: {}", teams.Get(haotherTeam2).GetScore());
		LOG("Your Team's Score: {}", teams.Get(haplayersTeam2).GetScore());

		LOG("Using Goals Hook", lastGoalScoredBy);
	}
	//LOG("{}", statEvent.GetEventName());
	
	if (statEvent.GetEventName() == "Demolish") {
		LOG("Demo Occured"); 
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

		int receiversTeam = receiver.GetTeamNum();
		int victimsTeam = victim.GetTeamNum();
		int currentTeam = haplayersTeam2;

		//LOG("test: {}", testInt);
		//LOG("receiver: {}", receiversTeam);
		//LOG("victim: {}", victimsTeam);
		//LOG("Current team number: {}", currentTeam);

		if (receiversTeam == haplayersTeam2) {
				std::string event = "teamDemoed";
				LOG("Using Demos Hook");
				SendCommands(event);
			}

		if (receiversTeam != haplayersTeam2) {
			std::string event = "gotDemoed";
			LOG("Using Demos Hook");
			SendCommands(event);
		}		

	}

}

/*
//=================================
// Quickchat-based automation prep. May not be implemented
struct ChatMessage
{
	void* PRI;
	void* Team;
	wchar_t* PlayerName;
	uint8_t PlayerNamePadding[0x8];
	wchar_t* Message;
	uint8_t MessagePadding[0x8];
	uint8_t ChatChannel;
	unsigned long bPreset : 1;
};

void RocketLeagueAssistant::ChatHook(void* params)
{

	if (params)
	{
		ChatMessage* chatMessage = static_cast<ChatMessage*>(params);
		if (chatMessage->PlayerName == nullptr) return;
		std::wstring playerName(chatMessage->PlayerName);
		if (chatMessage->Message == nullptr) return;
		std::wstring message(chatMessage->Message);
		std::string bMessage(message.begin(), message.end());
		cvarManager->log("Message: " + bMessage);
		//std::string whatasave = "Group2Message4";
		//
		//
		//int isWhatASave = whatasave.compare(bMessage);
		//		
		//if (isWhatASave == 0) {
		//
		//	cvarManager->log("Get What a Saved'd");
		//
		//}
	}

}
//=================================
*/


void RocketLeagueAssistant::FreeplayHook()
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

	//CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	//bool isReplay = replayCvar.getBoolValue();
	//if (!isReplay) { Log("It's a replay"); return; }

	//May be redundant, but good to check
	if (gameWrapper->IsInFreeplay()) {

		//Get freeplay automation url, transform, and convert to string
		std::string event = "freeplay";
		LOG("Sending freeplay commands");
		SendCommands(event);

	}
}

void RocketLeagueAssistant::MainMenuHook(std::string name)
{




	CVarWrapper updateBoolcvar = cvarManager->getCvar("updateModal_enabled");
	bool updateBool = updateBoolcvar.getBoolValue();
	//LOG("Rocket League Assistant Update Boolean: {}", updateBool);
	if (updateBool == false) {
		UpdateModal();
	}
	

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

	//See if main menu hook is enabled
	CVarWrapper mainmenuEnabledCvar = cvarManager->getCvar("mainmenu_enabled");
	bool mainmenuEnabled = mainmenuEnabledCvar.getBoolValue();
	if (!mainmenuEnabled) { LOG("Main Menu Automations are not enabled"); return; }

	//Get mainmenu automation url, transform, and convert to string
	std::string event = "mainmenu";
	LOG("Using Main Menu Hook");
	SendCommands(event);

}


void RocketLeagueAssistant::OvertimeHook(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }
	
	
	//See if overtime hook is enabled
	CVarWrapper overtimeEnabledCvar = cvarManager->getCvar("overtime_enabled");
	bool overtimeEnabled = overtimeEnabledCvar.getBoolValue();
	if (!overtimeEnabled) { LOG("Overtime Automations are not enabled"); return; }
	
	//Get overtime automation url, transform, and convert to string
	std::string event = "overtime";
	LOG("Using Overtime Hook");
	SendCommands(event);
	
	

}

void RocketLeagueAssistant::ExitHook(std::string name)
{

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

	//See if exit hook is enabled
	CVarWrapper exitEnabledCvar = cvarManager->getCvar("exit_enabled");
	bool exitEnabled = exitEnabledCvar.getBoolValue();
	if (!exitEnabled) { LOG("Exit Automations are not enabled"); return; }

	std::string event = "exit";
	LOG("Using Exit Hook");
	SendCommands(event);
	  
}


void RocketLeagueAssistant::Replay(std::string name)
{
	//LOG("Replay start");
	CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	bool isReplay = replayCvar.getBoolValue();
	isReplay = true;
	//LOG("{}", isReplay);
	replayCvar.setValue(isReplay);
	

}

void RocketLeagueAssistant::NotReplay(std::string name)
{
	//LOG("Replay ended");
	CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	bool isReplay = replayCvar.getBoolValue();
	isReplay = false;
	//LOG("{}", isReplay);
	replayCvar.setValue(isReplay);

}

int RocketLeagueAssistant::GetScore(int teamNum)
{


	//setup teamwrapper
	//LOG("Getting score");
	ServerWrapper server = gameWrapper->GetCurrentGameState();
	if (!server) { LOG("Server nullcheck failed");   return 99; }

	auto primary = server.GetLocalPrimaryPlayer();
	if (!primary) { LOG("Server primary nullcheck failed");   return 99; }
	//LOG("TEAM NUM {}", teamNum);
	//LOG("gettings teams");
	auto teams = server.GetTeams();
	if (teams.Count() < 2) { return 99; }
	if (teams.Count() < 2) { return 99; }
	//LOG("Getting score2");
	int score = teams.Get(teamNum).GetScore();
	//LOG("Returning score");
	return score;

}

void RocketLeagueAssistant::Log(std::string msg)
{
	//Send logs to BakkesMod console
	cvarManager->log(msg);
}

void RocketLeagueAssistant::UpdateModal()
{
	//Notify users of JSON implementation with a Modal popup
	ModalWrapper updateModal = gameWrapper->CreateModal("Plugin Change");
	const std::string iconName = "Texture2D gfx_shared.Icon_Warning";
	updateModal.SetIcon(iconName);
	updateModal.SetColor(255, 157, 147);
	updateModal.SetBody(R"T(Rocket League Assistant has changed its default functionality to utilize JSON based requests.
	
Going forward, JSON will be utilized and seperate webhook are no longer available. To continue using seperate webhook, please visit the GitHub and compile a version less than v2.2.

Additionally, the team color JSON keys have been changed.

You can read about the changes at
https://github.com/Gtt1229/RocketLeagueAssistant )T");

	std::string name = "None";
	updateModal.AddButton("I Understand", false, [this, name]() {this->RocketLeagueAssistant::modalClosed("I understand"); });
	updateModal.AddButton("Open Bakkes Settings", true, [this, name]() {this->RocketLeagueAssistant::modalClosed("settings"); });

}

//void RocketLeagueAssistant::UpdateModal()
//{
//	//Notify users of JSON implementation with a Modal popup
//	TextInputModalWrapper updateModal = gameWrapper->CreateTextInputModal("Plugin Change");
//	const std::string iconName = "Texture2D gfx_shared.Icon_Warning";
//		
//	updateModal.SetTextInput("Enter text:", 30, false, [&](std::string input, bool was_closed)
//		{
//
//			RocketLeagueAssistant::OnInput(input, was_closed);
//
//
//		});
//		
//	
//
//}

//void RocketLeagueAssistant::OnInput(const std::string& input, bool was_closed)
//{
//	LOG("input:: {}", input);
//}

void RocketLeagueAssistant::modalClosed(std::string name) {

	CVarWrapper updateBoolcvar = cvarManager->getCvar("updateModal_enabled");
	if (!updateBoolcvar) { return; }
	bool updateBool = true;
	updateBoolcvar.setValue(updateBool);
	LOG("ModalClosed::: {}", updateBoolcvar.getBoolValue());

	if (name == "settings") {

		cvarManager->executeCommand("openmenu settings");

	}
	
	return;
	
}
