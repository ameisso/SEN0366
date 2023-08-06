/*
  SEN0366.h - Library for SEN0366 distance sensor
  Created by Antoine M* (ameisso.fr), JULY 14, 2023.
  MIT LICENSE
*/


#warning todo : add command to change device address, getters and setters for device address

#ifndef SEN0366_h
#define SEN0366_h

#include "Arduino.h"

// #define SEN0366_DEBUG

typedef void (*DistanceCallback)(float distance);
static DistanceCallback distanceCallback;

typedef void (*ErrorCallback)(String error);
static ErrorCallback errorCallback;

typedef void (*ConsoleCallback)(String console);
static ConsoleCallback consoleCallback;

class SEN0366
{
public:
  SEN0366(HardwareSerial &SerialRef);
  void setup();
  void update();
  void setLaserOn();
  void setLaserOff();
  void singleMesurement();
  void startContinuousMesurement();
  void setResolution(bool precise);
  void readCache();
  void shutdown();
  void setMesureFromTop();
  void command(char *data, int length);
  char computeCRC(unsigned char *data, int length);
  void setCommandDelay(int newDelay);

  float getLastMesuredDistance();
  void setDistanceCallback(DistanceCallback callback);
  void setErrorCallback(ErrorCallback callback);
  void setConsoleCallback(ConsoleCallback callback);

private:
  HardwareSerial *_serial;
  // commands
  char deviceAddress = 0x80;
  char readMachineNumber[4] = {0xFA, 0x06, 0x04, 0xFC};    // Unique machine Serial Number
  char readParameterCommand[4] = {0xFA, 0x06, 0x01, 0xFF}; // function canceled in the latest Hardware version

  char reviseDistanceAdd1[6] = {0xFA, 0x04, 0x06, 0x2B, 0x01, 0xD0}; // Correct the distance mesured from the real world mesurement (stored at power off).
  char reviseDistanceSub1[6] = {0xFA, 0x04, 0x06, 0x2D, 0x01, 0xCE}; // WARNING CRC FROM DOC DIFFER FROM COMPUTED CRC

  char dataReturnInterval0s[5] = {0xFA, 0x04, 0x05, 0x00, 0xFD};
  char dataReturnInterval1s[5] = {0xFA, 0x04, 0x05, 0x01, 0xFC};
  char dataReturnInterval5s[5] = {0xFA, 0x04, 0x05, 0x05, 0xF8};

  char mesureFromTopCommand[5] = {0xFA, 0x04, 0x08, 0x01, 0xF9};
  char mesureFromTailCommand[5] = {0xFA, 0x04, 0x08, 0x00, 0xFA};

  char mesuringRangeCommand5m[5] = {0xFA, 0x04, 0x09, 0x05, 0xF4};
  char mesuringRangeCommand10m[5] = {0xFA, 0x04, 0x09, 0x0A, 0xEF};
  char mesuringRangeCommand30m[5] = {0xFA, 0x04, 0x09, 0x1E, 0xDB};
  char mesuringRangeCommand50m[5] = {0xFA, 0x04, 0x09, 0x32, 0xC7};
  char mesuringRangeCommand80m[5] = {0xFA, 0x04, 0x09, 0x50, 0xA9};

  char updateFrequencyOs[5] = {0xFA, 0x04, 0x0A, 0x00, 0xF8};
  char updateFrequency1s[5] = {0xFA, 0x04, 0x0A, 0x01, 0xF7};
  char updateFrequency5s[5] = {0xFA, 0x04, 0x0A, 0x05, 0xF3};
  char updateFrequency1Os[5] = {0xFA, 0x04, 0x0A, 0x0A, 0xEE};
  char updateFrequency2Os[5] = {0xFA, 0x04, 0x0A, 0x14, 0xE4};

  char fineResolutionCommand[5] = {0xFA, 0x04, 0x0C, 0x02, 0xF4};
  char coarseResolutionCommand[5] = {0xFA, 0x04, 0x0C, 0x01, 0xF5};

  char disableMesureOnPowerOnCommand[5] = {0xFA, 0x04, 0x0D, 0x00, 0xF5};
  char enableMesureOnPowerOnCommand[5] = {0xFA, 0x04, 0x0D, 0x01, 0xF4};

  char singleMesurementBroadCastCommand[4] = {0xFA, 0x06, 0x06, 0xFA}; // do the mesurement and store it in the cache. Use "readCacheCommand" to get the value.

  char readCacheCommand[4] = {deviceAddress, 0x06, 0x07, 0x73}; // sends last mesured value.

  char singleMesurementCommand[4] = {deviceAddress, 0x06, 0x02, 0x78};
  char continuousReadCommand[4] = {deviceAddress, 0x06, 0x03, 0x77};

  char laserOnCommand[5] = {deviceAddress, 0x06, 0x05, 0x01, 0x74};
  char laserOffCommand[5] = {deviceAddress, 0x06, 0x05, 0x00, 0x75};

  char shutdownCommand[4] = {deviceAddress, 0x04, 0x02, 0x7A}; // shutdown the sensor but not the communications

  int _commandDelayMS;
  int _commandTimeoutMS;

  void sendCommand(char command[], int size);
  long _lastCommandTime;
  bool _waitingForAnswer = false;
  float _lastMesuredDistance = -1;
};

#ifdef SEN0366_DEBUG
#define SEN0366_DEBUG_PRINT(x) Serial.print(x)
#define SEN0366_DEBUG_PRINT_HEX(x) Serial.print(x, HEX)
#define SEN0366_DEBUG_PRINTLN(x) Serial.println(x)
#else
#define SEN0366_DEBUG_PRINT(x)
#define SEN0366_DEBUG_PRINT_HEX(x)
#define SEN0366_DEBUG_PRINTLN(x)
#endif

#endif
