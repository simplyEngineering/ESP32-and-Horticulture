# ESP32 and ESP8266 microcontrollers in horticulture - a working example.

# The Project

Whatever the crop, balancing yield and quality against cost and time  mandates tight control of growing conditions. However, such control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, processing, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies experimental hardware and software elements employed at various points in the project over an extended period. Photographs of prototype hardware, screenshots of typical database output, WEB pages, control/monitoring GUIs and code listings are presented (with attributions where appropriate). 

<b>Whilst these elements are valid in their own right they are illustrative only and it should not be assumed that they are either complete or directly relateable.  However, I would be delighted to provide any further explanatory information on request.</b>

<a href="mailto://stuart@ceng.me.uk?subject=ESP32 and Horticulture">Contact Stuart A Spray C.Eng MIEE</a>
<br><br>
<table>
<tr>
	<th colspan = "2">Examples of the microcontrollers used from left to right (typical specs)</th>
</tr>
<tr>
	<td><image src = "images/ESP32 and ESP8266 variants.jpg"></td>
	<td width="50%"><b>ESP32-CAM:</b>ESP32-S chip, 520k SRAM, 4M PSRAM, 2MP OV2640 camera, microSD card, 10 accessible GPIOs and power pins, on-board WiFi antenna and IPEX connector for external antenna.<br><b>ESP8266(NodeMCU):</b>Single core 32-bit processor, 128k RAM, 4MB flash, 17 GPIO pins, I2C/SPI, 10-bit ADC, WiFi<br><b>Heltec WiFi kit 32:</b>Dual-core 32-bit processor, 448k ROM, 520k SRAM, WiFi, Blutooth, 128x64 OLED display, I2C/SPI, 34 GPIO, 2x8-bit DAC, up to 18 12-bit ADC (GPIO shared)<br><b>Heltec WiFi LoRa 32:</b>Similar to WiFi kit 32 + LoRa Wide Area Network capability.
	</td>
</tr>
<br><br>

<table>
	<tr><th>System Overview</th></tr>
	<tr><td>The system structure is based on a star/hub network,  each host (polytunnel) being equipped with its own data acquisition, communications and optional control capabilities. Hosts within WiFi range (local hosts) communicate directly  with the hub via WiFi whilst those beyond WiFi range (remote hosts) report to a central hub using low-power wide-area network <a href="https://en.wikipedia.org/wiki/LPWAN">LPWAN</a> protocols.  It should be noted that the <a href="https://www.espressif.com/en/products/socs/esp32">ESP32 series</a> of microcontrollers provide all the necessary LPWAN and WiFi hardware and software so are particuularly well suited to this application, distances of 1 - 10 kM being regularly achievable. The hub acts as a gateway providing an internet path via a WiFi router (landline or wireless) to an SQL database server where all host-originated parametric measurements are consolidated and available for off-line analytical purposes.</td></tr>
	<tr><td>For the purpose of this project and with the cooperation of a local tomato grower having  polytunnels scattered over many hectares, a field operation was set up so that selected tunnels could be set up with appropriate monitoring and communication equipment.</td><tr>
</table>
<table>
	<tr><th>An example on-site polytunnel and outbuiding configuration</th></tr>
	<tr><td><image src = "images/typical%20small%20scale%20tunnel%20setup.s.jpg"></td></tr>
<table>
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
		<td colspan="2">This module adopts a <a href="https://docs.platformio.org/en/latest/boards/espressif32/heltec_wifi_lora_32_V2.html">Heltec WiFi LoRa 32 (V2)</a>  microcontroller running two cores at 240MHz with 8Mb flash and 320kb RAM.
		It has a self-contained micro-USB serial port for uploading and may be used under either the <a href="https://github.com/espressif/esp-idf">Espressif IoT Development</a> or <a href="https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html#tutorial-espressif32-arduino-debugging-unit-testing">Arduino</a> frameworks.
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
		<td colspan="2">This module use the same hardware as the corresponding transmitter, but without the transducers and  utilising the OLED display and WiFi capability.  It acts as a Gateway to an SQL database server by receiving data packets over the LoRa link from the transmitter and forwarding them via a WiFi client connection to a local Internet Router (landline or Wireless 4/5G).  Visible on the OLED display are the temperature, pressure, humidity, light and weight measurements as well as some supervisory data including the RSSI (Radio Signal Strength Indicator), Battery voltage, Message ID (a transmitter generated serial number to identify missing packets), a 'Wait' countdown timer to indicate system activity  and a 'Post' counter to indicate the number of previous attempts to post the message. (an indicator of link quality).  See code <a href="code/LoRaReceiver_T_git.ino">here.</td></tr>
</table>

<br><br>
<table>
	<tr><th>Master module - local host</th></tr>
	<tr><td><image src="images/ESP8266%20autonomous%20data%20acquisition%2C%20telemetry%20and%20control.jpg"></td></tr>
	<tr><td>Based on a single <a href="https://en.wikipedia.org/wiki/NodeMCU">NodeMCU ESP8266</a> acquiring temperature, relative humidity, atmospheric pressure, light and weight via a I2C bus with CO2 measurements made by interrupt driven PWM techniques.  (See <a href="https://github.com/simplyEngineering/MHZ-14-PWM-by-Interrupt">MHZ-14-PWM-by-Interrupt</a> repository). The weight parameter is used as an analogue of water content, used by the weight control loop. Noise reduction of the weight measurement is carried out by  digital filtering techniques (see code listings <a href="https://github.com/simplyEngineering/Loadcell-Cleaning-the-output">Loadcell-Cleaning-the-output</a> repository.  
The WEB server component of the ESP8266 generates an interactive monitoring and control GUI on the local WiFi network (below left and see code <a href="code/vegtrug_monitor8_debug_git.ino">here </a>), uploading parametric measurements via a PHP interface (code <a href="code/post-data.php">here </a>) to a public SQL server (see sample database data list below centre). SQL data is subsequently accessed offline via a PHP page (see <a href="web/get_data.php"> code</a>) for analytical purposes and to form the basis of various javascript-driven charts: see example output below right and page-code listing <a href="web/indexDiffCompare9.html">here</a>. 
Sub-optimal <a href="https://en.wikipedia.org/wiki/Bang%E2%80%93bang_control">Bang-bang</a> control loops are provided for temperature, humidity, weight (pump control) and ventilation, their outputs being interfaced by low-power isolating relays.<br><br><b>WARNING!</b>. Whilst the ESP32/8266 microntroller is fully capable of delivering all the services in this module, due consideration must be given to their potential disruptive mutual interactions. For example, the WEB server GUI may be stimulated into activity by a client connexion at any arbitrary time, just when one of the control loops is commanded into action by a transducer measurement.  The outcome may be quite benign but is at the very least unpredictable and at worst may cause a system hangup or dangerous output. This has been an observed condition in this module and warrants a better design approach such as real-time task scheduling such as found in <a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html">FreeRTOS</a>. Further work is required here. </td></tr>
<table>
	<tr><th>Interactive GUI</th><th>Sample SQL data</th><th>Sample derived chart</th></tr>
<tr>
	<td><image src ="web/VegTrug%20Control%20Panel.jpg" width="100%"></td>
	<td><image src="web/SQL%20data%20list.png" width = "100%"></td>
	<td><image src="web/period%20cost%20calcs2.jpg" width = "100%"></td>
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
	<td colspan = "2">This is about the  simplest stand-alone ESP8266 environmental monitor which uses the BMP280 multi-transducer device on an I2C bus. It acts as a WEB server on the local network, generating  time-based graphical data of Temperature, Relative Humidity and Pressure.  By adopting web-socket protocols,  complete re-writes of the page after each data update are avoided resulting in a smooth glitch free display.  
		See code <a href = "code/ACROBOTIC_plot_sensor_data9_git.ino">here.</a>The blank pressure graph is due to the fact that at the time of publication the pressure was off-scale!</td>
</tr>
</table>
<br><br>
<table>
<tr>
	<th colspan="2">A data display module</th>
</tr>
<tr>
	<td><image src = "images/ESP32%20OLED%20HTML%20scrape.jpg" ></td>
	<td width = "40%">However a WEB page is originated, the raw data embedded within its HTML code can always be extracted and represented differently.  For example, the raw data embedded in the above WEB page can be analysed or 'scraped' so as to make it available through an alternative interface. <br>The Heltec WiFi kit 32 (seen left atop a 'PowerAdd' battery) with its on-board 128x64 OLED display is a suitable candidate for the application of this technique, where selected parameters are 'scraped' from the HTML code (which contains more than just the BMP280 data)  and represented directly on the display. <br>The particular beauty here lies in its simplicity of hardware: the ESP32 being the only component required.  
See code <a href="code/HTTP_Get_request_for_data_scraping9_RTOS_httpclient_git.ino">here</a></td>
</tr>
</table>

<br><br>
# The 'VegBot' R&D lab
<br>
To conveniently facilitate local off-site R&D activities, a small greenhouse 'lab' was constructed, based upon a <a href="https://www.quickcrop.co.uk/product/vegtrug-tomato-greenhouse"> 'VegTrug' Tomato greenhouse</a>. This structure has a small footprint, alowing it to be  unusually located within the cockpit of 'Lena Mary', my dry-docked Moody 40 sailing yacht stripped of its steering gear.
<br>
<br>
<table>
<tr>
	<th width="50%">The basic 'VegBot' frame.</th>
	<th>The 'VegBot' frame in the cockpit (looking aft).</th>
</tr>
<tr>
	<td><image src = "images/Vegtrug frame with cover.jpg"></td>
	<td><image src = "images/VegTrug%20in%20situ%20(cockpit).jpg"></td>
</tr>
</table><br><br>

<table>
	<tr>
		<th colspan="2">Air quality control in the VegBot</th>
	</tr>
	<tr>
		<td width="50%">Air conditions within the VegBot are regulated under the control of the ESP8266 Master module. Minimum temperature is established by a heater control loop which is activated whenever the actual temperature falls below the  setpoint temperature by a set margin (nominally 1 degree). Deactivation occurs whenever the actual temperature exceeds the setpoint temperature by more than the same set margin. Should the actual temperature exceed the solar gain margin (nominally 3 degrees) even when the heater is off as would be the case under summer conditions, induction and extractor fans are activated to introduce colder ambient air from and exhaust warm air to the outside space under the VegBot.<br><br>Irrespective of the temperature control status, the induction and exhaust fans are activated to ensure that the air in the VegBot is completely changed at least once every 4 minutes.<br><br>Maintaining a uniform temperature throughout the VegBot is only posible if the enclosed air is continuously recirculated. The recirculation fan at the head of the reciculation pipe lifts heated air from below the centre shelf and pushes it out into the space above the top shelf. From here it cools and by gravitation moves back down into the lower regions of the VegBot.</td>
		<td><image src="images/VegTrug%20prior%20to%20installation%20showing%20ventilation%20pipes.jpg"><br>Ventilation pipe (rear) and recirculation pipe (front).</td>
	</tr>
	<tr>
		<td><image src="images/inletCircFan.jpg"><br>Ambient air induction fan and recirculation fan atop the ventilation and recirculation pipes respectively.</td>
		<td><image src="images/ExtractorFan.jpg"><br>Extractor fan in base of VegBot.</td>
	</tr>
	<tr>
		<td><image src = "images/humidifierHeater.jpg" width = "100%"></td>
		<td><br>The heater  (right) is a 500W ceramic element PTC device which is both fast heating and safe to use in an enclosed environment. The humidifier (left) is a generic 2.8l domestic device.  Both are controlled via isolating relays from the Master module. </td>
	</tr>
</table>

# Watch this space for further material
### <a href="mailto://stuart@ceng.me.uk?subject=ESP32 and Horticulture">Stuart A Spray C.Eng MIEE</a>
