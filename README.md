# Employing ESP32 and ESP8266 microcontrollers in a horticultural setting - a working example.

# The Project

Whatever the crop, balancing yield and quality against cost and time  demands tight control of growing conditions. However, such control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, processing, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies various experimental elements employed in the project with photographs of prototype hardware, screenshots of typical database output, WEB pages, control/monitoring GUIs and code listings with attributions where appropriate.

|   |   |
|---|:--|	
|<image src = "images/ESP32 and ESP8266 variants.jpg" width = "100%">|<b>Examples of the microcontrollers used from left to right (typical specs):</b> <br><b>ESP32-CAM:</b>ESP32-S chip, 520k SRAM, 4M PSRAM, 2MP OV2640 camera, microSD card, 10 accessible GPIOs and power pins, on-board WiFi antenna and IPEX connector for external antenna.<br><b>ESP8266(NodeMCU):</b>Single core 32-bit processor, 128k RAM, 4MB flash, 17 GPIO pins, I2C/SPI, 10-bit ADC, WiFi<br><b>Heltec WiFi kit 32:</b>Dual-core 32-bit processor, 448k ROM, 520k SRAM, WiFi, Blutooth, 128x64 OLED display, I2C/SPI, 34 GPIO, 2x8-bit DAC, up to 18 12-bit ADC (GPIO shared)<br><b>Heltec WiFi LoRa 32:</b>Similar to WiFi kit 32 + LoRa Wide Area Network capability.|
	
For the purpose of this project and with the cooperation of a local tomato grower having  polytunnels scattered over many hectares, a field operation was set up so that selected tunnels could be set up with appropriate monitoring and communication equipment (see later for details).

|<image src = "images/typical%20small%20scale%20tunnel%20setup.s.jpg" width = "100%">|An example on-site polytunnel and outbuiding configuration|
|---|:--|

To conveniently facilitate local off-site R&D activities, a small greenhouse 'lab' was constructed, based upon a <a href="https://www.quickcrop.co.uk/product/vegtrug-tomato-greenhouse"> 'VegTrug' Tomato greenhouse</a>, a free-standing timber structure having a small footprint, a constraint due its unusual location within the cockpit of a dry-docked sailing yacht.

|<image src = "images/Vegtrug frame with cover.jpg" width = "50%">|The basic 'VegTrug' frame prior to equipping with monitoring and control systems.|
|---|:--|

# System Overview


|   |The system structure is based on a star network,  each host (polytunnel) having its own data acquisition, communications and optionally, control capabilities. Hosts beyond WiFi range report to a central hub using low-power wide-area network (LPWAN) protocols.  The hub, in turn is internet connected via a WiFi router to a public (secured)  SQL database server consolidating all host-originated parametric measurements. Where autonomous control is required local measurements are used to control various aspects of the Polytunnel environments whilst the remote database may be queried for analytical purposes. |
|---|:--|

|<image src="images/ESP8266%20autonomous%20data%20acquisition%2C%20telemetry%20and%20control.jpg" width="120%">|A data acquisition, WEB server, client and controller module. Serves an interactive monitoring and control GUI on the local network (below left code <a href="code/vegtrug_monitor8_debug_git.ino">here </a>) and uploads parametric measurements via a PHP interface (code <a href="code/post-data.php">here </a>) to a public (secured) SQL server (see sample database data list and derived chart below centre and right). |
|---|:--|
	
|<image src = "web/VegTrug%20Control%20Panel.jpg" width="50%">|   |   |
|---|---|---|
