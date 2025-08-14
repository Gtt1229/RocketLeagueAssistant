#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"

void RocketLeagueAssistant::SendCommands(std::string event)
{


	//Get RGB Color string
	CVarWrapper haMyTeamPrimaryColorCVar = cvarManager->getCvar("ha_myTeamPrimaryRGBColor");
	if (!haMyTeamPrimaryColorCVar) { return; }
	CVarWrapper haOtherTeamPrimaryColorCVar = cvarManager->getCvar("ha_OtherTeamPrimaryRGBColor");
	if (!haOtherTeamPrimaryColorCVar) { return; }

	

	//convert to string
	//auto myTeamPrimaryRGBColor = cvarManager->getCvar("ha_myTeamPrimaryRGBColor");
	std::string myTeamPrimaryRGBColorString = haMyTeamPrimaryColorCVar.getStringValue();
	//auto OtherTeamPrimaryRGBColor = cvarManager->getCvar("ha_OtherTeamPrimaryRGBColor");
	std::string OtherTeamPrimaryRGBColorString = haOtherTeamPrimaryColorCVar.getStringValue();
	//LOG("Other Team's Color: {}", OtherTeamPrimaryRGBColorString);

	//get current score
	// 

	std::string playerScoreString = "0";
	std::string otherScoreString = "0";
	int playerScore = 0;
	int otherScore = 0;

	if (!gameWrapper->IsInFreeplay()) {
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

	//LOG("Team nums:{}", haplayersTeam2);
	//LOG("Team nums:{}", haotherTeam2);
	playerScore = RocketLeagueAssistant::GetScore(haplayersTeam2);
	otherScore = RocketLeagueAssistant::GetScore(haotherTeam2);

	playerScoreString = std::to_string(playerScore);
	otherScoreString = std::to_string(otherScore);
	}

	//JSON formatting
	CurlRequest req;
	

	
	std::string jsonBody = R"T(
{
	"data": ")T" + event + R"T(",
	"TeamData": {
		"PlayersTeam": {
			"color": {
				)T" + myTeamPrimaryRGBColorString + R"T(
			},
			"score": )T" + playerScoreString + R"T(
		},
		"OtherTeam": {
			"color": {
				)T" + OtherTeamPrimaryRGBColorString + R"T(
			},
			"score": )T" + otherScoreString + R"T(
		}
	}
}

)T";

	
	//LOG("Sent:{}", jsonBody);
	req.verb = "POST";


	auto reqUrlJSON = cvarManager->getCvar("ha_jsonURL");
	std::string reqUrlJSONString = reqUrlJSON.getStringValue();

	req.url = reqUrlJSONString;

	std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };

	req.headers = jsonHeader;

	//LOG("{}", both);

	req.body = jsonBody;

	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
		});
}