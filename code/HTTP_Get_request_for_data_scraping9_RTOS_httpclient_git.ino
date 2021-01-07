/*******************************************************************
    A sample project for making a HTTP/HTTPS GET request on an ESP8266
    It will connect to the given request and print the body to
    serial monitor
    Parts:
    D1 Mini ESP8266 * - http://s.click.aliexpress.com/e/uzFUnIe
 *  * = Affilate
    If you find what I do usefuland would like to support me,
    please consider becoming a sponsor on Github
    https://github.com/sponsors/witnessmenow/
    Written by Brian Lough
    YouTube: https://www.youtube.com/brianlough
    Tindie: https://www.tindie.com/stores/brianlough/
    Twitter: https://twitter.com/witnessmenow

    adapted by Stuart A Spray C.Eng MIEE      'simplyEngineering'
 *******************************************************************/

#include <WiFi.h>
#include <string.h>
#include "Arduino.h"
#include "heltec.h"
#include <HttpClient.h>

#define WIFI_TIMEOUT_MS 20000 // 20 second WiFi connection timeout
#define WIFI_RECOVER_TIME_MS 30000 // Wait 30 seconds after a failed connection attempt
#define VERSION "HTTP_Get_request_for_data_scraping9_RTOS_httpclient_git"

int displayLoop = 0;
bool  halfSec = false;
bool  isConnected = false;
unsigned long oldMillis = 0;
String getTemp,getHumid,getWeight,getLight,getCO2,getPress = "";
char ssid[] = "HUAWEI-F212";       // your network SSID (name)
char password[] = "5Q0H0TAQRT1";  // your network key
WiFiClient client;
#define TEST_HOST "192.168.8.108"

void keepWiFiAlive(void * parameter){
    for(;;){
        if(WiFi.status() == WL_CONNECTED){
            vTaskDelay(10000 / portTICK_PERIOD_MS);
            continue;
        }

    //    Serial.println("[WIFI] Connecting");
        WiFi.mode(WIFI_STA);
        WiFi.begin("HUAWEI-F212", "5Q0H0TAQRT1");

        unsigned long startAttemptTime = millis();

        // Keep looping while we're not connected and haven't reached the timeout
        while (WiFi.status() != WL_CONNECTED && 
                millis() - startAttemptTime < WIFI_TIMEOUT_MS){}

        // When we couldn't make a WiFi connection (or the timeout expired)
      // sleep for a while and then retry.
        if(WiFi.status() != WL_CONNECTED){
      //      Serial.println("[WIFI] FAILED");
              isConnected = false;
            vTaskDelay(WIFI_RECOVER_TIME_MS / portTICK_PERIOD_MS);
        continue;
        }
        isConnected = true;
     //   Serial.println("[WIFI] Connected: " + WiFi.localIP());
    }
}

void setup() 
{
  Serial.begin(115200);
  delay(500);
  Serial.println(VERSION);
  xTaskCreatePinnedToCore(
  keepWiFiAlive,
  "keepWiFiAlive",  // Task name
  7000,             // Stack size (bytes)
  NULL,             // Parameter
  1,                // Task priority
  NULL,             // Task handle
  0                 // Task core
);
 
// initialise display etc
  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->clear();
  Heltec.display->flipScreenVertically(); 
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->drawStringMaxWidth(0, 0, 128,VERSION);
  Heltec.display->display();
}


void makeHTTPRequest() {
  char ch[4000] = {};
  int i  = 0; // index for ch array
  char status[32] = {0};
  if (!client.connect(TEST_HOST, 80))
  {
    Serial.println(F("Connection failed"));
    return;
  }
  yield();

  client.print(F("GET "));
  client.println(F(" HTTP/1.1"));
  client.print(F("Host: "));
  client.println(TEST_HOST);
  client.println(F("Cache-Control: no-cache"));
  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  delay(100);
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }
  while(client.connected()) 
  {
    while(client.available())
    {
      ch[i] = client.read();
      i++;
    }
  }
  Serial.println();

  char * pch;
  char key[] = "name='nowTemp'";
  pch = strstr (ch,key);
  char  result[27];
  result[26] = '\0';
  strncpy (result,pch,26);
  getTemp = String(result);
  getTemp = getTemp.substring(22) + "C";

  char * pch1;
  char key1[] = "name='nowHumid'";
  pch1 = strstr (ch,key1);
  char  result1[28];
  result1[27] = '\0';
  strncpy (result1,pch1,27);
  getHumid = String(result1);
  getHumid = getHumid.substring(23) + "%";

  char * pch2;
  char key2[] = "id='nowWeight'";
  pch2 = strstr (ch,key2);
  char  result2[41];
  result2[40] = '\0';
  strncpy (result2,pch2,40);
  getWeight = String(result2);
  getWeight = getWeight.substring(36) + "kG";

  char * pch3;
  char key3[] = "name='nowPress'";
  pch3 = strstr (ch,key3);
  char  result3[28];
  result3[27] = '\0';
  strncpy (result3,pch3,27);
  String x = String(result3);
  getPress = x.substring(23) + "mBar";

  char * pch4;
  char key4[] = "name='nowLight'";
  pch4 = strstr (ch,key4);
  char  result4[29];
  result4[28] = '\0';
  strncpy (result4,pch4,28);
  getLight = String(result4);
  getLight = getLight.substring(23) + "Lux";

  char * pch5;
  char key5[] = "id='nowCO2'";
  pch5 = strstr (ch,key5);
  char  result5[38];
  result5[37] = '\0';
  strncpy (result5,pch5,37);
  getCO2 = String(result5);
  getCO2 = getCO2.substring(33) + "ppm";

  
  Serial.println(getTemp);
  Serial.println(getHumid);
  Serial.println(getWeight);
  Serial.println(getCO2);
  Serial.println(getPress);
  Serial.println(getLight);
  Serial.print("loop() running on core ");
  Serial.println(xPortGetCoreID());
}



void loop() 
{
  if(millis() - oldMillis > 500)  // flash interval mS
  {
    halfSec = !halfSec;   // toggle flag
    oldMillis = millis();
    ++displayLoop;
    Heltec.display->clear();
    Heltec.display->setFont(ArialMT_Plain_24);
    if(halfSec && isConnected) 
    {
      Heltec.display->drawString(10, 42, getWeight + " * ");
      Heltec.display->drawString(10, 0, getTemp);
      Heltec.display->drawString(10, 21, getHumid);
    }
    else 
      Heltec.display->drawString(10, 42, getWeight + "   ");
      Heltec.display->drawString(10, 0, getTemp);
      Heltec.display->drawString(10, 21, getHumid);
  }
  Heltec.display->display();
  if(displayLoop > 10)
  {
    displayLoop = 0;
    makeHTTPRequest();
    oldMillis = millis();
  }
}
