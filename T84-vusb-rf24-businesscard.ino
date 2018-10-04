#include <RF24.h> //My fork with a fix for ATTiny44/84
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define BIT(x) (0x01 << (x)) 
#define getBit(p,m) ((p) & BIT((m)))

#include <TrinketHidCombo.h> //My fork for ATTiny44/84

#define PIN 8
Adafruit_NeoPixel strip = Adafruit_NeoPixel(5, PIN, NEO_GRB + NEO_KHZ800);

#define LED_BUILTIN 7

uint8_t ledState = HIGH; //Status LED
unsigned long previousMillis = 0;
unsigned int freq = 1000; //LED blinking frequency
uint8_t mode = 1;
uint8_t isConnected;

//uint64_t pipe01 = 0xFAFBFCFDFELL;
uint64_t pipe02 = 0xFAFAFAFA02LL;
RF24 radio(2, 3);

typedef struct{
  byte mode;
  byte data;
  uint64_t address;
}
message;

message msg; 

void setup() {

pinMode(LED_BUILTIN, OUTPUT);
 
TrinketHidCombo.begin();
while (getBit(PORTA,7) == 0 && millis() < 2000) { //I hackish-ly set A7 high in usbdrv.c / usbDeviceRead(), since TrinkedHidCombo.isConnected() doesnt work for some reason.
stayAliveDelay(0);
}
if (millis() < 1999) { //If under 2 sec then we have USB.
  isConnected = 1;
  mode = 3; 
}

strip.begin();
//  strip.show(); // Initialize all pixels to 'off' //Scraping a few bytes
    
  radio.begin();
   radio.setPayloadSize(sizeof(msg));
   
//Error out if radio not detected 
#ifdef debug 
  if (!radio.isPVariant()) { //Easy way to detect if tranceiver correctly connected
  while (1) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(50);
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
  }
}
#endif

//Send out address for any remote to pick up when first powered on
#ifdef debug
 radio.stopListening();
  radio.setPayloadSize(sizeof(msg));
  radio.openWritingPipe(pipe01);
  msg.address = pipe02;
  msg.data = 100;
  msg.mode = 100;
bool ok = radio.write(&msg, sizeof(msg)); 
#endif
//Maybe do something if successfully sent.


//  radio.openReadingPipe(0, pipe01);
  radio.openReadingPipe(1, pipe02);
  radio.startListening();

  wdt_enable(WDTO_2S);// enable 2s watchdog timer
  WDTCSR &= ~(1<<WDE);
  WDTCSR |= (1<<WDIE);
    wdt_reset();
}

void loop() {
uint8_t pipe;
stayAliveDelay(0);

 if (radio.available(&pipe)) //Msg available?
  {
     while (radio.available()) {     
       radio.read(&msg, sizeof(msg)); //Get message
     }
  
//Parse message
    switch (msg.data) {
        case 1:
        mode = 1;
        if (isConnected) TrinketHidCombo.pressMultimediaKey(MMKEY_PLAYPAUSE);
        break;
        case 2:
        mode = 2;
        if (isConnected) TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_UP);
        break;
        case 3:
        mode = 3;
        if (isConnected) TrinketHidCombo.pressMultimediaKey(MMKEY_VOL_DOWN);
        break;
        case 4:
        mode = 4;
        if (isConnected) TrinketHidCombo.pressMultimediaKey(MMKEY_STOP);
        break;
        default:
       freq = 0;
 break;
    }
  }
switch (mode) {
              
        case 1:
        rainbow(10);        
        break;
        case 2:
        rainbowCycle(10);
        break;
        case 3:
        //Set to princess's color
        for(int i=0; i<strip.numPixels(); i++) {
          if (radio.available()) break;
        strip.setPixelColor(i, 255,0,255);
        }
        break;
        case 4:
        //Off
        for(int i=0; i<strip.numPixels(); i++) {        
        strip.setPixelColor(i, 0,0,0);
        if (radio.available()) break;
        freq = 0;
        }  
        default:
       freq = 0;
 break;
    }
    strip.show();
//Non-blocking blinking
  if (millis() - previousMillis >= freq) {
    // save the last time you blinked the LED
    previousMillis = millis();

    // if the LED is off turn it on and vice-versa:
      if (ledState == LOW && freq > 0) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }
    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }


}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if (radio.available()) break;
    strip.setPixelColor(i, c);
    strip.show();
    stayAliveDelay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    if (radio.available()) break;
    for(i=0; i<strip.numPixels(); i++) {
      if (radio.available()) break;
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    stayAliveDelay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    if (radio.available()) break;
    for(i=0; i< strip.numPixels(); i++) {
      if (radio.available()) break;
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    stayAliveDelay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void stayAliveDelay (long datime) {
  TrinketHidCombo.poll();
    wdt_reset();
  long starttime = millis();
  while (millis() - starttime < datime) {
    TrinketHidCombo.poll();
    wdt_reset();
  }
}

