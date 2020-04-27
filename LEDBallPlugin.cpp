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
		this->StartLoop();
	}
}

void LEDBallPlugin::onUnload() {
	this->SP->~Serial();
}

void LEDBallPlugin::StartLoop() {
	gameWrapper->Execute(std::bind(&LEDBallPlugin::UpdateMatchState, this));
	this->gameWrapper->SetTimeout(std::bind(&LEDBallPlugin::StartLoop, this), 1);
}

void LEDBallPlugin::UpdateMatchState()
{
	ServerWrapper wrapper = GetCurrentGameType();

	if (!wrapper.IsNull())
		this->UpdateState(wrapper);

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

						string data = to_string(R) + "," + to_string(G) + "," + to_string(B) + "\n";

						cvarManager->log("RGB: " + data);

						UpdateArduino(data.c_str(), data.length());
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

void LEDBallPlugin::UpdateArduino(const char* data, int len_data) {
	if (SP->IsConnected()) {
		SP->WriteData(data, len_data);
	}
}
