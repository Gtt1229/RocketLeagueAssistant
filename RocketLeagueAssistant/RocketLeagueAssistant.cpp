#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"




BAKKESMOD_PLUGIN(RocketLeagueAssistant, "Rocket League/HomeAsisstant Integration", plugin_version, PLUGINTYPE_BOTAI)


std::shared_ptr<CVarManagerWrapper> _globalCvarManager;
std::string haurlen = "None";

void RocketLeagueAssistant::onLoad()
{

	//Enable Cvars
	cvarManager->registerCvar("ha_enabled", "1", "Enable Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("jsonEnabled", "1", "Enable Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("globalURL_enabled", "1", "Enable Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("teams_enabled", "1", "Enable Team Colors", true, true, 0, true, 1);
	cvarManager->registerCvar("demos_enabled", "1", "Enable Demos Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("goalScored_enabled", "1", "Enable Overtime Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("freeplay_enabled", "1", "Enable Freeplay Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("mainmenu_enabled", "1", "Enable Mainemenu Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("overtime_enabled", "1", "Enable Overtime Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("exit_enabled", "1", "Enable Exit Webhook", true, true, 0, true, 1);
	cvarManager->registerCvar("isReplay", "0", "Replay boolean", true, true, 0, true, 1);

	//URL Cvars
	cvarManager->registerCvar("ha_globalURL", "http://192.168.1.256:8123/api/webhook/webhook-light-example");
	cvarManager->registerCvar("ha_haBaseURL", "http://192.168.1.256:8123");
	cvarManager->registerCvar("ha_jsonURL", "http://192.168.1.256:8123/api/webhook/webhook-light-example-json");
	cvarManager->registerCvar("ha_home", "http://192.168.1.256:8123/api/webhook/webhook-light-example-home");
	cvarManager->registerCvar("ha_away", "http://192.168.1.256:8123/api/webhook/webhook-light-example-away");
	cvarManager->registerCvar("ha_demos", "http://192.168.1.256:8123/api/webhook/webhook-light-example-demos");
	cvarManager->registerCvar("ha_freeplay", "http://192.168.1.256:8123/api/webhook/webhook-light-example-freeplay");
	cvarManager->registerCvar("ha_mainmenu", "http://192.168.1.256:8123/api/webhook/webhook-light-example-mainmenu");
	cvarManager->registerCvar("ha_overtime", "http://192.168.1.256:8123/api/webhook/webhook-light-example-overtime");
	cvarManager->registerCvar("ha_exit", "http://192.168.1.256:8123/api/webhook/webhook-light-example-exit");
	cvarManager->registerCvar("ha_goalHome", "http://192.168.1.256:8123/api/webhook/webhook-light-example-homegoal");
	cvarManager->registerCvar("ha_goalAway", "http://192.168.1.256:8123/api/webhook/webhook-light-example-awaygoal");

	//Team CVARs
	cvarManager->registerCvar("ha_playersTeam", "2");
	cvarManager->registerCvar("ha_myTeamPrimaryRGBColor", "\"r\":\"255\", \"g\":\"0\", \"b\":\"0\"");
	cvarManager->registerCvar("ha_OtherTeamPrimaryRGBColor", "\"r\":\"255\", \"g\":\"0\", \"b\":\"0\"");


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

	LOG("Force Closed Game Test");
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

	//See if exit hook is enabled
	CVarWrapper exitEnabledCvar = cvarManager->getCvar("exit_enabled");
	bool exitEnabled = exitEnabledCvar.getBoolValue();
	if (!exitEnabled) { LOG("Exit Automations are not enabled"); return; }

	//Get exit automation url, transform, and convert to string
	CVarWrapper haExitCVar = cvarManager->getCvar("ha_exit");
	if (!haExitCVar) { return; }
	auto reqUrlexit = cvarManager->getCvar("ha_exit");
	std::string reqUrlExitString = reqUrlexit.getStringValue();
	std::string event = "exit";
	LOG("Using Exit Hook");
	SendCommands(reqUrlExitString, event);

	CVarWrapper htokenCvar = cvarManager->getCvar("ha_token");
	if (!htokenCvar) { return; }
	htokenCvar.ResetToDefault();
	cvarManager->executeCommand("writeconfig", false);

}


void RocketLeagueAssistant::LoadHooks()
{
	//Teamcolors
	gameWrapper->HookEvent("Function Engine.Pawn.GetTeam", std::bind(&RocketLeagueAssistant::LoadTeams, this, std::placeholders::_1));


	//Goal Scored
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&RocketLeagueAssistant::GoalScoredHook, this, std::placeholders::_1));

	//Stats Feature
	gameWrapper->HookEventWithCallerPost<ActorWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
		[this](ActorWrapper caller, void* params, std::string eventname) {
			StatsHook(params);
		});

	//Chat Beta
	gameWrapper->HookEventWithCaller<ActorWrapper>("Function TAGame.HUDBase_TA.OnChatMessage",
		[this](ActorWrapper caller, void* params, std::string eventName) {
			ChatHook(params);
		});

	//Overtime
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.OnOvertimeUpdated", std::bind(&RocketLeagueAssistant::OvertimeHook, this, std::placeholders::_1));

	//Main Menu
	gameWrapper->HookEvent("Function TAGame.GFxData_MainMenu_TA.MainMenuAdded", std::bind(&RocketLeagueAssistant::MainMenuHook, this, std::placeholders::_1));

	//On Game Exit
	gameWrapper->HookEvent("Function ProjectX.GFxShell_X.ExitGame", std::bind(&RocketLeagueAssistant::ExitHook, this, std::placeholders::_1));

	//Check if it is a replay
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", std::bind(&RocketLeagueAssistant::Replay, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", std::bind(&RocketLeagueAssistant::NotReplay, this, std::placeholders::_1));

	
	


	
	//Goal Scored Hook Test
	
	
	

	//gameWrapper->HookEventWithCallerPost<ServerWrapper>(
	//	"Function TAGame.GFxHUD_TA.HandleStatTickerMessage",
	//	std::bind(&RocketLeagueAssistant::GoalScoredTest, this,
	//		std::placeholders::_1, std::placeholders::_2));

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
		TeamWrapper otherTeam = teams.Get(otherteamnum);

		//set other team's number based on player's current team number

		if (teamnum == 0) {

			int otherteamnum = 1;
			TeamWrapper otherTeam = teams.Get(otherteamnum);
		}

		if (teamnum == 1) {

			int otherteamnum = 0;
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

		//


		//get ha url cvars	
		CVarWrapper haHomeCVar = cvarManager->getCvar("ha_home");
		if (!haHomeCVar) { return; }
		CVarWrapper haAwayCVar = cvarManager->getCvar("ha_away");
		if (!haAwayCVar) { return; }
		
		//get current team cvar
		CVarWrapper ha_playersTeam = cvarManager->getCvar("ha_playersTeam");
		if (!ha_playersTeam) { return; }
		//

		//this may be redundant?
		auto reqUrlHome = cvarManager->getCvar("ha_home");
		auto reqUrlAway = cvarManager->getCvar("ha_away");

		//

		//convert haurlcvars to string
		std::string reqUrlHomeString = reqUrlHome.getStringValue();
		std::string reqUrlAwayString = reqUrlAway.getStringValue();

		//Send based on home or away team
		if (teamnum <= 1) {

			if (teamnum == 0) {

				std::string event = "home";
				std::string reqUrlTeam = reqUrlHomeString;
				SendCommands(reqUrlTeam, event);
				ha_playersTeam.setValue(teamnum);
				LOG("Using Home Team Colors");

			}   

			if (teamnum == 1) {
				std::string event = "away";
				std::string reqUrlTeam = reqUrlAwayString;
				SendCommands(reqUrlTeam, event);
				ha_playersTeam.setValue(teamnum);
				LOG("Using Away Team Colors");
			}

		}
	}
}
	
std::string RocketLeagueAssistant::ConvertLinearColor(float red, float green, float blue)
{

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


	//Formats to JSON
	std::string rgbColors = "\"r\":\"" + R + "\", \"g\":\"" + G + "\", \"b\":\"" + B + "\"";
	LOG("{}", rgbColors);
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


	//Get goals automation url, transform, and convert to string
	CVarWrapper haGoalHomeCVar = cvarManager->getCvar("ha_goalHome");
	if (!haGoalHomeCVar) { return; }
	CVarWrapper haGoalAwayCVar = cvarManager->getCvar("ha_goalAway");
	if (!haGoalAwayCVar) { return; }
	//

	//Current team CVAR
	//get current team cvar
	CVarWrapper ha_playersTeam = cvarManager->getCvar("ha_playersTeam");
	if (!ha_playersTeam) { return; }
	//
	auto haplayersTeam = cvarManager->getCvar("ha_playersTeam");
	int haplayersTeam2 = haplayersTeam.getIntValue();

	//convert team to int
	//int haplayersTeam = ha_playersTeam.getIntValue();
	//
	//
	//this may be redundant?
	auto reqGoalUrlHome = cvarManager->getCvar("ha_goalHome");
	auto reqGoalUrlAway = cvarManager->getCvar("ha_goalAway");
	//

	//convert haurlcvars to string
	std::string reqUrlGoalHomeString = reqGoalUrlHome.getStringValue();
	std::string reqUrlGoalAwayString = reqGoalUrlAway.getStringValue();

	//Get demos automation url, transform, and convert to string
	CVarWrapper haDemosCVar = cvarManager->getCvar("ha_demos");
	if (!haDemosCVar) { return; }
	auto reqUrlDemos = cvarManager->getCvar("ha_demos");
	std::string reqUrlDemosString = reqUrlDemos.getStringValue();

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
	//
	LOG("StatEventOccured");
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

		if (lastGoalScoredBy <= 1) {

			if (lastGoalScoredBy == haplayersTeam2) {
				std::string event = "teamScored";
				LOG("Your team scored");
				SendCommands(reqUrlGoalHomeString, event);
			}

			if (lastGoalScoredBy != haplayersTeam2) {
				std::string event = "otherTeamScored";
				LOG("Other team scored");
				SendCommands(reqUrlGoalAwayString, event);
			}

		}



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
		int testInt = 22;
		int currentTeam = haplayersTeam2;

		//LOG("test: {}", testInt);
		//LOG("receiver: {}", receiversTeam);
		//LOG("victim: {}", victimsTeam);
		//LOG("Current team number: {}", currentTeam);

		if (receiversTeam == haplayersTeam2) {
				std::string event = "teamDemoed";
				LOG("Using Demos Hook");
				SendCommands(reqUrlDemosString, event);
			}

		if (receiversTeam != haplayersTeam2) {
			std::string event = "gotDemoed";
			LOG("Using Demos Hook");
			SendCommands(reqUrlDemosString, event);
		}		

	}

}

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
		std::string whatasave = "Group2Message4";

	
		int isWhatASave = whatasave.compare(bMessage);
				
		if (isWhatASave == 0) {

			cvarManager->log("Get What a Saved'd");

		}
	}

}

void RocketLeagueAssistant::FreeplayHook()
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

	////Check if it is a replay (this is may be temporary to minimize log flooding on Home Assistant)

	//CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	//bool isReplay = replayCvar.getBoolValue();
	//if (!isReplay) { Log("It's a replay"); return; }

	//May be redundant, but good to check

	if (gameWrapper->IsInFreeplay()) {

		//Get freeplay automation url, transform, and convert to string
		CVarWrapper haFreeplayCVar = cvarManager->getCvar("ha_freeplay");
		if (!haFreeplayCVar) { return; }
		auto reqUrlFreeplay = cvarManager->getCvar("ha_freeplay");
		std::string reqUrlFreeplayString = reqUrlFreeplay.getStringValue();
		std::string event = "freeplay";
		SendCommands(reqUrlFreeplayString, event);

	}
}

void RocketLeagueAssistant::MainMenuHook(std::string name)
{

	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();

	if (!enabled) { LOG("RocketLeagueAssistant is not enabled"); return; }

		//See if main menu hook is enabled
	CVarWrapper mainmenuEnabledCvar = cvarManager->getCvar("mainmenu_enabled");
	bool mainmenuEnabled = mainmenuEnabledCvar.getBoolValue();
	if (!mainmenuEnabled) { LOG("Main Menu Automations are not enabled"); return; }

	//Get mainmenu automation url, transform, and convert to string
	CVarWrapper haMainMenuCVar = cvarManager->getCvar("ha_mainmenu");
	if (!haMainMenuCVar) { return; }
	auto reqUrlmainmenu = cvarManager->getCvar("ha_mainmenu");
	std::string reqUrlMainMenuString = reqUrlmainmenu.getStringValue();
	std::string event = "mainmenu";
	LOG("Using Main Menu Hook");
	SendCommands(reqUrlMainMenuString, event);

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
	CVarWrapper haOvertimeCVar = cvarManager->getCvar("ha_overtime");
	if (!haOvertimeCVar) { return; }
	auto reqUrlovertime = cvarManager->getCvar("ha_overtime");
	std::string reqUrlOvertimeString = reqUrlovertime.getStringValue();
	std::string event = "overtime";
	LOG("Using Overtime Hook");
	SendCommands(reqUrlOvertimeString, event);
	
	

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

	//Get exit automation url, transform, and convert to string
	CVarWrapper haExitCVar = cvarManager->getCvar("ha_exit");
	if (!haExitCVar) { return; }
	auto reqUrlexit = cvarManager->getCvar("ha_exit");
	std::string reqUrlExitString = reqUrlexit.getStringValue();
	std::string event = "exit";
	LOG("Using Exit Hook");
	SendCommands(reqUrlExitString, event);
	  
}


void RocketLeagueAssistant::Replay(std::string name)
{
	LOG("Replay start");
	CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	bool isReplay = replayCvar.getBoolValue();
	isReplay = true;
	LOG("{}", isReplay);
	replayCvar.setValue(isReplay);
	

}

void RocketLeagueAssistant::NotReplay(std::string name)
{
	LOG("Replay ended");
	CVarWrapper replayCvar = cvarManager->getCvar("isReplay");
	bool isReplay = replayCvar.getBoolValue();
	isReplay = false;
	LOG("{}", isReplay);
	replayCvar.setValue(isReplay);

}

void RocketLeagueAssistant::Log(std::string msg)
{
	//Send logs to BakkesMod console
	cvarManager->log(msg);
}