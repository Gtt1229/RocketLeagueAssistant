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

	// Get MMR data if enabled
	std::string mmrDataString = "";
	CVarWrapper mmrEnabledCvar = cvarManager->getCvar("mmr_enabled");
	if (mmrEnabledCvar && mmrEnabledCvar.getBoolValue() && (event == "matchEnded")) {
		mmrDataString = ", " + GetMMRData();
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
	})T" + mmrDataString + R"T(
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

std::string RocketLeagueAssistant::GetMMRData()
{
	try {
		// Only get MMR data for online games (not freeplay or replays)
		if (!gameWrapper->IsInOnlineGame()) {
			LOG("Not in online game, skipping MMR data");
			return "";
		}

		MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();
		if (mmrWrapper.memory_address == 0) {
			LOG("MMRWrapper is null");
			return "";
		}

		ServerWrapper server = gameWrapper->GetCurrentGameState();
		if (!server) {
			LOG("Server is null");
			return "";
		}

		auto primaryPlayer = server.GetLocalPrimaryPlayer();
		if (!primaryPlayer) {
			LOG("Primary player is null");
			return "";
		}

		auto playerPRI = primaryPlayer.GetPRI();
		if (!playerPRI) {
			LOG("Player PRI is null");
			return "";
		}

		UniqueIDWrapper playerUID = playerPRI.GetUniqueIdWrapper();

 		int currentPlaylist = mmrWrapper.GetCurrentPlaylist();
		
		// Check if MMR is synced and not currently syncing
		bool isSynced = mmrWrapper.IsSynced(playerUID, currentPlaylist);
		bool isSyncing = mmrWrapper.IsSyncing(playerUID);
		
		if (!isSynced || isSyncing) {
			LOG("MMR not synced or currently syncing, skipping MMR data");
			return "";
		}

		// Get current MMR and rank for the current playlist
		float currentMMRFloat = mmrWrapper.GetPlayerMMR(playerUID, currentPlaylist);
		int currentMMR = static_cast<int>(std::round(currentMMRFloat));
		SkillRank currentRank = mmrWrapper.GetPlayerRank(playerUID, currentPlaylist);

		// Map playlist IDs to names
		std::string playlistName = "Unknown";
		switch (currentPlaylist) {
			case 10: playlistName = "Solo Duel"; break;
			case 11: playlistName = "Doubles"; break;
			case 13: playlistName = "Standard"; break;
			case 27: playlistName = "Hoops"; break;
			case 28: playlistName = "Rumble"; break;
			case 29: playlistName = "Dropshot"; break;
			case 30: playlistName = "Snow Day"; break;
			case 34: playlistName = "Tournaments"; break;
			default: playlistName = "Playlist_" + std::to_string(currentPlaylist); break;
		}

		// Map tier to rank name
		std::string rankName = "Unranked";
		if (currentRank.Tier > 0 && currentRank.Tier <= 22) {
			const std::vector<std::string> rankNames = {
				"Unranked", "Bronze I", "Bronze II", "Bronze III",
				"Silver I", "Silver II", "Silver III",
				"Gold I", "Gold II", "Gold III",
				"Platinum I", "Platinum II", "Platinum III",
				"Diamond I", "Diamond II", "Diamond III",
				"Champion I", "Champion II", "Champion III",
				"Grand Champion I", "Grand Champion II", "Grand Champion III",
				"Supersonic Legend"
			};
			rankName = rankNames[currentRank.Tier];
			
			// Add division info for ranks
			if (currentRank.Tier < 22 && currentRank.Division >= 0 && currentRank.Division < 4) {
				rankName += " Div " + std::to_string(currentRank.Division + 1);
			}
		}

		// Get player name
		std::string playerName = playerPRI.GetPlayerName().ToString();

		// Get playlist name
		auto getPlaylistName = [](int playlistId) -> std::string {
			switch (playlistId) {
				case 10: return "Solo_Duel";
				case 11: return "Doubles";
				case 13: return "Standard";
				case 27: return "Hoops";
				case 28: return "Rumble";
				case 29: return "Dropshot";
				case 30: return "Snow_Day";
				case 34: return "Tournaments";
				default: return "Playlist_" + std::to_string(playlistId);
			}
		};

		// Get rank name
		auto getRankName = [](int tier, int division) -> std::string {
			if (tier <= 0 || tier > 22) return "Unranked";
			
			const std::vector<std::string> rankNames = {
				"Unranked", "Bronze I", "Bronze II", "Bronze III",
				"Silver I", "Silver II", "Silver III",
				"Gold I", "Gold II", "Gold III",
				"Platinum I", "Platinum II", "Platinum III",
				"Diamond I", "Diamond II", "Diamond III",
				"Champion I", "Champion II", "Champion III",
				"Grand Champion I", "Grand Champion II", "Grand Champion III",
				"Supersonic Legend"
			};
			
			std::string rankName = rankNames[tier];
			
			// Add division info for ranks
			if (tier < 22 && division >= 0 && division < 4) {
				rankName += " Div " + std::to_string(division + 1);
			}
			
			return rankName;
		};

		// Get current playlist info
		std::string currentPlaylistName = getPlaylistName(currentPlaylist);
		std::string currentRankName = getRankName(currentRank.Tier, currentRank.Division);

		// MMR data JSON with all playlists
		std::string mmrDataJson = R"T("MMRData": {
		"player_data": {
			"name": ")T" + playerName + R"T(",
			"uid": ")T" + playerUID.GetIdString() + R"T("
		},
		"current_playlist": {
			"id": )T" + std::to_string(currentPlaylist) + R"T(,
			"name": ")T" + currentPlaylistName + R"T(",
			"mmr": )T" + std::to_string(currentMMR) + R"T(,
			"tier": )T" + std::to_string(currentRank.Tier) + R"T(,
			"division": )T" + std::to_string(currentRank.Division) + R"T(,
			"matches_played": )T" + std::to_string(currentRank.MatchesPlayed) + R"T(,
			"rank_name": ")T" + currentRankName + R"T(",
			"is_synced": )T" + (isSynced ? "true" : "false") + R"T(
		},
		"ranks": {)T";

		// Get all playlist data
		std::vector<int> allPlaylists = {10, 11, 13, 27, 28, 29, 30, 34}; // All ranked playlists
		bool firstPlaylist = true;

		for (int playlistId : allPlaylists) {
			try {
				// Skip if this is a playlist that doesn't exist for this player
				if (!mmrWrapper.IsRanked(playlistId)) {
					continue;
				}

				float playlistMMRFloat = mmrWrapper.GetPlayerMMR(playerUID, playlistId);
				SkillRank playlistRank = mmrWrapper.GetPlayerRank(playerUID, playlistId);

				// Skip if no data available
				if (playlistMMRFloat <= 0 && playlistRank.Tier <= 0) {
					continue;
				}

				int playlistMMR = static_cast<int>(std::round(playlistMMRFloat));
				std::string playlistNameClean = getPlaylistName(playlistId);
				std::string playlistRankName = getRankName(playlistRank.Tier, playlistRank.Division);

				if (!firstPlaylist) {
					mmrDataJson += ",";
				}
				firstPlaylist = false;

				mmrDataJson += R"T(
			")T" + playlistNameClean + R"T(": {
				"mmr": )T" + std::to_string(playlistMMR) + R"T(,
				"tier": )T" + std::to_string(playlistRank.Tier) + R"T(,
				"division": )T" + std::to_string(playlistRank.Division) + R"T(,
				"matches_played": )T" + std::to_string(playlistRank.MatchesPlayed) + R"T(,
				"rank_name": ")T" + playlistRankName + R"T("
			})T";

			} catch (...) {
				// Skip this playlist if there's an error getting data
				continue;
			}
		}

		mmrDataJson += R"T(
		}
	})T";

		LOG("MMR Data Generated: {}", mmrDataJson);
		return mmrDataJson;

	} catch (const std::exception& e) {
		LOG("Exception in GetMMRData: {}", e.what());
		return "";
	} catch (...) {
		LOG("Unknown exception in GetMMRData");
		return "";
	}
}

void RocketLeagueAssistant::OnMatchWinnerSet(std::string eventName)
{
	// Check if MMR is enabled
	CVarWrapper mmrEnabledCvar = cvarManager->getCvar("mmr_enabled");
	if (!mmrEnabledCvar || !mmrEnabledCvar.getBoolValue()) {
		return;
	}

	// Only for online ranked games
	ServerWrapper sw = gameWrapper->GetOnlineGame();
	if (sw.IsNull() || !sw.IsOnlineMultiplayer() || gameWrapper->IsInReplay()) {
		return;
	}

	// Get the current playlist to check if it's ranked
	MMRWrapper mw = gameWrapper->GetMMRWrapper();
	if (mw.memory_address == 0) {
		return;
	}

	int currentPlaylist = mw.GetCurrentPlaylist();
	if (mw.IsRanked(currentPlaylist)) {
		// retry
		CheckMMRAndSend(5);
	}
}

void RocketLeagueAssistant::CheckMMRAndSend(int retryCount)
{
	// Check if MMR is still enabled
	CVarWrapper mmrEnabledCvar = cvarManager->getCvar("mmr_enabled");
	if (!mmrEnabledCvar || !mmrEnabledCvar.getBoolValue()) {
		return;
	}

	// Null checks
	ServerWrapper sw = gameWrapper->GetOnlineGame();
	if (sw.IsNull() || !sw.IsOnlineMultiplayer() || gameWrapper->IsInReplay()) {
		return;
	}

	if (retryCount > 20 || retryCount < 0) {
		LOG("MMR retry count exceeded, giving up");
		return;
	}

	MMRWrapper mw = gameWrapper->GetMMRWrapper();
	if (mw.memory_address == 0) {
		return;
	}

	UniqueIDWrapper uniqueID = gameWrapper->GetUniqueID();
	int userPlaylist = mw.GetCurrentPlaylist();

	if (userPlaylist != 0) {
		gameWrapper->SetTimeout([retryCount, this, uniqueID, userPlaylist](GameWrapper* gameWrapper) {
			MMRWrapper mmrWrapper = gameWrapper->GetMMRWrapper();
			
			// Check if MMR is synced
			if (mmrWrapper.IsSynced(uniqueID, userPlaylist) && !mmrWrapper.IsSyncing(uniqueID)) {
				LOG("MMR is synced, sending updated match data");
				// Send the match ended command with MMR data
				SendCommands("matchEnded");
			} else {
				// Retry if not synced yet
				if (retryCount > 0) {
					LOG("MMR not synced yet, retrying... (attempts left: {})", retryCount);
					gameWrapper->SetTimeout([retryCount, this](GameWrapper* gameWrapper) {
						this->CheckMMRAndSend(retryCount - 1);
					}, 0.5f);
				} else {
					LOG("MMR sync timeout, sending without MMR data");
					// Send without MMR data as fallback
					SendCommands("matchEnded");
				}
			}
		}, 3.0f); // Wait if not synced
	}
}