# Employing ESP32 and ESP8266 microcontrollers in a horticultural setting - a working example.

# The Project

Whatever the crop, balancing yield and quality against cost and time  demands tight control of growing conditions. However, such control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, processing, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies experimental hardware and software elements employed at various points in the project over a considerable period. Photographs of prototype hardware, screenshots of typical database output, WEB pages, control/monitoring GUIs and code listings are presented (with attributions where appropriate). Whilst these elements are valid in their own right they are illustrative only and it should not be asumed that they directly relateable.  However, I would be delighted to provide any further explanatory information on request.

<a href="mailto://stuart@ceng.me.uk?subject=ESP32 and Horticulture">Contact Stuart A Spray C.Eng MIEE</a>

|   |   |
|---|:--|	
|<image src = "images/ESP32 and ESP8266 variants.jpg" width = "100%">|<b>Examples of the microcontrollers used from left to right (typical specs):</b> <br><b>ESP32-CAM:</b>ESP32-S chip, 520k SRAM, 4M PSRAM, 2MP OV2640 camera, microSD card, 10 accessible GPIOs and power pins, on-board WiFi antenna and IPEX connector for external antenna.<br><b>ESP8266(NodeMCU):</b>Single core 32-bit processor, 128k RAM, 4MB flash, 17 GPIO pins, I2C/SPI, 10-bit ADC, WiFi<br><b>Heltec WiFi kit 32:</b>Dual-core 32-bit processor, 448k ROM, 520k SRAM, WiFi, Blutooth, 128x64 OLED display, I2C/SPI, 34 GPIO, 2x8-bit DAC, up to 18 12-bit ADC (GPIO shared)<br><b>Heltec WiFi LoRa 32:</b>Similar to WiFi kit 32 + LoRa Wide Area Network capability.|
	
For the purpose of this project and with the cooperation of a local tomato grower having  polytunnels scattered over many hectares, a field operation was set up so that selected tunnels could be set up with appropriate monitoring and communication equipment (see later for details).

|   |   |
|---|:--|
|<image src = "images/typical%20small%20scale%20tunnel%20setup.s.jpg" width = "100%">|An example on-site polytunnel and outbuiding configuration|
	
To conveniently facilitate local off-site R&D activities, a small greenhouse 'lab' was constructed, based upon a <a href="https://www.quickcrop.co.uk/product/vegtrug-tomato-greenhouse"> 'VegTrug' Tomato greenhouse</a>, a free-standing timber structure having a small footprint, a constraint due its unusual location within the cockpit of a dry-docked sailing yacht.

|   |   |
|---|:--|
|<image src = "images/Vegtrug frame with cover.jpg" width = "50%">|The basic 'VegTrug' frame prior to equipping with monitoring and control systems.|


|System Overview|
|:--|
|The system structure is based on a star network,  each host (polytunnel) being equipped with its own data acquisition, communications and optionally, control capabilities. Hosts beyond WiFi range report to a central gateway using low-power wide-area network <a href="https://en.wikipedia.org/wiki/LPWAN">LPWAN</a> protocols.  It should be noted that the <a href="https://www.espressif.com/en/products/socs/esp32">ESP32 series</a> of microcontrollers provide all the necessary LPWAN and WiFi hardware and software so are particuularly well suited to this application, distances of 1 - 10 kM being regularly achievable. The gateway provides an internet path via a WiFi router (landline or wireless) to a public  SQL database server where all host-originated parametric measurements are consolidated and available for off-line analytical purposes. |

# The Modules
<table>
	<tr>
		<td>**Data acquisition and LoRa transmitter Module.....**</td><td>**..... Int-situ**</td><td>**..... Internal**</td>
	</tr>
	<tr>
		<td><image src="images/mk2%20sensors%20lora%20transmitter.jpg"></td>
		<td><image src="images/mk2%20sensors%20lora%20transmitter%20in_situ.jpg"></td>
		<td><image src="images/mk2%20sensors%20lora%20transmitter%20internal.jpg"></td>	
	</tr>
	<tr>
		<td colspan="3">Some words here</td>
	</tr>
</table>


|Data acquisition, WEB server, WEB client and controller module|
|:--|
|<image src="images/ESP8266%20autonomous%20data%20acquisition%2C%20telemetry%20and%20control.jpg" width="120%">|
|Based on a single ESP8266 acquiring temperature, relative humidity, atmospheric pressure, light and weight via a I2C bus with CO2 measurements made by interrupt driven PWM techniques.  (See <a href="https://github.com/simplyEngineering/MHZ-14-PWM-by-Interrupt">MHZ-14-PWM-by-Interrupt</a> repository). The weight parameter is used as an analogue of water content, used by the weight control loop. Noise reduction of the weight measurement is carried out by  digital filtering techniques (see code listings <a href="https://github.com/simplyEngineering/HZ711-Cleaning-the-output">HZ711-Cleaning-the-output</a> repository.  
The WEB server component of the ESP8266 generates an interactive monitoring and control GUI on the local WiFi network (below left and see code <a href="code/vegtrug_monitor8_debug_git.ino">here </a>), uploading parametric measurements via a PHP interface (code <a href="code/post-data.php">here </a>) to a public SQL server (see sample database data list below centre). SQL data is subsequently accessed offline via a PHP page (see <a href="web/get_data.php"> code</a>) for analytical purposes and to form the basis of various javascript-driven charts: see example output below right and page-code listing <a href="web/indexDiffCompare9.html">here</a>. 
Bang-bang control loops are provided for temperature, humidity, weight (pump control) and ventilation, their outputs being interfaced by low-power isolating relays.|

|Interactive GUI|Sample SQL data|Sample derived chart|
|---|---|---|
|<image src = "web/VegTrug%20Control%20Panel.jpg" width="100%">|<image src="web/SQL%20data%20list.png" width = "100%">|<image src="web/period%20cost%20calcs2.jpg" width = "100%">|

