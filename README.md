# Employing microcontrollers (esp ESP32 and ESP8266) in a horticultural setting - a working example.

# The Project

Whatever the crop, balancing yield and quality against cost and time  demands tight control of growing conditions. However, such control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, processing, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies the various elements employed in the project with photographs of the prototype hardware, screenshots of typical database output, WEB pages, control/monitoring GUIs and code listings where appropriate.

|   |   |
|---|:--|	
|<image src = "/processors.s.jpg" width = "100%">|<b>Examples of the microcontrollers used from left to right (typical specs):</b> <br><b>ESP32-CAM:</b>ESP32-S chip, 520k SRAM, 4M PSRAM, 2MP OV2640 camera, microSD card, 10 accessible GPIOs and power pins, on-board WiFi antenna and IPEX connector for external antenna.<br><b>ESP8266(NodeMCU):</b>Single core 32-bit processor, 128k RAM, 4MB flash, 17 GPIO pins, I2C/SPI, 10-bit ADC, WiFi<br><b>Heltec WiFi kit 32:</b>Dual-core 32-bit processor, 448k ROM, 520k SRAM, WiFi, Blutooth, 128x64 OLED display, I2C/SPI, 34 GPIO, 2x8-bit DAC, up to 18 12-bit ADC (GPIO shared)<br><b>Heltec WiFi LoRa 32:</b>Similar to WiFi kit 32 + LoRa Wide Area Network capability.|
	
For the purpose of this project and with the cooperation of a local tomato grower having  polytunnels scattered over many hectares, a field operation was set up so that selected tunnels could be set up with appropriate monitoring and communication equipment (see later for details).

|<image src = "/typical%20small%20scale%20tunnel%20setup.s.jpg" width = "100%">|An example on-site polytunnel and outbuiding configuration|
|---|:--|

To conveniently facilitate local off-site R&D activities, a small greenhouse 'lab' was constructed, based upon a <a href="https://www.quickcrop.co.uk/product/vegtrug-tomato-greenhouse"> 'VegTrug' Tomato greenhouse</a>, a free-standing timber structure having a small footprint, a constraint due its unusual location within the cockpit of a dry-docked sailing yacht.

|<image src = "/vegtrug.jpg" width = "50%">|The basic 'VegTrug' frame prior to equipping with monitoring and control systems.
|---|:--|

# System Overview

The system structure is based on a star network,  each host (polytunnel) having its own autonomous data acquisition and control capabilities. Each host reports to a central hub using low-power wide-area network (LPWAN) protocols,  internet connected via a WiFi router to an SQL database server consolidating all host-originated parametric measurements. The local measurements are used autonomously to control various aspects of the Polytunnel environments whilst the database may be queried for analytical purposes.
