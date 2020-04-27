/*
I did not write this file. I found it here: https://playground.arduino.cc/Interfacing/CPPWindows
I did modify it for use within the context of a BakkesMod Plugin (adding a parent plugin for console logging support, etc.)
*/

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
    Serial(const LPCWSTR portName, BakkesMod::Plugin::BakkesModPlugin* parent);
    //Close the connection
    ~Serial();
    //Read data in a buffer, if nbChar is greater than the
    //maximum number of bytes available, it will return only the
    //bytes available. The function return -1 when nothing could
    //be read, the number of bytes actually read.
    int ReadData(char* buffer, unsigned int nbChar);
    //Writes data from a buffer through the Serial connection
    //return true on success.
    bool WriteData(const char* buffer, unsigned int nbChar);
    //Check if we are actually connected
    bool IsConnected();
};