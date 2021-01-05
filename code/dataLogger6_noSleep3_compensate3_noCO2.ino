// Uses part of  BMx280_I2C.ino.  Copyright (c) 2018 Gregor Christandl
//
// Data logger for inclusion in the Acre-Saver project
// Uses Heltec ESP WB-32 card, i2c BH1750 light sensor, i2c BME280 Temp/Humid/Press sensor and spi  MicroSD card adapter
// Pinout I2C: SCL 22,SDA 21 | SPI: MISO 19,MOSI 23,SCK 18,CS 2 | 3.3V for ESP32 and sensors and 5V for the microSD card
// I2C addr allocation: BME280 0x76, OLED 128x64 onboard display 0x23, RTC 0x68
// Camera control output to pin 13.
//
// Above modified for the Lora board.  SD removed. RTC removed. Cam control removed.
// test mode removed. Lora stuff from OLED_LoRa_Sender1

#define measureInterval 26300
#define diffMax 100  // used to filter out weight spikes
#include "SPI.h"
#include <hp_BH1750.h>
#include "heltec.h"
#include "Seeed_BME280.h"
#include "HX711.h"
#define   BAND    868E6
long  millisOld = 0;

struct  Weights 
{
  float w1 = 0;
  float w2 = 0;
} weights,weights2;

RTC_DATA_ATTR uint32_t counter = 0;
String rssi = "RSSI --";
String packSize = "--";
String packet ;
char        buffer[150] = {};
float       lux = 0;
bool        avail = false;
float       getPress, getTemp, getHumid, getWeight, getCo2, getVoc = 0;
uint16_t    getLight = 0;
RTC_DATA_ATTR bool doOnce = false;
int         battV = 0;
float       battVscaled = 0;
char        strBuff[10] = {};

hp_BH1750 BH1750;  // light
BME280 bme280;     // temp/humid/press
HX711 scale;       // weight

void setup()
{
  pinMode(4, INPUT);         // batt sense
  Serial.begin(115200);
  Heltec.begin(false /*DisplayEnable Enable*/, true /*Heltec.Heltec.Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  scale.begin(17/*data*/, 23/*clk*/);
  Wire.begin();
  if(!bme280.init())
  {
  Serial.println("Device error!");
  }
  avail = BH1750.begin(BH1750_TO_GROUND);
  BH1750.start();
  Serial.println("VERSION:dataLogger6_noSleep3_compensate3_noCO2");
}

void  LoRaSend()
{ 
  LoRa.beginPacket();
  //LoRa.setTxPower(14, RF_PACONFIG_PASELECT_PABOOST);
  sprintf(strBuff, "%4.1f:", float(getPress/100));
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%4.1f:", getTemp);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%4.1f:", getHumid);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%5d:", getLight);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%1.2f:", battVscaled);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%06d:", counter);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%5.0f:", getWeight);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%4.0f:", getCo2);
  LoRa.print(String(strBuff));
  sprintf(strBuff, "%4.0f:", getVoc);
  LoRa.print(String(strBuff));
  LoRa.endPacket();
}

Weights readWeight()
{
  float const tareWeight = 8337279 - (540*28);
  float const scaleFac = -28.11 * 0.98;
  int const   readings = 10;
  float diffWeight = 0;
  scale.read();  // allow to stabilise
  delay(100);
  scale.read();
  delay(100);
  scale.read();
  delay(100);
  weights.w1 = (scale.read_average(readings) - tareWeight)/scaleFac;
  weights.w2 = (scale.read_average(readings) - tareWeight)/scaleFac;
  return weights;
}
 
void  getSensors()
{
  avail = BH1750.begin(BH1750_TO_GROUND);
  BH1750.start();
  getPress = bme280.getPressure();
  getTemp =  bme280.getTemperature();
  getHumid = bme280.getHumidity();
  while (!BH1750.hasValue())
  getLight = BH1750.getLux() * 6.25;
  battVscaled = float(analogRead(4) * 4.68 / 2618);
  weights2 = readWeight();    // filter out spikes wher successive readings differ by more than
  float diff = weights2.w1 - weights2.w2;
  if (diff > - diffMax && diff < diffMax) getWeight = ((weights2.w1 + weights2.w2)/ 2);
  else getWeight = 99999;
  sprintf(buffer, "Pss:%4.1f Tmp:%04.1f Hmd:%04.1f Lgt:%05d bat:%03.2f Wgt:%5.0f Ctr:%4d CO2:%4.0f VOC:%4.0f\n", float(getPress/100), getTemp, getHumid, getLight, battVscaled, getWeight,counter,getCo2, getVoc);
}

void loop()
{
    if(millis() - millisOld > measureInterval) 
    {
        getSensors();
        Serial.println();
        Serial.print(buffer);
        delay(100);
        counter++;
        delay(50); 
        LoRaSend();
        digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
        delay(50);                       // wait for a second
        digitalWrite(LED, LOW);
        millisOld = millis();
    }
    Serial.print(".");
    delay(300);
}
