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

	cvarManager->registerCvar("ha_enabled", "1", "Enable Plugin", true, true, 0, true, 1);
	cvarManager->registerCvar("h_a", "http://192.168.1.256:8123/api/webhook/webhook-light-example");

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
}


void LightColorChanger::LoadHooks()
{
	//Load GameHooks
	gameWrapper->HookEvent("Function Engine.Pawn.GetTeam", std::bind(&LightColorChanger::LoadTeams, this, std::placeholders::_1));

}


void LightColorChanger::LoadTeams(std::string name)
{
	//Check if plugin is enabled
	CVarWrapper enableCvar = cvarManager->getCvar("ha_enabled");
	bool enabled = enableCvar.getBoolValue();
	
	if (!enabled) { LOG("LightColorChanger is not enabled"); return; }
	
	//Get player team and primary color

	if (gameWrapper->IsInFreeplay()) { /*LOG("Player is in training, not sending light command"); */ return; }

	cvarManager->log("Get Team Event");

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

	if (teamnum <= 1) {

		ChangeLights(teamnum);

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

void LightColorChanger::ChangeLights(int teamNum)
{
	//Send command(s) to HomeAssistant

	LOG("Preping Commands to HA");

	//get h_a url cvar
	CVarWrapper haCVar = cvarManager->getCvar("h_a");
	if (!haCVar) { return; }

	//this may be redundant?
	auto reqUrl = cvarManager->getCvar("h_a");
	//convert cvar to string
	std::string reqUrlString = reqUrl.getStringValue();

	LOG("Request URL: {}", reqUrlString);

	//Prep for possibly using tokens in the future for requests over https

	//get ha token cvar
	CVarWrapper tokenCVar = cvarManager->getCvar("ha_token");
	if (!tokenCVar) { return; }

	//this may be redundant?
	auto reqToken = cvarManager->getCvar("ha_token");
	//convert cvar to string
	std::string reqTokenString = reqToken.getStringValue();
		
	//LOG("Request URL{}", reqTokenString);


	//Send Curl to Automation assigned to Blue
	if (teamNum == 0) {
		CurlRequest req;
		req.url = reqUrlString + "-blue";
		req.body = R"T({
				"Authorization": "Bearer " ,
				"content-type": "application/json",
				})T";



		LOG("Sending Request as Home Team");
		HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
			{
				LOG("Body result(Generally empty): {}", result);
			});


	}


	//Send Curl to Automation assigned to Orange
	if (teamNum == 1) {
		CurlRequest req;
		req.url = reqUrlString + "-orange";
		req.body = R"T({
				"Authorization": "Bearer ",
				"content-type": "application/json",
				})T";
		


		LOG("Sending Request as Away Team");
		HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
			{
				LOG("Body result(Generally empty): {}", result);
			});


	}

}

void LightColorChanger::Log(std::string msg)
{
	//Send logs to BakkesMod console
	cvarManager->log(msg);
}