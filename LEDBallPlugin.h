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

	void StartLoop();

	void UpdateMatchState();
	void UpdateState(ServerWrapper wrapper);
	ServerWrapper GetCurrentGameType();
	void UpdateArduino(const char* data, int len_data);
};