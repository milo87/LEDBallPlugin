#pragma once

constexpr auto ARDUINO_WAIT_TIME = 2000;

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "bakkesmod/plugin/bakkesmodplugin.h"

class Serial
{
private:
    // Parent plugin for logging
    BakkesMod::Plugin::BakkesModPlugin* parent;
    //Serial comm handler
    HANDLE hSerial;
    //Connection status
    bool connected;
    //Get various information about the connection
    COMSTAT status;
    //Keep track of last error
    DWORD errors;

public:
    //Initialize Serial communication with the given COM port
    Serial(BakkesMod::Plugin::BakkesModPlugin* parent);
    bool Connect(const LPCWSTR portName);
    //Close the connection
    ~Serial();
    void Disconnect();
    //Read data in a buffer, if nbChar is greater than the
    //maximum number of bytes available, it will return only the
    //bytes available. The function return -1 when nothing could
    //be read, the number of bytes actually read.
    char* ReadData(unsigned int nbChar);
    //Writes data from a buffer through the Serial connection
    //return true on success.
    bool WriteData(const char* buffer, unsigned int nbChar);
    //Check if we are actually connected
    bool IsConnected();
};