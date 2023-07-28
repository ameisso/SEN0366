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