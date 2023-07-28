# SEN0366
Arduino Lib to interface with DFRobot infrared distance sensor 

https://wiki.dfrobot.com/Infrared_Laser_Distance_Sensor_50m_80m_SKU_SEN0366



## Usage 
This Sensor uses Serial communication. 
First thing to do is to associate the sensor to the Serial port : 
```cpp
SEN0366 sensor(Serial1);
```

Then, you will nedd to setup some callbacks so your code is informed that new value are comming :

```cpp
void sensorErrorCallback(String error);
void sensorConsoleCallback(String console);
void sensorDistanceCallback(float distance);
```

Note that console and error callbacks are not necessary. 


You can now write your Arduino Code : 

```cpp
#include <Arduino.h>
#include <SEN0366.h>

SEN0366 sensor(Serial1);
void sensorDistanceCallback(float distance);

void setup()
{
  Serial.begin(9600); // Serial conected to computer
  sensor.setup();
  sensor.setDistanceCallback(sensorDistanceCallback);
  sensor.startContinuousMesurement();
}

void loop()
{
  sensor.update(); //waits and interpret answers.
}

void sensorDistanceCallback(float distance)
{
  Serial.print("distance :");
  Serial.println(distance);
}
```



