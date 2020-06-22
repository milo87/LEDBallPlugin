#include "pch.h"
#include "LEDBallPlugin.h"
#include "Utils.h"
#include "SerialClass.h"

BAKKESMOD_PLUGIN(LEDBallPlugin, "LEDBallPlugin", plugin_version, PLUGINTYPE_THREADED)

void LEDBallPlugin::onLoad() {
	this->SP = new Serial(this);

	int comNum = 2;
	wchar_t comNumBuffer[5];

	bool isConnected = false;

	while (!isConnected) {
		cvarManager->log("Attempting to connect to COM" + std::to_string(comNum));
		wsprintfW(comNumBuffer, L"COM%d", comNum++);
		if(this->SP->Connect(comNumBuffer)) {
			isConnected = true;
			break;
		}
;		if (comNum == 10) {
			cvarManager->log("ERROR: Cannot connect");
			break;
		}
	}

	if (SP->IsConnected()) {
		cvarManager->log("Connected!");
		this->isPulsing = false;
		this->gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", 
			std::bind(&LEDBallPlugin::DoGoalFlash, this, 
				LEDBallPlugin::goal_flash_cycles,
				LEDBallPlugin::goal_flash_on_time,
				LEDBallPlugin::goal_flash_off_time
			)
		);
		this->StartLoop();
	}
}

void LEDBallPlugin::onUnload() {
	this->SP->~Serial();
}

void LEDBallPlugin::DoGoalFlash(int numCycles, int onTime, int offTime) {
	cvarManager->log("GOAL! " + std::to_string(numCycles) + "," + std::to_string(onTime) + ";" + std::to_string(offTime));
	this->UpdateArduino("GOAL;" + std::to_string(numCycles) + "," + std::to_string(onTime) + ";" + std::to_string(offTime) + ";", true);
	this->gameWrapper->SetTimeout(std::bind(&LEDBallPlugin::UnlockState, this), (float)(numCycles * (onTime + offTime)) / 1000);
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
		UpdateState(wrapper);
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

						uint8_t R = (uint8_t)(Utils::Lerp(0, 255, fc.R));
						uint8_t G = (uint8_t)(Utils::Lerp(0, 255, fc.G));
						uint8_t B = (uint8_t)(Utils::Lerp(0, 255, fc.B));

					
						std::string data = "TEAM;" + std::to_string(R) + "," + std::to_string(G) + "," + std::to_string(B) + ";";
						cvarManager->log(data);

						UpdateArduino(data);
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

void LEDBallPlugin::UpdateArduino(std::string data, bool locking) {
	const char* data_array = data.c_str();

	if (this->SP->IsConnected() && !this->stateLocked) {
		cvarManager->log(std::to_string(data.length()));
		this->SP->WriteData(data_array, static_cast<unsigned int>(data.length()));
	}

	if (locking) {
		this->stateLocked = true;
	}
}

void LEDBallPlugin::UnlockState() {
	if (this->stateLocked) {
		this->stateLocked = false;
	}
}