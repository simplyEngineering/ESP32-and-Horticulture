# Employing microcontrollers (esp ESP32 and ESP8266) in a horticultural setting - a working example.



Whatever the crop, balancing yield and quality against cost and time  demands tight control of growing conditions. However, effective control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies the various elements employed in the project with photographs of the prototype hardware  with code listings where appropriate.

Examples of the microcontrollers used from left to right: ESP32-CAM, ESP8266, ESP32 Heltec WiFi32, ESP32  Heltec LoraWan

<image src = "https://github.com/simplyEngineering/ESP32-and-Horticulture/blob/main/processors.s.jpg" width = "50%">


For the purpose of this project and with the cooperation of a local tomato grower, a field operation was set up having  polytunnels scattered over many hectares.
A typical on-site polytunnel and outbuiding configuration:

<image src = "https://github.com/simplyEngineering/ESP32-and-Horticulture/blob/main/typical%20small%20scale%20tunnel%20setup.s.jpg" width = "75%">


To conveniently facilitate local off-site R&D activities, a small greenhouse 'lab' was constructed, based upon a <a href="https://www.quickcrop.co.uk/product/vegtrug-tomato-greenhouse"> 'VegTrug' Tomato greenhouse</a>, a free-standing timber structure having a small footprint, a constraint due its unusual location within the cockpit of a dry-docked sailing yacht.

The basic 'VegTrug' frame:

<image src = "https://github.com/simplyEngineering/ESP32-and-Horticulture/blob/main/vegtrug.jpg" width = "25%">











Control loops are provided for Temperature, Humidity, Irrigation and Ventilation whilst CO2 and light levels are monitored/logged for informational purposes only.

A single ESP8266 programmed in the Arduino environment incorporates standard off-the-shelf sensor libraries and delivers the following functionality:

	1: Temperature, humidity, weight, light  and atmospheric pressure are measured via I2C interfaces whilst CO2 measurments are interrupt based.  
	2: Relays are provided to switch a mains powered heater, humidifier and water pump and low voltage DC ventilation fans.  
	3: An interactive web page delivered via the local WiFi network provides monitoring of all parameters, the control status of each loop. and set-point configuration capabilities.   
	4: All measurements and control activity are logged to a remote SQL database for later off-line analysis.
