#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"

void RocketLeagueAssistant::SendCommands(std::string reqUrl, std::string event)
{

	//See if main menu hook is enabled
	CVarWrapper globalURLEnabledCvar = cvarManager->getCvar("globalURL_enabled");
	bool globalURLEnabled = globalURLEnabledCvar.getBoolValue();

	//Get automation url, transform
	CVarWrapper haglobalURLCVar = cvarManager->getCvar("ha_globalURL");
	if (!haglobalURLCVar) { return; }

	//convert to string
	auto globalURL = cvarManager->getCvar("ha_globalURL");
	std::string globalURLString = globalURL.getStringValue();

	//Get RGB Color string
	CVarWrapper haMyTeamPrimaryColorCVar = cvarManager->getCvar("ha_myTeamPrimaryRGBColor");
	if (!haMyTeamPrimaryColorCVar) { return; }
	CVarWrapper haOtherTeamPrimaryColorCVar = cvarManager->getCvar("ha_OtherTeamPrimaryRGBColor");
	if (!haOtherTeamPrimaryColorCVar) { return; }

	//convert to string
	auto myTeamPrimaryRGBColor = cvarManager->getCvar("ha_myTeamPrimaryRGBColor");
	std::string myTeamPrimaryRGBColorString = myTeamPrimaryRGBColor.getStringValue();
	auto OtherTeamPrimaryRGBColor = cvarManager->getCvar("ha_OtherTeamPrimaryRGBColor");
	std::string OtherTeamPrimaryRGBColorString = OtherTeamPrimaryRGBColor.getStringValue();
	LOG("Other Team's Color: {}", OtherTeamPrimaryRGBColorString);

	//JSON formatting
	CurlRequest req;
	std::string dataVar = "{\"data\" :";
	std::string both = dataVar + " \"" + event + "\"}";
	std::string colorJson;
	colorJson = ", \"teamColor\" : [{" + myTeamPrimaryRGBColorString + "},{" + OtherTeamPrimaryRGBColorString + "}]}"; //sends team's colors
	both = dataVar + "\"" + event + "\"" + colorJson;
	LOG("Sent:{}", both);



	req.verb = "POST";

	//Check if JSON is enabled
	CVarWrapper jsonEnableCvar = cvarManager->getCvar("jsonEnabled");
	bool jsonEnabled = jsonEnableCvar.getBoolValue();

	if (jsonEnabled == true)
	{
		auto reqUrlJSON = cvarManager->getCvar("ha_jsonURL");
		std::string reqUrlJSONString = reqUrlJSON.getStringValue();

		req.url = reqUrlJSONString;
	}
	else {

		if (globalURLEnabled) {

			req.url = globalURLString;
			LOG("Using Global URL");

		};

		if (!globalURLEnabled) {

			req.url = reqUrl;

		};
	};
	std::map<std::string, std::string> jsonHeader{ {"Content-Type", "application/json"} };

	req.headers = jsonHeader;

	LOG("{}", both);

	req.body = both;

	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
		});
}