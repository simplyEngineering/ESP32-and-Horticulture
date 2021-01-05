/* 
Copyright 2020 Stuart A Spray C.Eng MIEE
*/

#include "heltec.h"
#include <HTTPClient.h>
#include <iostream>
#include <string>
using namespace std;

const char* ssid     = "**************";
const char* password = "***************";
const char* ssidAlt     ="**************";
const char* passwordAlt = "**************";
const char* serverName = "http://******************/post-data.php";
char    buFfer[10];
String  rSSi = "";
String  LoRaPacket;
String  packet[8];
uint8_t pktIDX;
uint8_t packetSize;
uint8_t postcount = 0;
uint8_t waitCount = 0;
int     lastTime = 0;
int     interval = 0;
int     packetsReceived = 0;
int     packetsLost = 0;
float   packetQuality = 0;
String  packetLast;
String  tmp;
String  apiKeyValue = "tPmAT5Ab3j7F9";
String  rxreset;
String   wIfIconnectStatus = "CONNECT 0";

#define BAND    868E6  //you can set band here directly,e.g. 868E6,915E6
#define qBufferLen 100
uint8_t qBuffer[qBufferLen];
int     qBufferPtr = 0;
void setup() 
{
  Heltec.begin(true /*DisplayEnable Enable*/, true /*Heltec.LoRa Disable*/, true /*Serial Enable*/, true /*PABOOST Enable*/, BAND /*long BAND*/);
  netConnect();
}

void  netConnect()
{
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) 
  {
    delay(100);
    if (++waitCount > 10) // allow 5 seconds to connect
    {
      waitCount = 0;
      break;
    }
    wIfIconnectStatus = "CONNECT 0";
  }
  if (WiFi.status() == WL_CONNECTED) wIfIconnectStatus = "CONNECT 1";
}

bool  tryPost()
{
  bool stat;
  HTTPClient http;
  http.begin(serverName);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "api_key=" + apiKeyValue + "&pressure=" + packet[1]
                         + "&temperature=" + packet[2] + "&humidity=" + packet[3] + "&light=" + packet[4]
                         + "&battery=" + packet[5] + "&sequence=" + packet[6] + "&weight=" + packet[7] 
                         + "&rxreset=" + rxreset + "&packet=" + String(packetSize) + "&postcount=" + String(postcount)
                         + "&rssi=" + rSSi + "&lostpackets=" + packetsLost + "&quality=" + packetQuality + "";
  Serial.println(httpRequestData);
  int httpResponseCode = http.POST(httpRequestData);
  if (httpResponseCode == 200) 
  {
    stat = true;
    postcount = 1;
  }
  else 
  {
    stat = false;
    ++postcount;
  }
  http.end();
  return stat;
}
void processPacket()
{
  ++packetsReceived;
  interval = 0;
  LoRaPacket = "";
  pktIDX = 0;
  packet[0] = "";
  while (LoRa.available()) 
  {
    LoRaPacket += (char)LoRa.read();
  }
  rSSi = LoRa.packetRssi();
  for (int i = 0; i < packetSize; i++)
  {
    tmp = LoRaPacket[i];
    if (tmp == ":")   // parameter separator
    {
      packet[++pktIDX] = packet[0];
      packet[0] = "";
    }
    else packet[0] += tmp; 
  }
  Serial.println();   // print whole packet
  for (int i = 1; i < 8;i++)
  {
    Serial.print(packet[i] + ":");
  }
  Serial.print(rSSi);
  if (tryPost()) rxreset = "YES";  // success or failure of previous post
  else 
  {
    rxreset = "NO";
    netConnect();
    delay(500);
  }
  Serial.println(rxreset);
  // calculate missing packets percentage for the most recent n received packets
  // store  differences between last and current sequence numbers in circular buffer
  // for each received packet compute ratio of lost packets in buffer to all packets
  // all packets = sum of lost packets in buffer + packets received (length of buffer n).

  if(++qBufferPtr > qBufferLen-1) qBufferPtr = 0;  // rotate circular buffer ptr
  if(packet[6].toInt() > 0 && packetLast.toInt() > 0) // only process if both current and last sequence number are non-zero 
  {
    packetsLost = packet[6].toInt() - packetLast.toInt() - 1;
    Serial.println("Lost:" + String(packetsLost));
    qBuffer[qBufferPtr] = packetsLost;
    int lostAccum = 0;    // calculate content of buffer
    for (int i = 0; i < qBufferLen; i++)
    {
      lostAccum += qBuffer[i];
    }
    packetQuality = float(qBufferLen *100/(qBufferLen + lostAccum));
  }
  LoRaData();
  Serial.println(qBufferPtr);
  Serial.println(buFfer);
  packetLast = packet[6];
  digitalWrite(LED, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(50);                       // wait for a second
  digitalWrite(LED, LOW);
}
void LoRaData()
{
  Heltec.display->clear();
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawString(60 , 0 , "ID: "+ packet[6]);
  Heltec.display->drawString(0 , 10 ,"Press: " + packet[1]);
  Heltec.display->drawString(75 , 10 ,"Wait:" + String(interval));
  Heltec.display->drawString(0 , 20 ,"Temp:  " + packet[2]);
  sprintf(buFfer,"%d/%2.0f%%",packetsReceived,packetQuality);
  Heltec.display->drawString(65 , 20 ,String(buFfer));
  Heltec.display->drawString(0 , 30 ,"Humid: " + packet[3]);
  Heltec.display->drawString(65 , 30 ,"Post:  " + rxreset);   // actually state of last POST
  Heltec.display->drawString(0 , 40 ,"Light: " + packet[4]);
  Heltec.display->drawString(65 , 40 ,wIfIconnectStatus);
  Heltec.display->drawString(0 , 50 ,"BattV:  " + packet[5]);
  Heltec.display->drawString(65 , 50 , "Wt: "+ packet[7]);
  Heltec.display->drawString(0, 0, "rssi: " +  rSSi);  
  Heltec.display->display();
}
void loop() 
{
  delay(10);
  packetSize = LoRa.parsePacket();
  if (packetSize) processPacket();
  if (millis() - lastTime > 1000)
  {
    if(++interval > 300)  // no lora data received for 5 minutes
    {
      for (int i = 0;i < 8; i++) packet[i] = "0000";  // set lora data to 0
      if (tryPost()) rxreset = "YES X";  // X says posted without lora data
      else 
      {
        rxreset = "NO";
      }
      interval = 0;
    }
    LoRaData();
    lastTime = millis();
    Serial.print(interval);
  }
}
