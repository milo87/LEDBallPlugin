#include "pch.h"
#include "LEDBallPlugin.h"
#include "Utils.h"
#include "SerialClass.h"

BAKKESMOD_PLUGIN(LEDBallPlugin, "LEDBallPlugin", plugin_version, PLUGINTYPE_THREADED)

void LEDBallPlugin::onLoad() {
	cvarManager->log("Attempting to connect to COM port...");
	this->SP = new Serial(L"COM4", this);

	if (SP->IsConnected()) {
		cvarManager->log("Connected!");
		this->isPulsing = false;
		this->gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.TriggerGoalScoreEvent", bind(&LEDBallPlugin::DoGoalFlash, this));
		this->StartLoop();
	}
}

void LEDBallPlugin::onUnload() {
	this->SP->~Serial();
}

void LEDBallPlugin::DoGoalFlash() {
	cvarManager->log("GOAL!!!!!!!!!");
	this->UpdateArduino("FLASH");
}

void LEDBallPlugin::StartLoop() {
	gameWrapper->Execute(std::bind(&LEDBallPlugin::UpdateMatchState, this));
	this->gameWrapper->SetTimeout(std::bind(&LEDBallPlugin::StartLoop, this), 1);
}

void LEDBallPlugin::UpdateMatchState()
{
	ServerWrapper wrapper = GetCurrentGameType();

	if (!wrapper.IsNull()) {
		this->isPulsing = false;
		this->UpdateState(wrapper);
	}
	else {
		if (!this->isPulsing) {
			cvarManager->log("Not in a game, defaulting to PULSE");
			UpdateArduino("PULSE");
			isPulsing = true;
		}
	}

	return;
}

void LEDBallPlugin::UpdateState(ServerWrapper wrapper)
{
	auto players = wrapper.GetPRIs();
	ArrayWrapper<TeamWrapper> teams = wrapper.GetTeams();

	for (int i = 0; i < players.Count(); i++) {
		auto local = players.Get(i);
		if (!local.IsNull()) {
			if (local.GetUniqueId().ID == this->gameWrapper->GetSteamID()) {
				if (!local.GetTeam().IsNull()) {
					int teamIndex = local.GetTeam().GetTeamIndex();
					if (!teams.Get(teamIndex).IsNull()) {
						LinearColor fc = teams.Get(teamIndex).GetFontColor();

						uint8_t R = (uint8_t)(Utils::Lerp(0, 255, fc.R) * 0.3);
						uint8_t G = (uint8_t)(Utils::Lerp(0, 255, fc.G) * 0.6);
						uint8_t B = (uint8_t)(Utils::Lerp(0, 255, fc.B) * 0.1);

					
						string data = "TEAM " + to_string(R) + "," + to_string(G) + "," + to_string(B) + "N";
						cvarManager->log(data);

						this->UpdateArduino(data);
					}
				}
			}
		}
	}
}

ServerWrapper LEDBallPlugin::GetCurrentGameType()
{
	if (this->gameWrapper->IsInOnlineGame()) {
		return this->gameWrapper->GetOnlineGame();
	}
	else if (this->gameWrapper->IsInFreeplay()) {
		return this->gameWrapper->GetGameEventAsServer();
	}
	else {
		return NULL;
	}
}

void LEDBallPlugin::UpdateArduino(string data) {
	const char* data_array = data.c_str();
	if (SP->IsConnected()) {
		SP->WriteData(data_array, data.length());
	}
}
