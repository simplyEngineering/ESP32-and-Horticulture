# ESP32 and ESP8266 microcontrollers in horticulture <br>- a working example.

<table><tr><td colspan="2">
	
## Contents
</td></tr>
<tr><td>
<ul>
<li><a href="#project">The Project</a></li>
<li><a href="#microcontrollers">Microcontrollers</a></li>
<li><a href="#system">System overview</li></a></li></ul>
</td>
<td><a href="#modules">Modules</a>
<ul>
<li><a href="#remote">Remote Host</a></li>
<li><a href="#gateway">Lora Receiver/Gateway</a></li>
<li><a href="#local">Local Host</a></li>
<li><a href="#transducer">Stand-alone transducer/WEB server</a></li>
<li><a href="#data">Data display</a></li>
<li><a href="#camera">Camera</a></li>
</td></tr>
<a name="project">
	
### The Project
<table><tr><td>
Whatever the crop, balancing yield and quality against cost and time  mandates tight control of growing conditions. However, such control cannot be achieved without an intimate knowledge of the growing environment, demanding measurement, processing, data-logging, display, control and communication sub-systems for which the low-cost ESP32 and ESP8266 devices are very well suited.

This document identfies experimental hardware and software elements employed at various points in the project over an extended period. Photographs of prototype hardware, screenshots of typical database output, WEB pages, control/monitoring GUIs and code listings are presented (with attributions where appropriate). 

<b>Whilst these elements are valid in their own right they are illustrative only and it should not be assumed that they are either complete or directly relateable.  However, I would be delighted to provide any further explanatory information on request.</b></td></tr></table>

<a href="mailto://stuart@ceng.me.uk?subject=ESP32 and Horticulture">Contact Stuart A Spray C.Eng MIEE</a>
<br><br>
<a name="microcontrollers"></a>
<table>
<tr>
	<th colspan = "2">Examples of the microcontrollers used from left to right (typical specs)</th>
</tr>
<tr>
	<td><image src = "images/ESP32 and ESP8266 variants.jpg"></td>
	<td width="50%"><b>ESP32-CAM:</b>ESP32-S chip, 520k SRAM, 4M PSRAM, 2MP OV2640 camera, microSD card, 10 accessible GPIOs and power pins, on-board WiFi antenna and IPEX connector for external antenna.<br><b>ESP8266(NodeMCU):</b>Single core 32-bit processor, 128k RAM, 4MB flash, 17 GPIO pins, I2C/SPI, 10-bit ADC, WiFi<br><b>Heltec WiFi kit 32:</b>Dual-core 32-bit processor, 448k ROM, 520k SRAM, WiFi, Blutooth, 128x64 OLED display, I2C/SPI, 34 GPIO, 2x8-bit DAC, up to 18 12-bit ADC (GPIO shared)<br><b>Heltec WiFi LoRa 32:</b>Similar to WiFi kit 32 + LoRa Wide Area Network capability.
	</td>
</tr>
</table>
<a name="system"></a>
	
### System Overview
<table>
	<tr><td>The system structure is based on a star/hub network,  each host (polytunnel) being equipped with its own data acquisition, communications and optional control capabilities. Hosts within WiFi range (local hosts) communicate directly  with the hub via WiFi whilst those beyond WiFi range (remote hosts) report to a central hub using low-power wide-area network <a href="https://en.wikipedia.org/wiki/LPWAN">LPWAN</a> protocols.  It should be noted that the <a href="https://www.espressif.com/en/products/socs/esp32">ESP32 series</a> of microcontrollers provide all the necessary LPWAN and WiFi hardware and software so are particuularly well suited to this application, distances of 1 - 10 kM being regularly achievable. The hub acts as a gateway providing an internet path via a WiFi router (landline or wireless) to an SQL database server where all host-originated parametric measurements are consolidated and available for off-line analytical purposes.<br><br></td></tr>
	<tr><td>For the purpose of this project and with the cooperation of a local tomato grower having  polytunnels scattered over many hectares, a field operation was set up so that selected tunnels could be set up with appropriate monitoring and communication equipment.<br><br>To conveniently facilitate local off-site R&D  for this project a small greenhouse 'lab' was constructed (the <b>'VegBot'</b>), described in the related <a href="https://github.com/simplyEngineering/ESP32-and-Horticulture-R-D-Lab">ESP32 R&D Lab</a> repository.
<br></td><tr>
</table>
<table>
	<tr><th>An example on-site polytunnel and outbuiding configuration</th></tr>
	<tr><td><image src = "images/typical%20small%20scale%20tunnel%20setup.s.jpg"></td></tr>
<table>
<br><br>
<a name="modules">
	
## The Modules
<table>
	<a name="remote">
	<tr><th>A Remote Host</th><th>Internal</th></tr>
	<tr>
		<td><image src="images/mk2%20sensors%20lora%20transmitter.jpg"></td>
		<td><image src="images/mk2%20sensors%20lora%20transmitter%20internal.jpg"></td>	
	</tr>
	<tr><th>In-situ</th><th>Load cell platform</th></tr>
	<tr>
		<td><image width = "50%" src="images/mk2%20sensors%20lora%20transmitter%20in_situ.jpg"></td>
		<td><image src="images/weigh%20platform.s.jpg"</td>	
	</tr>
	<tr>
		<td colspan="2">This module adopts a <a href="https://docs.platformio.org/en/latest/boards/espressif32/heltec_wifi_lora_32_V2.html">Heltec WiFi LoRa 32 (V2)</a>  microcontroller running two cores at 240MHz with 8Mb flash and 320kb RAM.
		It has a self-contained micro-USB serial port for uploading and may be used under either the <a href="https://github.com/espressif/esp-idf">Espressif IoT Development</a> or <a href="https://docs.platformio.org/en/latest/tutorials/espressif32/arduino_debugging_unit_testing.html#tutorial-espressif32-arduino-debugging-unit-testing">Arduino</a> frameworks.
		Power management is on-board (can interface directly with a LiPo) and communicates via conventional WiFi and LoRa at 868MHz. 30+ GPIO pins are available with 			I2C/SPI busses. Its 128x64 OLED display is redundant for this module.
		The primary purpose of this module is to acquire temperature, relative humidity, light, atmospheric pressure and weight measurements, assemble the data into packets and transmit them over a LoRa link to its receiver counterpart at the gateway. The loadcell platform uses a pair of 10kG strain guages linked to a bridge amplifier and 16-bit ADC via an I2C bus.  One of the strain guages can just be seen in the photograph, sandwiched between the upper and lower platforms, the jack screws being used for levelling purposes. See code <a href="code/dataLogger6_noSleep3_compensate3_noCO2_git.ino">here</a></td>
	</tr>
</table>
<br>
<table>
	<tr>
		<a name="gateway">
		<td><b>LoRa Receiver and Gateway Module.....</b></td><td><b>..... Internal</b></td>
	</tr>
	<tr>
		<td><image width="70%" src="images/mk2%20%20lora%20receiver%20wifi%20client.jpg"></td>
		<td><image src="images/mk2%20%20lora%20receiver%20wifi%20client%20internal.jpg"></td>
	</tr>
	<tr>	
		<td colspan="2">This module uses the same hardware as the corresponding transmitter, but without the transducers and  utilising the OLED display and WiFi capability.  It acts as a Gateway to an SQL database server by receiving data packets over the LoRa link from the transmitter and forwarding them via a WiFi client connection to a local Internet Router (landline or Wireless 4/5G).  Visible on the OLED display are the temperature, pressure, humidity, light and weight measurements as well as some supervisory data including the RSSI (Radio Signal Strength Indicator), Battery voltage, Message ID (a transmitter generated serial number to identify missing packets), a 'Wait' countdown timer to indicate system activity  and a 'Post' counter to indicate the number of previous attempts to post the message. (an indicator of link quality).  See code <a href="code/LoRaReceiver_T_git.ino">here.</td></tr>
</table>

<table>
	<a name="local">
	<tr><th colspan="2">A Local Host</th></tr>
	<tr><td colspan="2"><image src="images/dataAcquisitionCommunicationsOutputModule.jpg"></td></tr>
	<tr><td width = "50%">The local host is based on a single <a href="https://en.wikipedia.org/wiki/NodeMCU">NodeMCU ESP8266</a> acquiring temperature, relative humidity, atmospheric pressure, light and weight via a I2C bus. CO2 measurements are made by interrupt driven PWM techniques.  (See <a href="https://github.com/simplyEngineering/MHZ-14-PWM-by-Interrupt">MHZ-14-PWM-by-Interrupt</a> repository). It might seem strange to be measuring weight but this parameter is used as an analogue of water content/dryness of the growing medium and thus a convenient way of controlling irrigation via the pump control loop. The fan ensures that the transducers are flooded with ambient air and not just responding to local board conditions.<br><br>Load cell noise reduction of the weight measurement is carried out by  digital filtering techniques (see code listings <a href="https://github.com/simplyEngineering/Loadcell-Cleaning-the-output">Loadcell-Cleaning-the-output</a> repository.  <br><br>Sub-optimal <a href="https://en.wikipedia.org/wiki/Bang%E2%80%93bang_control">Bang-bang</a> control loops are provided for temperature, humidity, weight (pump control) and ventilation, their outputs being interfaced by low-power isolating relays. The relays are interfaced to the ESP8266 output ports via 2n3904 NPN emitter followers (Historical note: 2n3904s have been around since the late 60's!)<br><br>
The WEB server component of the ESP8266 generates an interactive monitoring and control GUI on the local WiFi network (see code <a href="code/vegtrug_monitor8_debug_git.ino">here </a>), uploading parametric measurements via a PHP interface (code <a href="code/post-data.php">here </a>) to a public SQL server (see sample database data list below left). SQL data is subsequently accessed offline via a PHP page (see <a href="web/get_data.php"> code</a>) for analytical purposes and to form the basis of various javascript-driven charts: see example output below right and page-code listing <a href="web/indexDiffCompare9.html">here</a>. 
		</td><td><b> Local Host GUI</b><br><image src ="images/VegBotControlPOanel.png"><br><br><b>CAUTION!</b> Whilst the ESP32/8266 microntroller is fully capable of delivering all the services in this module, due consideration should be given to their potential disruptive mutual interactions. For example, the WEB server GUI may be stimulated into activity by a client connexion at any arbitrary time, just when one of the control loops is commanded into action by a transducer measurement.  The outcome may be quite benign but is at the very least unpredictable and at worst may cause a system hangup or dangerous output. This has been an observed condition in this module and warrants a better design approach such as real-time task scheduling such as found in <a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html">FreeRTOS</a>. Further work is required here. </td></tr>
<tr><th>Sample SQL data</th><th>Sample derived chart</th></tr>
<tr>
	<td><image src="web/SQL%20data%20list.png"></td>
	<td><image src="web/period%20cost%20calcs2.jpg"></td>
</tr>

<br>

<table>
<tr>
	<a name="transducer">
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
<br>
<table>
<tr>
	<a name="data">
	<th colspan="2">A data display module</th>
</tr>
<tr>
	<td><image src = "images/ESP32%20OLED%20HTML%20scrape.jpg" ></td>
	<td width = "40%">However a WEB page is originated, the raw data embedded within its HTML code can always be extracted and represented differently.  For example, the raw data embedded in the above WEB page can be analysed or 'scraped' so as to make it available through an alternative interface. <br>The Heltec WiFi kit 32 (seen left atop a 'PowerAdd' battery) with its on-board 128x64 OLED display is a suitable candidate for the application of this technique, where selected parameters are 'scraped' from the HTML code (which contains more than just the BMP280 data)  and represented directly on the display. <br>The particular beauty here lies in its simplicity of hardware: the ESP32 being the only component required.  
See code <a href="code/HTTP_Get_request_for_data_scraping9_RTOS_httpclient_git.ino">here</a></td>
</tr>
</table>
<br><br>
<table>
	<a name="camera">
	<tr><th colspan="2">The Camera Module</th></tr>
	<tr><td colspan="2"><image src="images/cameraModule.jpg"></td></tr>
	<tr><td><b>Seeing is believing!</b><br><br> To supplant the data acquisition an ESP32-cam module (upper right) acquires a 1600 x 1200 jpg image every 30 minutes.  Using its WiFi client capabilities it uploads it to a public file server for viewing.<br><br> Having no intrinsic USB serial port a generic FTDI interface (lower right) is included on the board for convenience. The circular component is a 0.22F supercapacitor without which this camera may 'brownout' the supply due to a high level of surge current during data acquisition. <br><br>The relay module(centre left) is used to switch power to a 20W LED batten luminaire during the acquisition cycle.</td><td width="75%"><image src = "images/vegcam1.jpg"><br>The uploaded camera image is overlayed with a data panel by means of an HTML iFrame, its data source being extracted from the SQL data server sourced from a WiFi local host or LoRa Receiver/Gateway module.</td></tr>
</table>

# This is an active project - watch this space for further material<br><br>
## See also the <a href="https://github.com/simplyEngineering/ESP32-and-Horticulture-R-D-Lab"> ESP32 and Horticulture R&D Lab</a> related repository<br><br>
### Enquiries to:&nbsp;&nbsp;<a href="mailto://stuart@ceng.me.uk?subject=ESP32 and Horticulture">Stuart A Spray C.Eng MIEE</a>
