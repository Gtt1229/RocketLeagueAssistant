#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"

void RocketLeagueAssistant::GetHAVersion() {

	//Get HA url, transform
	CVarWrapper haBaseURLURLCVar = cvarManager->getCvar("ha_haBaseURL");
	if (!haBaseURLURLCVar) { return; }

	//convert to string
	auto baseURL = cvarManager->getCvar("ha_haBaseURL");
	std::string baseURLString = baseURL.getStringValue();

	//Get token, transform
	CVarWrapper hatokenLCVar = cvarManager->getCvar("ha_token");
	if (!hatokenLCVar) { return; }

	//convert to string
	auto token = cvarManager->getCvar("ha_token");
	std::string tokenString = token.getStringValue();

	CurlRequest req;

	req.verb = "GET";
	req.url = baseURLString + "/api/config";

	std::map<std::string, std::string> jsonHeader{ {"Authorization", "Bearer " + tokenString} };

	req.headers = jsonHeader;






	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			//LOG("Code::: {}", code);
			if (code != 200)
			{
				LOG("Body: {}", result);
				return;
			}

			else
			{
				//parsing for relevant version change:
				std::string delimiter = R"T("version":")T";
				std::string versionOn = result.erase(0, result.find(delimiter) + delimiter.length());
				std::string delimiter2 = R"T(","config_source")T";
				std::string version = versionOn.substr(0, versionOn.find(delimiter2));
				//LOG("Version: {}", version);
				std::string delimiterPeriod = ".";
				//parse year
				std::string year = version.substr(0, version.find(delimiterPeriod));
				
				//parse month
				std::string versionNoYear = version.erase(0, version.find(delimiterPeriod) + delimiterPeriod.length());
				std::string month = versionNoYear.substr(0, versionNoYear.find(delimiterPeriod));
								
				int yearInt = std::stoi(year);
				int monthInt = std::stoi(month);
				
				if (yearInt >= 2023 && monthInt >= 5)
				{
					RocketLeagueAssistant::CreateAutomation(true);
				}
				else
				{
					RocketLeagueAssistant::CreateAutomation(false);
				}
			};

			

		});

}