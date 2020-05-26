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

	const int goal_flash_cycles = 10;
	const int goal_flash_on_time = 50;
	const int goal_flash_off_time = 50;

	bool stateLocked = false;

	void StartLoop();

	void DoGoalFlash(int numCycles = 10, int onTime = 50, int offTime = 50);

	bool isPulsing;

	void UpdateMatchState();
	void UpdateState(ServerWrapper wrapper);
	ServerWrapper GetCurrentGameType();
	void UpdateArduino(string data, bool locking=false);
	void UnlockState();
};