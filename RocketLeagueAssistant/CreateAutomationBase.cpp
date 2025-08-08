#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"


//
// 
// 
//This code will take the randomly generated automation ID and webhookURL from the RandomAlphaNumeric.cpp function and create a new automation to be used with the plugin.
// 
// 
//


void RocketLeagueAssistant::CreateAutomation(bool version7bool)
{


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

    //JSON URL CVAR
    CVarWrapper haJsonURLCvargui = cvarManager->getCvar("ha_jsonURL");
    if (!haJsonURLCvargui) { return; }
    
    //generate webhook and automation ID
    std::string webhookURL = RocketLeagueAssistant::GenWebHook();
    std::string automationID = RocketLeagueAssistant::GenAutomationID();

    //format URL to send commands to
    std::string generateWebHookFull = baseURLString + "/api/webhook/" + webhookURL;

    //set jsonURL for commands to be sent to
    haJsonURLCvargui.setValue(generateWebHookFull);

	CurlRequest req;

	req.verb = "POST";
	req.url = baseURLString + "/api/config/automation/config/" + automationID;

	//Token authorization header to create the automation via API
	std::map<std::string, std::string> jsonHeader{ {"Authorization", "Bearer " + tokenString}};

	req.headers = jsonHeader;
	
	std::string dataVar = "{\"id\" :";
	std::string last;
	//Automation YAML for pre 2023.7.X
	if (version7bool == false)
	{				
		last = R"T(
"alias": "RocketLeague - BakkesGenerated",
  "description": "",
  "trigger": [
    {
      "platform": "webhook",
      "webhook_id":")T" + webhookURL + R"T("
    }
  ],
  "condition": [],
  "action": [
    {
      "choose": [
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'home' }}",
              "alias": "Blue Team (Home) Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'away' }}",
              "alias": "Orange Team (Away) Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'mainmenu' }}",
              "alias": "Main Menu Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'matchcountdown' }}",
              "alias": "Match Countdown Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'teamScored' }}",
              "alias": "Your Team Scored Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'otherTeamScored' }}",
              "alias": "The Other Team Scored Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'teamDemoed' }}",
              "alias": "Your Team Demo'd the Other Team Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'gotDemoed' }}",
              "alias": "The Other Team Demo'd Your Team Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'endGameCountdown' }}",
              "alias": "The End of Game Countdown Started"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'matchEnded' }}",
              "alias": "The Match Officially Ended"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'freeplay' }}",
              "alias": "Freeplay Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'overtime' }}",
              "alias": "Overtime Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'exit' }}",
              "alias": "Game Exit Automation (ex: Return Lights to Normal)"
            }
          ],
          "sequence": []
        }
      ]
    }
  ],
  "mode": "single"
}
)T";

	}
	else
	{

//Automation with webhook attributes populated for HA 2023.7.x
    last = R"T(
"alias": "RocketLeague - BakkesGenerated 2023.7.X",
  "description": "",
  "trigger": [
    {
      "platform": "webhook",
      "webhook_id":")T" + webhookURL + R"T(",
      "allowed_methods": [
        "POST",
        "PUT"
      ],
      "local_only": true
    }
  ],
  "condition": [],
  "action": [
    {
      "choose": [
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'home' }}",
              "alias": "Blue Team (Home) Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'away' }}",
              "alias": "Orange Team (Away) Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'mainmenu' }}",
              "alias": "Main Menu Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'matchcountdown' }}",
              "alias": "Match Countdown Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'teamScored' }}",
              "alias": "Your Team Scored Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'otherTeamScored' }}",
              "alias": "The Other Team Scored Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'teamDemoed' }}",
              "alias": "Your Team Demo'd the Other Team Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'gotDemoed' }}",
              "alias": "The Other Team Demo'd Your Team Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'freeplay' }}",
              "alias": "Freeplay Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'overtime' }}",
              "alias": "Overtime Automation"
            }
          ],
          "sequence": []
        },
        {
          "conditions": [
            {
              "condition": "template",
              "value_template": "{{ trigger.json.data == 'exit' }}",
              "alias": "Game Exit Automation (ex: Return Lights to Normal)"
            }
          ],
          "sequence": []
        }
      ]
    }
  ],
  "mode": "single"
}
)T";
	}


	//LOG("{}", last);
	//Format's request for the automation it's YAML
	std::string both = dataVar + " \"" + automationID + "\"," + last;
		
	req.body = both;
	
	//LOG("{}", req.url);

	HttpWrapper::SendCurlRequest(req, [this](int code, std::string result)
		{
			LOG("Body result(Generally empty): {}", result);
		});

}