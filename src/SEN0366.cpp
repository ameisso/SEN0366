#include "SEN0366.h"

SEN0366::SEN0366(HardwareSerial &SerialRef)
{
  _serial = &SerialRef;
}

void SEN0366::setup()
{
  _serial->begin(9600, SERIAL_8N1, RX1, TX1, false);
  delay(500);
  _commandDelayMS = 50;
  _commandTimeoutMS = 1000;
  _serial->flush();
}

void SEN0366::update()
{
  if (_serial->available() > 0)
  {
    delay(50); // wait for the end of the feedback transmission
    int answerSize = _serial->available();
    unsigned char _data[answerSize] = {0};

    _serial->readBytes(_data, answerSize);
    SEN0366_DEBUG_PRINT("ANSWER [");
    SEN0366_DEBUG_PRINT(answerSize);
    SEN0366_DEBUG_PRINT("] ");

    for (int i = 0; i < answerSize; i++)
    {
      SEN0366_DEBUG_PRINT_HEX(_data[i]);
      SEN0366_DEBUG_PRINT(" ,");
    }
    SEN0366_DEBUG_PRINTLN();
    _waitingForAnswer = false;
    char CRC = computeCRC(_data, answerSize - 1);
    if (CRC != _data[answerSize - 1])
    {
      SEN0366_DEBUG_PRINTLN("answer skipped wrong CRC");
      if (errorCallback)
      {
        errorCallback("COMMAND SKIPPED WRONG CRC");
      }
      return;
    }
    // PARSING ANSWERS
    if (_data[0] == 0x80 && _data[1] == 0x06 && (_data[2] == 0x82 || _data[2] == 0x83))
    {
      if (_data[3] == 'E' && _data[4] == 'R' && _data[5] == 'R')
      {
        _lastMesuredDistance = -1;
        if (errorCallback)
        {
          errorCallback("OUT OF RANGE");
        }
      }
      else
      {
        if (answerSize == 11)
        {
          _lastMesuredDistance = (_data[3] - 0x30) * 100 + (_data[4] - 0x30) * 10 + (_data[5] - 0x30) * 1 + (_data[7] - 0x30) * 0.1 + (_data[8] - 0x30) * 0.01 + (_data[9] - 0x30) * 0.001;
        }
        else if (answerSize == 12)
        {
          _lastMesuredDistance = (_data[3] - 0x30) * 100 + (_data[4] - 0x30) * 10 + (_data[5] - 0x30) * 1 + (_data[7] - 0x30) * 0.1 + (_data[8] - 0x30) * 0.01 + (_data[9] - 0x30) * 0.001 + (_data[10] - 0x30) * 0.0001;
        }
        else
        {
          if (errorCallback)
          {
            errorCallback("WRONG MESUREMENT");
          }
        }

        if (distanceCallback)
        {
          distanceCallback(_lastMesuredDistance);
        }
        if (consoleCallback)
        {
          if (_data[2] == 0x82)
          {
            if (answerSize == 11)
            {
              consoleCallback("SINGLE MESUREMENT COARSE");
            }
            else
            {
              consoleCallback("SINGLE MESUREMENT FINE");
            }
          }
          else
          {
            if (answerSize == 11)
            {
              consoleCallback("CONTINUOUS MESUREMENT COARSE");
            }
            else
            {
              consoleCallback("CONTINUOUS MESUREMENT FINE");
            }
          }
        }
      }
    }
    else if (_data[0] == 0x80 && _data[1] == 0x06 && _data[2] == 0x85 && _data[3] == 0x01)
    {
      if (consoleCallback)
      {
        consoleCallback("LASER_ON/OFF");
      }
    }
    else if (_data[0] == 0x80 && _data[1] == 0x04 && _data[2] == 0x82 && _data[3] == 0xFA)
    {
      if (consoleCallback)
      {
        consoleCallback("STOP");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x8D && _data[3] == 0x75)
    {
      if (consoleCallback)
      {
        consoleCallback("MESUREMENT ENABLED/DISABLED AT STARTUP");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x88 && _data[3] == 0x7A)
    {
      if (consoleCallback)
      {
        consoleCallback("SET DISTANCE FROM TOP/TAIL");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x89 && _data[3] == 0x79)
    {
      if (consoleCallback)
      {
        consoleCallback("MESURING RANGE UPDATED");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x8A && _data[3] == 0x78)
    {
      if (consoleCallback)
      {
        consoleCallback("MESURING FREQUENCY UPDATED");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x85 && _data[3] == 0x7D)
    {
      if (consoleCallback)
      {
        consoleCallback("DATA INTERVAL UPDATED");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x8C && _data[3] == 0x76)
    {
      if (consoleCallback)
      {
        consoleCallback("RESOLUTION UPDATED");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x06 && _data[2] == 0x84)
    {
      if (consoleCallback)
      {
        String answer = "MACHINE NUMBER ";
        for (int i = 0; i < answerSize; i++)
        {
          char hexVal[4] = {0};
          itoa(_data[i], hexVal, 16);
          answer.concat(hexVal);
          answer.concat(", ");
          SEN0366_DEBUG_PRINT_HEX(_data[i]);
          SEN0366_DEBUG_PRINT(",");
        }
        consoleCallback(answer);
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x04 && _data[2] == 0x8B && _data[3] == 0x77)
    {
      if (consoleCallback)
      {
        consoleCallback("DISTANCE REVISED");
      }
    }
    else if (_data[0] == 0xFA && _data[1] == 0x06 && _data[2] == 0x81 && _data[3] == 0x80)
    {
      if (consoleCallback)
      {
        String answer = "SENSOR PARAMETERS ";
        for (int i = 0; i < answerSize; i++)
        {
          char hexVal[4] = {0};
          itoa(_data[i], hexVal, 16);
          answer.concat(hexVal);
          answer.concat(", ");
          SEN0366_DEBUG_PRINT_HEX(_data[i]);
          SEN0366_DEBUG_PRINT(",");
        }
        consoleCallback(answer);
      }
    }
    else
    {
      String answer;
      answer.concat("UNCATCHED ANSWER : ");

      for (int i = 0; i < answerSize; i++)
      {
        char hexVal[4] = {0};
        itoa(_data[i], hexVal, 16);
        answer.concat(hexVal);
        answer.concat(", ");
        SEN0366_DEBUG_PRINT_HEX(_data[i]);
        SEN0366_DEBUG_PRINT(",");
      }
      SEN0366_DEBUG_PRINTLN("\n------------");
      if (errorCallback)
      {
        errorCallback(answer);
      }
    }
  }
  else if (millis() - _lastCommandTime > _commandTimeoutMS * 2 && _waitingForAnswer)
  {
    _waitingForAnswer = false;
    if (errorCallback)
    {
      errorCallback("TIMEOUT");
    }
    SEN0366_DEBUG_PRINTLN("timeout");
  }
}

void SEN0366::setLaserOn()
{
  sendCommand(laserOnCommand, 5);
}

void SEN0366::setLaserOff()
{
  sendCommand(laserOffCommand, 5);
}

void SEN0366::singleMesurement()
{
  sendCommand(singleMesurementCommand, 4);
}

void SEN0366::startContinuousMesurement()
{
  sendCommand(continuousReadCommand, 4);
}

void SEN0366::setResolution(bool precise)
{
  if (precise)
  {
    // -> 0.1mm
    sendCommand(fineResolutionCommand, 5);
  }
  else
  {
    // -> 1mm
    sendCommand(coarseResolutionCommand, 5);
  }
}

void SEN0366::readCache()
{
  sendCommand(readCacheCommand, 4);
}

void SEN0366::shutdown()
{
  sendCommand(shutdownCommand, 4);
}

void SEN0366::setMesureFromTop()
{
  sendCommand(mesureFromTopCommand, 5);
}

void SEN0366::command(char *data, int length)
{
  sendCommand(data, length);
}

void SEN0366::setCommandDelay(int newDelay)
{
  _commandDelayMS = newDelay;
  _serial->setTimeout(_commandDelayMS);
  String message = "DELAY :";
  message.concat(_commandDelayMS);
  if (newDelay > _commandTimeoutMS)
  {
    _commandTimeoutMS = 2 * _commandDelayMS;
    message.concat(" > NEW TIMEOUT ");
    message.concat(_commandTimeoutMS);
    message.concat(" ");
  }
  if (consoleCallback)
  {
    consoleCallback(message);
  }
}

void SEN0366::sendCommand(char command[], int size)
{
  if (!_waitingForAnswer)
  {
    for (int i = 0; i < size; i++)
    {
      _serial->print(command[i]);
    }
    _lastCommandTime = millis();
    _waitingForAnswer = true;
    delay(_commandDelayMS);
  }
  else
  {
    if (errorCallback)
    {
      errorCallback("COMMAND NOT SENT, WAITING FOR ANSWER");
    }
  }
}

char SEN0366::computeCRC(unsigned char *data, int length)
{
  int sum = 0;
  for (int i = 0; i < length; i++)
  {
    sum += data[i];
  }
  sum = ~sum + 1;
  return sum;
}

void SEN0366::setDistanceCallback(DistanceCallback callback)
{
  distanceCallback = callback;
}

void SEN0366::setErrorCallback(ErrorCallback callback)
{
  errorCallback = callback;
}

void SEN0366::setConsoleCallback(ConsoleCallback callback)
{
  consoleCallback = callback;
}

float SEN0366::getLastMesuredDistance()
{
  return _lastMesuredDistance;
}