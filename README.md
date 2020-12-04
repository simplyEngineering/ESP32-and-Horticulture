# Automatic-greenhouse
An ESP32/ESP8266-based experimental system incorporating monitoring, control and data logging capabilities for the express purpose of optimising crop growing conditions.

The system is incorporated in a free-standing structure provide by ??? having a small footprint due its unusual location within the cockpit of a dry-docked sailing yacht.

Control loops are provided for Temperature, Humidity, Irrigation and Ventilation whilst CO2 and light levels are monitored/logged for informational purposes only.

A single ESP8266 programmed in the Arduino environment incorporates standard off-the-shelf sensor libraries and delivers the following functionality:

1: Temperature, humidity, weight, light  and atmospheric pressure are measured via I2C interfaces whilst CO2 measurments are interrupt based.  
2: Relays are provided to switch a mains powered heater, humidifier and water pump and low voltage DC ventilation fans.  
3: An interactive web page delivered via the local WiFi network provides monitoring of all parameters, the control status of each loop. and set-point configuration capabilities.   
4: All measurements and control activity are logged to a remote SQL database for later off-line analysis.
