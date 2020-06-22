#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "SerialClass.h"

constexpr auto plugin_version = "1.0";

class LEDBallPlugin : public BakkesMod::Plugin::BakkesModPlugin
{
public:
	virtual void onLoad();
	virtual void onUnload();

private:
	Serial* SP;
	unsigned char R;
	unsigned char G;
	unsigned char B;

	static const int goal_flash_cycles = 10;
	static const int goal_flash_on_time = 60;
	static const int goal_flash_off_time = 120;

	bool stateLocked = false;

	void StartLoop();

	void DoGoalFlash(int numCycles, int onTime, int offTime);

	bool isPulsing;

	void UpdateMatchState();
	void UpdateState(ServerWrapper wrapper);
	ServerWrapper GetCurrentGameType();
	void UpdateArduino(std::string data, bool locking=false);
	void UnlockState();
};