#include <ctime>
#include <iostream>
#include "pch.h"
#include "RocketLeagueAssistant.h"
#include "IMGUI/imgui_internal.h"
#include "IMGUI/imgui_searchablecombo.h"
#include "imgui_stdlib.h"
#include <iostream>
#include <vector>
#include <random>
#include <string>
#include <functional> //for std::function
#include <algorithm>  //for std::generate_n

//
// 
// 
// This creates the random string for the Webhook ID and Automation ID when created using the plugin.
//  
// 
//



typedef std::vector<char> char_array;

std::string random_string()
{
    std::string str("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, 32);             
}

std::string random_number()
{
    std::string str("012345678901234567890");

    std::random_device rd;
    std::mt19937 generator(rd());

    std::shuffle(str.begin(), str.end(), generator);

    return str.substr(0, 13);             
}

std::string RocketLeagueAssistant::GenWebHook() {



    std::string webhookURL = "rocketleagueassistant-"+random_string();
    return webhookURL;
 
};

std::string RocketLeagueAssistant::GenAutomationID() {

    std::string automationID = random_number();
    return automationID;
    
};