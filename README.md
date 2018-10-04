# ATTiny84 V-USB Business card

This project contains KiCad schematics and board layouts, modified versions of the RF24 nRF24L01+ library (https://github.com/nRF24/RF24) and Adafruit's TrinketHidCombo library (https://github.com/adafruit/Adafruit-Trinket-USB/tree/master/TrinketHidCombo) to work with the ATTiny84 with a 16mhz crystal. 

The TrinketHidCombo needed to be reconfigured for the different registers on the T84 compared to the T85 and the RF24 library needed to be made compatible with clockwise pin definitions(a la damellis core).

The microusb connector footprint used is a modified version of one found in Sparkfun's KiCAD connector library. Thanks! (https://github.com/sparkfun/SparkFun-KiCad-Libraries) 

# So what does it do?
It's my business card. It also doubles as a development board. It successfully fits code to accept commands wirelessly to control onboard(or external) WS2812B LEDs and send Play/Pause/Vol up/Vol down/Stop media keys via USB, into the 8192 bytes available on an ATTiny84. Hardware for sending the RF commands is not covered by this project. 
