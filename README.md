# An account of the integration of embedded microcontrollers (esp ESP32 and ESP8266) in an indoor horticultural setting.



Whatever the crop, balancing yield and quality against cost and time  demands tight control of growing conditions. However, effective control cannot be carried out without an intimate knowledge of growing conditions, demanding measurement and control sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

For the purpose of this exercise and as a realistic field operation the growing facilities of a local tomato grower were recruited whos polytunnels were scattered over an area in excess of 100 hA. In order to conveniently facilitate R&D activities, a small greenhouse 'lab' was constructed, based upon a 'Veg trug' Tomato greenhouse, a free-standing timber structure having a small footprint due its unusual location within the cockpit of a dry-docked sailing yacht.

Control loops are provided for Temperature, Humidity, Irrigation and Ventilation whilst CO2 and light levels are monitored/logged for informational purposes only.

A single ESP8266 programmed in the Arduino environment incorporates standard off-the-shelf sensor libraries and delivers the following functionality:

	1: Temperature, humidity, weight, light  and atmospheric pressure are measured via I2C interfaces whilst CO2 measurments are interrupt based.  
	2: Relays are provided to switch a mains powered heater, humidifier and water pump and low voltage DC ventilation fans.  
	3: An interactive web page delivered via the local WiFi network provides monitoring of all parameters, the control status of each loop. and set-point configuration capabilities.   
	4: All measurements and control activity are logged to a remote SQL database for later off-line analysis.
