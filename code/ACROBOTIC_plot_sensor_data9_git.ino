/*------------------------------------------------------------------------------
  10/20/2018
  Author: Cisco • A C R O B O T I C 
  Platforms: ESP8266
  Language: C++/Arduino
  File: bmp180_gui.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to plot sensor data from a BMP180 tem-
  perature, and barometric pressure sensor. A web server running on the ESP8266 
  serves a web page that uses Chart.js and websockets to plot the data.
  https://youtu.be/lEVoRJSsEa8
  ------------------------------------------------------------------------------
  Do you like my work? You can support me:
  https://patreon.com/acrobotic
  https://paypal.me/acrobotic
  https://buymeacoff.ee/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products and kits to help fund future Open-Source 
  projects like this! We'll always put our best effort in every project, and 
  release all our design files and code for you to use. 
  https://acrobotic.com/
  https://amazon.com/shops/acrobotic

  Adapted by Stuart A Spray C.Eng MIEE   'simplyEngineering'
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Ticker.h>
#include <Adafruit_BME280.h>  // modified from original bmp180
String VERSION = "ACROBOTIC_plot_sensor_data8";
Adafruit_BME280 bme; // I2C
// Collecting BMP180 sensor data
Ticker timer;

bool get_data = false;

// Connecting to the Internet
const char* ssid     = "BTHub5-9R6R";
const char* password = "5a7fe98fab";

// Running a web server
ESP8266WebServer server;

// Adding a websocket to the server
WebSocketsServer webSocket = WebSocketsServer(81);

// Serving a web page (from flash memory)
// formatted as a string literal!
char webpage[] PROGMEM = R"=====(
<html>
<!-- Adding a data chart using Chart.js -->
<head>
  <script src='https://cdnjs.cloudflare.com/ajax/libs/Chart.js/2.5.0/Chart.min.js'></script>
</head>
<body onload="javascript:init()">
<!-- Adding a slider for controlling data rate -->
<span>
  <input type="range" min="1" max="60" value="1" id="dataRateSlider" oninput="sendDataRate()" />
  <label for="dataRateSlider" id="dataRateLabel">Interval (Seconds)</label>
</span>
<span>
  <input type="range" min="20" max="100" value="20" id="dataLengthSlider" oninput="fixDataLength()" />
  <label for="dataLengthSlider" id="dataLengthLabel">Length</label>
</span><hr/>
<span><label id = "latestTemp">Latest temperature</label></span>
<hr />
<div>
  <canvas id="line-chartT" width="400" height="150"></canvas>
</div>
<div>
  <canvas id="line-chartH" width="400" height="150"></canvas>
</div>
<div>
  <canvas id="line-chartP" width="400" height="150"></canvas>
</div>
<!-- Adding a websocket to the client (webpage) -->
<script>
  var webSocket, dataPlotT,dataPlotH,dataPlotP;
  var maxDataPoints = 20;
  function removeData(){
    dataPlotT.data.labels.shift();
    dataPlotT.data.datasets[0].data.shift();
    dataPlotH.data.labels.shift();
    dataPlotH.data.datasets[0].data.shift();
    dataPlotP.data.labels.shift();
    dataPlotP.data.datasets[0].data.shift();
  }
  function addData(label, data) {
  document.getElementById("latestTemp").innerHTML = "<div><h1>Latest: " + data.temp.toFixed(1)
  + "C  " + data.humid.toFixed(0) + "%  " + data.press.toFixed(0) + "mBar </h1><h6>ACROBOTIC_plot_sensor_data9</h6></div>";
    if(dataPlotT.data.labels.length > maxDataPoints) removeData();
    dataPlotT.data.labels.push(label);
    dataPlotT.data.datasets[0].data.push(data.temp);
    dataPlotT.update();
    console.log(dataPlotT.data.datasets[0].data);
    dataPlotH.data.labels.push(label);
    dataPlotH.data.datasets[0].data.push(data.humid);
    dataPlotH.update();
    dataPlotP.data.labels.push(label);
    dataPlotP.data.datasets[0].data.push(data.press);
    dataPlotP.update();
    
  }
  function init() {
    webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
    dataPlotT = new Chart(document.getElementById("line-chartT"), {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          data: [],
          label: "Temperature (C)",
          borderColor: "#3e95cd",
          fill: false
        }]
      },
      options:
      {
        scales: 
        {
          yAxes: 
          [{
            scaleLabel:
            {
              display: true,
              labelString: 'C'
            },
            ticks: 
            {
              max: 40,
              min: 10
            }
          }],
         }
      }
    });
    dataPlotH = new Chart(document.getElementById("line-chartH"), {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          data: [],
          label: "Relative Humidity %",
          borderColor: "#3e95cd",
          fill: false
        }]
      },
      options:
      {
        scales: 
        {
          yAxes: 
          [{
            scaleLabel:
            {
              display: true,
              labelString: 'RH%'
            },
            ticks: 
            {
              max: 100,
              min: 10
            }
          }],
         }
      }
    });
    dataPlotP = new Chart(document.getElementById("line-chartP"), {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          data: [],
          label: "Pressure mBar",
          borderColor: "#3e95cd",
          fill: false
        }]
      },
      options:
      {
        scales: 
        {
          yAxes: 
          [{
            scaleLabel:
            {
              display: true,
              labelString: 'mBar'
            },
            ticks: 
            {
              max: 1020,
              min: 980
            }
          }],
         }
      }
    });
    webSocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
     // console.log(data);
      var today = new Date();
      var t = today.getHours() + ":" + today.getMinutes() + ":" + today.getSeconds();
      addData(t, data);
    }
  }
  function fixDataLength()
  {
    maxDataPoints = document.getElementById("dataLengthSlider").value;
    document.getElementById("dataLengthLabel").innerHTML = "Length: " + maxDataPoints;
  }
  function sendDataRate(){
    var dataRate = document.getElementById("dataRateSlider").value;
    webSocket.send(dataRate);
    //console.log(dataRate);
    document.getElementById("dataRateLabel").innerHTML = "Interval: " + dataRate + " Seconds";
  }
</script>
</body>
</html>
)=====";

void setup() {
  // put your setup code here, to run once:
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  delay(500);
  while(WiFi.status()!=WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println(VERSION);
  Serial.println("");
  Serial.print("Local IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/",[](){
    server.send_P(200, "text/html", webpage);
  });
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
 bool   status = bme.begin(0x76);  
  if (!status) 
  {
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
   while (1);
  }
  timer.attach(5, getData);
}

void loop() {
  // put your main code here, to run repeatedly:
  webSocket.loop();
  server.handleClient();
  if(get_data){
    const  float cTemp = bme.readTemperature();
    Serial.println(cTemp);
    String json = "{\"temp\":";
    json += bme.readTemperature();
    json += ",\"humid\":";
    json += bme.readHumidity();
    json += ",\"press\":";
    json += bme.readPressure()/100;
    json += "}";
    Serial.println(json);
    webSocket.broadcastTXT(json.c_str(), json.length());
    get_data = false;
  }
}

void getData() {
  get_data = true;
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length){
  // Do something with the data from the client
  if(type == WStype_TEXT){
    float dataRate = (float) atof((const char *) &payload[0]);
    timer.detach();
    timer.attach(dataRate, getData);
  }
}
