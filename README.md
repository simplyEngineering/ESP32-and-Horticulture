# ESP32 and ESP8266 microcontrollers in a horticultural setting - a working example.

# The Project

Whatever the crop, balancing yield and quality against cost and time  demands tight control of growing conditions. However, such control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, processing, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies experimental hardware and software elements employed at various points in the project over a considerable period. Photographs of prototype hardware, screenshots of typical database output, WEB pages, control/monitoring GUIs and code listings are presented (with attributions where appropriate). 

<b>Whilst these elements are valid in their own right they are illustrative only and it should not be assumed that they are either complete or directly relateable.  However, I would be delighted to provide any further explanatory information on request.</b>

<a href="mailto://stuart@ceng.me.uk?subject=ESP32 and Horticulture">Contact Stuart A Spray C.Eng MIEE</a>
<br><br>
|   |   |
|---|:--|	
|<image src = "images/ESP32 and ESP8266 variants.jpg" width = "100%">|<b>Examples of the microcontrollers used from left to right (typical specs):</b> <br><b>ESP32-CAM:</b>ESP32-S chip, 520k SRAM, 4M PSRAM, 2MP OV2640 camera, microSD card, 10 accessible GPIOs and power pins, on-board WiFi antenna and IPEX connector for external antenna.<br><b>ESP8266(NodeMCU):</b>Single core 32-bit processor, 128k RAM, 4MB flash, 17 GPIO pins, I2C/SPI, 10-bit ADC, WiFi<br><b>Heltec WiFi kit 32:</b>Dual-core 32-bit processor, 448k ROM, 520k SRAM, WiFi, Blutooth, 128x64 OLED display, I2C/SPI, 34 GPIO, 2x8-bit DAC, up to 18 12-bit ADC (GPIO shared)<br><b>Heltec WiFi LoRa 32:</b>Similar to WiFi kit 32 + LoRa Wide Area Network capability.|
	
For the purpose of this project and with the cooperation of a local tomato grower having  polytunnels scattered over many hectares, a field operation was set up so that selected tunnels could be set up with appropriate monitoring and communication equipment (see later for details).

|   |   |
|---|:--|
|<image src = "images/typical%20small%20scale%20tunnel%20setup.s.jpg" width = "100%">|An example on-site polytunnel and outbuiding configuration|


|System Overview|
|:--|
|The system structure is based on a star/hub network,  each host (polytunnel) being equipped with its own data acquisition, communications and optionally, control capabilities. Hosts within WiFi range (local hosts) communicate directly  with the hub via WiFi whilst those beyond WiFi range (remote hosts) report to a central hub using low-power wide-area network <a href="https://en.wikipedia.org/wiki/LPWAN">LPWAN</a> protocols.  It should be noted that the <a href="https://www.espressif.com/en/products/socs/esp32">ESP32 series</a> of microcontrollers provide all the necessary LPWAN and WiFi hardware and software so are particuularly well suited to this application, distances of 1 - 10 kM being regularly achievable. The hub acts as a gateway providing an internet path via a WiFi router (landline or wireless) to an SQL database server where all host-originated parametric measurements are consolidated and available for off-line analytical purposes. |
<br><br>
# The Modules
<table>
	<tr>
		<td><b>Data acquisition and LoRa transmitter Module - remote host .....</b></td><td><b>..... Internal</b></td>
	</tr>
	<tr>
		<td><image src="images/mk2%20sensors%20lora%20transmitter.jpg"></td>
		<td><image src="images/mk2%20sensors%20lora%20transmitter%20internal.jpg"></td>	
	</tr>
	<tr>
		<td><image width = "50%" src="images/mk2%20sensors%20lora%20transmitter%20in_situ.jpg"><br><b>In-situ</b></td>
		<td><image src="images/weigh%20platform.s.jpg"><br><b>Load cell platform</b></td>	
	</tr>
	<tr>
		<td colspan="2">This module adopts a Heltec WiFi LoRa 32 (V2)  microcontroller running two cores at 240MHz with 8Mb flash and 320kb RAM.
		It has a self-contained micro-USB serial port for uploading and may be used under either the Espressif IoT Development or Arduino frameworks.
		Power management is on-board (can interface directly with a LiPo) and communicates via conventional WiFi and LoRa at 868MHz. 30+ GPIO pins are available with 			I2C/SPI busses. Its 128x64 OLED display is redundant for this module.
		The primary purpose of this module is to acquire temperature, relative humidity, light, atmospheric pressure and weight measurements, assemble the data into packets and transmit them over a LoRa link to its receiver counterpart at the gateway. The loadcell platform uses a pair of 10kG strain guages linked to a bridge amplifier and 16-bit ADC via an I2C bus.  One of the strain guages can just be seen in the photograph, sandwiched between the upper and lower platforms, the jack screws being used for levelling purposes. See code <a href="code/dataLogger6_noSleep3_compensate3_noCO2_git.ino">here</a></td>
	</tr>
</table>
<br><br>
<table>
	<tr>
		<td><b>LoRa Receiver and Gateway Module.....</b></td><td><b>..... Internal</b></td>
	</tr>
	<tr>
		<td><image width="70%" src="images/mk2%20%20lora%20receiver%20wifi%20client.jpg"></td>
		<td><image src="images/mk2%20%20lora%20receiver%20wifi%20client%20internal.jpg"></td>
	</tr>
	<tr>	
		<td colspan="2">This module use the same hardware as the corresponding transmitter, but without the transducers and  utilising the OLED display and WiFi capability.  It acts as a Gateway to an SQL database server by receiving data packets over the LoRa link from the transmitter and forwarding them via a WiFi client connection to a local Internet Router (landline or Wireless 4/5G).  Visible on the OLED display are the temperature, pressure, humidity, light and weight measurements as well as some supervisory data including the RSSI (Radio Signal Strength Indicator), Battery voltage, Message ID (a transmitter generated serial number to identify missing packets), a 'Wait' countdown timer to indicate system activity  and a 'Post' counter to indicate the number of previous attempts to post the message. (an indicator of link quality).  See code <a href="code/LoRaReceiver_T_git.ino">here.</td>
	</tr>
</table>

<br><br>

|Data acquisition, WEB server, WEB client and controller module - local host|
|:--|
|<image src="images/ESP8266%20autonomous%20data%20acquisition%2C%20telemetry%20and%20control.jpg" width="120%">|
|Based on a single ESP8266 acquiring temperature, relative humidity, atmospheric pressure, light and weight via a I2C bus with CO2 measurements made by interrupt driven PWM techniques.  (See <a href="https://github.com/simplyEngineering/MHZ-14-PWM-by-Interrupt">MHZ-14-PWM-by-Interrupt</a> repository). The weight parameter is used as an analogue of water content, used by the weight control loop. Noise reduction of the weight measurement is carried out by  digital filtering techniques (see code listings <a href="https://github.com/simplyEngineering/Loadcell-Cleaning-the-output">Loadcell-Cleaning-the-output</a> repository.  
The WEB server component of the ESP8266 generates an interactive monitoring and control GUI on the local WiFi network (below left and see code <a href="code/vegtrug_monitor8_debug_git.ino">here </a>), uploading parametric measurements via a PHP interface (code <a href="code/post-data.php">here </a>) to a public SQL server (see sample database data list below centre). SQL data is subsequently accessed offline via a PHP page (see <a href="web/get_data.php"> code</a>) for analytical purposes and to form the basis of various javascript-driven charts: see example output below right and page-code listing <a href="web/indexDiffCompare9.html">here</a>. 
Bang-bang control loops are provided for temperature, humidity, weight (pump control) and ventilation, their outputs being interfaced by low-power isolating relays.|
<table>
	<tr><th>Interactive GUI</th><th>Sample SQL data</th><th>Sample derived chart</th></tr>
<tr>
	<td><image src ="web/VegTrug%20Control%20Panel.jpg" width="100%"></td>
	<td><image src="web/SQL%20data%20list.png" width = "100%"></td>
	<td><image src="web/period%20cost%20calcs2.jpg" width = "100%"></td>
</tr>
	</table>
<br><br>
	
To conveniently facilitate local off-site R&D activities, a small greenhouse 'lab' was constructed, based upon a <a href="https://www.quickcrop.co.uk/product/vegtrug-tomato-greenhouse"> 'VegTrug' Tomato greenhouse</a>, a free-standing timber structure having a small footprint, a constraint due its unusual location within the cockpit of a dry-docked sailing yacht.
<table>
<tr>
	<th>The basic 'VegTrug' frame.</th>
	<th>The 'VegTrug' frame in the cockpit (looking aft).</th>
	<th>The 'VegTrug' frame showing ventilation pipes.</th>
</tr>
<tr>
	<td><image src = "images/Vegtrug frame with cover.jpg" width = "100%"></td>
	<td><image src = "images/VegTrug%20in%20situ%20(cockpit).jpg" width = "100%"></td>
	<td><image src = "images/VegTrug%20prior%20to%20installation%20showing%20ventilation%20pipes.jpg" width = "100%"></td>
</tr>
</table>
<br><br>

## Ancillary modules

<table>
<tr>
	<th>A simple stand-alone temperature/humidity/pressure/WEB server module</th>
	<th>Stand-alone WEB page</th>
	
</tr>
<tr>
	<td><image src = "images/ESP8266%20BMP280%20I2C.jpg" width = "100%"></td>
	<td><image src = "images/standalone%20module%20WEB%20page.png" width = "100%"></td>
<tr>
	<td colspan = "2">This is about the  simplest stand-alone ESP8266 environmental monitor which uses the BMP280 multi-transducer device on an I2C bus. It acts as a WEB server on the local network, generating  graphical data through web-socket protocols, avoiding complete re-writes of the web page each time the data is updated.  
		See code <a href = "code/ACROBOTIC_plot_sensor_data9_git.ino">here.</a>The blank pressure graph is due to the fact that at the time of publication the pressure was off-scale!</td>
</tr>
</table>


# Watch this space for further material
