/*
Copyright 2020 Stuart A Spray 'simplyEngineering'
*/

bool            serialMonitor = false;
#include        <ADS1X15.h>
#include        <ESP8266WiFi.h>
#include        <ESP8266HTTPClient.h>
#include        <Adafruit_Sensor.h>
#include        <Adafruit_BME280.h>
#include        <hp_BH1750.h>
#include        "HX711.h"
#include        <EasyNTPClient.h>
#include        <WiFiUdp.h>
#include        <time.h>
#define         PWM_IN 10
#define         HUMID 14
#define         PUMP 12
#define         HEAT 13
#define         FAN_VENT 16
#define         FAN_CIRC 2

Adafruit_BME280 bme; // temp/press/humid
ADS1115         ADS(0x48); // load
hp_BH1750       BH1750;  // light
WiFiServer      server(80);
WiFiUDP         udp;
EasyNTPClient ntpClient(udp, "pool.ntp.org", (0)); // IST = GMT + 5:30

String          version = "VERSION: vegtrug_monitor8_debug";
bool            avail = false;
String          header;
double          weightRaw = 0;
double          weightRawLast = 0;
volatile  long  hiStart = 0;
volatile  long  loStart = 0;
volatile long   tHi  = 0;
volatile long   tLo  = 0;
long            ppm = 0;
volatile bool   edgeR = false;
volatile bool   edgeF = false;
String          apiKeyValue = "*************";
const char*     ssid     = "***************";
const char*     password = "**************";
const char*     APssid = "***************";
const char*     APpassword = "*********";
const char*     serverName = "http://**************/post-data.php";
char            buFfer[10];
String          xtra,xtra1 = "";
bool            toggle = false;
int             waitCount = 0;
int             lastSample = 0;
int             lastWeight = 0;
int             pwmTime = 0;
int             sequence = 0;
float           gTemp = 0;
String          getTemperature = "24.5",getPressure,getHumidity = "50.0",getCO2,getLight,getWeight = "1000";
String          getSetHeat = "0";
String          getSetHumid = "0";
String          getSetVent = "0";
String          getSetWeight = "0";
String          postStatus = "";
unsigned long   currentTime = millis();
unsigned long   previousTime = 0; 
const long      timeoutTime = 2000;
bool            heatOff = false;
bool            ventOff = false;
bool            humidOff = false;
bool            weightOff = false;
float           tSet =23.0;
float           vSet = 26.0;
float           hSet = 40.0;
float           wSet = 0; // grams
const float     hHyst = 1;
const int       sampleInterval = 5000;  // main loop interval
int             mainLoopCount = 0;
const int       weightInterval = 100;  // weight sampling interval
bool            setPointChanged = true;
const float     weightScale = 1.0;
float           weightScaled = 0;
String          postSequence = "";
String          postDate = "";
String          demandWeight = "_";  // part of the demand string to indicate when pump,heat,humidity or ventilation have been demanded 
String          demandHeat = "_";
String          demandHumidity = "_";
String          demandVent = "_";
String          demand = "____";
long            oneMinuteCount = 0;
int             airChangePhase = 0;
bool            airChangeOff = true;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.softAP(APssid, APpassword);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  bool   status = bme.begin(0x76);  
  if (!status) 
  {
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
   //while (1);
  }
  pinMode(PWM_IN, INPUT_PULLUP);
  Serial.println(version);
  delay(100);
  server.begin();
  Serial.println(WiFi.localIP());
 // Serial.println(WiFi.softAPIP());
  attachInterrupt(digitalPinToInterrupt(10),isrR,RISING);
  
  pinMode(HUMID,OUTPUT);
  digitalWrite(HUMID,HIGH);
  pinMode(PUMP,OUTPUT);
  digitalWrite(PUMP,HIGH);
  pinMode(HEAT,OUTPUT);
  digitalWrite(HEAT,HIGH);
  //pinMode(FAN_CIRC,OUTPUT);
  //digitalWrite(FAN_CIRC,LOW);
  pinMode(FAN_VENT,OUTPUT);
  digitalWrite(FAN_VENT,HIGH);
   avail = BH1750.begin(BH1750_TO_GROUND);
  BH1750.start();
  if (!ADS.begin()) Serial.println("Device not found");
  ADS.setGain(16);
  ADS.setMode(1);
}
double static a,b,c,d = 0;
double static z1a,z1b,z2a,z2b = 0;
class Bessel4p{
  
    public:
    double process(double in)
    {
      a = in + 1.077 * z1a - 0.301 * z2a;
      b = 0.069 * a + 0.137 * z1a + 0.069 * z2a;
      c = b + 1.141 * z1b - 0.447 * z2b;
      d = 0.063 * c + 0.125 * z1b + 0.063 * z2b;
      z1a = a; z1b = z1a; z2a = c; z2b = z2a;
      return  d;     
    };
    void resetFilt(){
      a,b,c,d,z1a,z1b,z2a,z2b = 0;
    }
}bessel4p,bessel4p1;

String get_time(){ // from technote 122 ESP8266/ESP32 NTP Time
  time_t now;
  time(&now);
  char time_output[20];
  strftime(time_output, 20, "%T %d-%m-%y ", localtime(&now)); 
  return String(time_output);
}

bool  tryPost(){
  ++sequence;
  HTTPClient http;
  http.begin(serverName);  //"http://aerial-photoco.uk/polytunnel/post-data.php";
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  String httpRequestData = "api_key=" + apiKeyValue + "&pressure=" + getPressure
  + "&temperature=" + getTemperature + "&humidity=" + getHumidity + "&CO2=" + getCO2 + "&sequence=" 
  + sequence+ "&light=" + getLight + "&quality=" + "VT" + "&weight=" + getWeight  + "&rxreset=" + demand + "";  // rxreset field used for indicating service demand
  String  temp = httpRequestData;
  Serial.println(temp);
  int httpResponseCode = http.POST(temp);
  if(serialMonitor) Serial.println(httpResponseCode);
  if (httpResponseCode == 200) return true;
  else return false;
  http.end();
}

void getData(){
  float temp = 0;
  BH1750.start();
  gTemp = bme.readTemperature();
  sprintf(buFfer, "%2.1f", gTemp);
  getTemperature = buFfer;
  if(serialMonitor) Serial.println(getTemperature);
  sprintf(buFfer,"%2.1f",tSet);
  getSetHeat = buFfer;
  sprintf(buFfer, "%2.1f", bme.readHumidity());
  getHumidity = buFfer;
  if(serialMonitor)Serial.println(getHumidity);
  sprintf(buFfer,"%2.1f",hSet);
  getSetHumid = buFfer;
  sprintf(buFfer,"%2.2f",wSet);
  getSetWeight = buFfer;
  sprintf(buFfer, "%4.0f", bme.readPressure() / 100.0F);
  getPressure = buFfer;
  if(serialMonitor)Serial.println(getPressure);
  sprintf(buFfer, "%04d", ppm);
  getCO2 = buFfer;
  if(serialMonitor)Serial.println(getCO2);
  while (!BH1750.hasValue())
  temp =  BH1750.getLux() * 6.25;
  sprintf(buFfer, "%5.0f",temp);
  getLight = buFfer;
  if(serialMonitor)Serial.println(getLight);
  sprintf(buFfer, "%2.2f",weightScaled);
  getWeight = buFfer;
  if(serialMonitor)Serial.println(getWeight);
  sprintf(buFfer,"%2.1f",vSet);
  getSetVent = buFfer;
}

ICACHE_RAM_ATTR void isrR(){
  hiStart = millis();
  detachInterrupt(digitalPinToInterrupt(10));
  edgeR = true;
  attachInterrupt(digitalPinToInterrupt(10),isrF,FALLING);
}

ICACHE_RAM_ATTR void isrF(){
  loStart = millis();
  detachInterrupt(digitalPinToInterrupt(10));
  edgeF = true;
  attachInterrupt(digitalPinToInterrupt(10),isrR,RISING);
}

void thermoStat(){
  if(getTemperature.toFloat() > tSet + hHyst) {heatOff = true;demandHeat = "_";}
  if(getTemperature.toFloat() < tSet - hHyst) heatOff = false;
  if(heatOff){ 
     digitalWrite(HEAT, HIGH); 
     demandHeat = "_";
  }
  else {
    digitalWrite(HEAT, LOW);
    demandHeat = "H";
  }
}

void solarStat(){
  if(getTemperature.toFloat() > (vSet + hHyst) || airChangeOff == false) ventOff = false; // ventilate if above vSet or on phase = true
  if(getTemperature.toFloat() < (vSet - hHyst) && airChangeOff == true)  ventOff = true;  // ventilate off if both vSet below thresh and on phase = false
  if (!ventOff) {
      digitalWrite(FAN_VENT, LOW);
      demandVent = "V";
    } else {
      digitalWrite(FAN_VENT, HIGH);
      demandVent = "_";
    }
}

void  humiStat(){
  if(getHumidity.toFloat() > hSet + hHyst)  humidOff = true;
  if(getHumidity.toFloat() < hSet - hHyst)  humidOff = false;
  if (humidOff) {
    digitalWrite(HUMID, HIGH);
    demandHumidity = "_";
  } 
  else {
    digitalWrite(HUMID, LOW);
    demandHumidity = "Y";
  }
}

void  weightStat(float weight){
  if(weight > wSet + 0.05)  {
    weightOff = true;
    digitalWrite(PUMP, HIGH);
  }
  if(weight < wSet - 0.05)  {
    weightOff = false;
    digitalWrite(PUMP, LOW);
  }
  if (!weightOff) {
    digitalWrite(PUMP, LOW);
    demandWeight = "W";
  } else {
    digitalWrite(PUMP, HIGH);
    demandWeight = "_";
  } 
}
 
void loop(){
  if(millis() - oneMinuteCount > 60000) // create 1:4 interval for air change cycle
  {
    if(airChangePhase < 1) airChangeOff = false;
    else airChangeOff = true;
    if(++ airChangePhase > 3) airChangePhase = 0; 
    oneMinuteCount = millis();
  }
  
  
  if(millis() - lastWeight > weightInterval){ // 100mS control loop
    if(!ADS.isBusy()){
      float zz = ADS.readADC_Differential_0_1();  //16 bit
      weightRaw = bessel4p.process(zz);
      weightScaled = ((weightRaw  - 49) * 19.8)/1000;  // tare and scale to kg
      lastWeight = millis();
    }
    thermoStat();
    solarStat();
    humiStat();
    weightStat(weightScaled);
  }
  
  if(millis() - lastSample > sampleInterval){
    lastSample = millis();
    getData();
    if(++mainLoopCount > 5) // 
    {
      mainLoopCount = 0;
      if (tryPost()) 
      {
        Serial.print("POST SUCCESS:");
        Serial.println(sequence);
        sprintf(buFfer,"%d",sequence);
        postSequence = buFfer;
        postDate = get_time();
        postStatus = "PY";
      }
      else 
      {
        Serial.println("POST FAILED");
        postStatus = "PN";      
      }
    }
  }
  WiFiClient client = server.available();   // Listen for incoming client
  if (client) {                             // If a new client connects,
    if(serialMonitor)Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) { // loop while the client's connected
      currentTime = millis();         
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        if(serialMonitor)Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            Serial.println("Header:" + header);
            if(header.substring(5,6) == "?") // is a form submission for change
            {
              if(header.substring(6,7) == "t") tSet = header.substring(11,15).toFloat();  // heatstat
              if(header.substring(6,7) == "h") hSet = header.substring(11,15).toFloat();  // humistat
              if(header.substring(6,7) == "v") vSet = header.substring(11,15).toFloat();  // ventstat
              if(header.substring(6,7) == "w") wSet = header.substring(11,15).toFloat();  // weightstat
              if(header.substring(6,7) == "s") wSet = getWeight.toFloat();  // set weight setpoint to current weight
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<meta charset='utf-8'>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".data{font-size: 18pt;}.plusMinus {font-size:24pt;color:black; background-color:rgb(200, 200, 200);}");
            client.println(".lbl{text-align:right;}");
            client.println(".button { background-color: green; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: red;}</style>");
            client.println("<script>function rLoad(){location.reload(true);}</script>");
            client.println("<script>setInterval(rLoad,5000);</script>");
            client.println("</head>");
            // Web Page Heading
            client.println("<body><h1>VegTrug</h1>");
            client.println("<table style = 'margin-left:auto; margin-right:auto'>");
            client.println("<tr>");
           
            client.println("<td style='width:10%' class='lbl'>PUMP</td>");
            if (!weightOff) {
              client.println("<td style='width:10%'><button class=\"button\">ON</button></td>");

            } else {
              client.println("<td style='width:10%'><button class=\"button button2\">OFF</button></td>");
            } 
            client.println("<td class='plusMinus'><input style='text-align:center;font-weight:bold' type='text' required size='4' id='setWeight' value='" + getSetWeight + "'></td>");
            client.println("<td class='plusMinus'><form action='/'><input required size='4' name = 'wSet' type='text'><input value='commit' type='submit'></form></td>");  //temp set point
           
            client.print("<td style='width:5%'></td><td style='width:5%'></td>");
            client.println("</tr>"); 
            client.println("<tr>");
            client.println("<td class='lbl'>HUMID</td>");
            if (humidOff) {
              client.println("<td><button class='button button2'>OFF</button></td>");
            } else {
              client.println("<td><button class='button'>ON</button></td>");
            }
            client.println("<td class='plusMinus'><input style='text-align:center;font-weight:bold' type='text' required size='4' id='setHumid' value='" + getSetHumid + "'></td>");
            client.println("<td class='plusMinus'><form action='/'><input required size='4' name = 'hSet' type='text'><input value='commit' type='submit'></form></td>");  //temp set point
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td class='lbl'>HEAT</td>");
              if(heatOff){ 
                client.println("<td><button class='button button2'>OFF</button></td>");
              }
              else {
                client.println("<td><button class='button '>ON</button></td>");

              }
              client.println("<td class='plusMinus'><input style='text-align:center;font-weight:bold' type='text' required size='4' id='setHeat' value='" + getSetHeat + "'></td>");
              client.println("<td class='plusMinus'><form action='/'><input required size='4' name = 'tSet' type='text'><input value='commit' type='submit'></form></td>");  //temp set point
              client.println("</tr>");
            client.println("<tr>");   
            client.println("<td class='lbl'>FAN-Circ</td>");
            client.println("<td><button class='button'>ON</button></td>");
            client.print("<td></td><td></td><td></td>");
            client.println("</tr>");
            client.println("<tr>");
            client.println("<td class='lbl'>FAN-Vent</td>");
            if (!ventOff) {
              client.println("<td><button class='button'>ON</button></td>");
            } else {
              client.println("<td><button class='button button2'>OFF</button></td>");
            }
            client.println("<td class='plusMinus'><input style='text-align:center;font-weight:bold' type='text' required size='4' id='setHeat' value='" + getSetVent + "'></td>");
            client.println("<td class='plusMinus'><form action='/'><input required size='4' name = 'vSet' type='text'><input value='commit' type='submit'></form></td>");  //temp set point
            client.println("</tr>");
           
            client.println("<tr><td class='lbl'><label class='data' for='nowTemp'>Tmp</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='nowTemp' name='nowTemp' value='" + getTemperature + "'></td>");
            client.println("<td class='lbl'><label class='data' for='nowLight'>Lgt</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='nowLight' name='nowLight' value='" + getLight + "'></td></tr>");
            
            client.println("<tr><td class='lbl'><label class='data' for='nowHumid'>Hum</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='nowHumid' name='nowHumid' value='" + getHumidity + "'></td>");
            client.println("<td class='lbl'><label class='data' for='nowPressure'>Prs</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='nowPress' name='nowPress' value='" + getPressure + "'></td></tr>");
            
            client.println("<tr><td class='lbl'><label class='data' for='nowCO2'>CO2</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='nowCO2' name='nowCO2' value='" + getCO2 + "'></td>");
            client.println("<td class='lbl'><label class='data' for='nowWeight'>Wgt</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='nowWeight' name='nowCO2' value='" + getWeight + "'></td></tr>");
            
            client.println("<tr><td class='lbl'><label class='data' for='postSeq'>Seq</label></td>");
            client.println("<td><input size='4' class='data' type='text' id='postSeq' name='postSeq' value='" + postSequence + "'></td>");
            client.println("<td class='lbl'><label class='data' for='postDate'>Stamp</label></td>");
            client.println("<td><input style='text-align:center;font-weight:bold' type='text' id='postDate' name='postDate' value='" + postDate + "'></td></tr>");
            client.println("</table>");
            client.println("<br>");
            client.println("<div style='text-align:center'</style>" + version + "</div>");
            client.println("</body>");
            client.println("</html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    demand = demandHeat + demandHumidity + demandWeight + demandVent;
    Serial.println("Demand:" + demand);
    if(serialMonitor)Serial.println("Client disconnected.");
    if(serialMonitor)Serial.println("");
  }
  
  if (edgeR){   
    edgeR = false;
    tLo = hiStart - loStart;
  }
  if (edgeF){
    edgeF = false;
    tHi = loStart - hiStart;
    if(tLo > 2 && tHi > 2) ppm = 5000 * (tHi - 2)/(tHi + tLo - 4);
  }
}
