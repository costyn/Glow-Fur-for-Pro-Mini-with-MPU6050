/*
   Heavily modified from https://learn.adafruit.com/animated-neopixel-gemma-glow-fur-scarf

   This code will not work on a Gemma, it's too big. It will work on any Atmel with at least 16K memory.

   Blame: Costyn van Dongen

   Future ideas:
   - choose 1 color, brightenall to max, then fade to min
   - heartbeat pulse
   - color rain https://www.youtube.com/watch?v=nHBImYTDZ9I
   - two "faders" moving back and forth
*/

#include <FastLED.h>
#include <TaskScheduler.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include <avr/pgmspace.h>
//#include <EEPROM.h>


// #define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x)       Serial.print (x)
#define DEBUG_PRINTDEC(x)    Serial.print (x, DEC)
#define DEBUG_PRINTLN(x)     Serial.println (x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTDEC(x)
#define DEBUG_PRINTLN(x)
#endif

#define LED_PIN     12   // which pin your Neopixels are connected to
#define NUM_LEDS    90   // how many LEDs you have
#define BRIGHTNESS 200  // 0-255, higher number is brighter. 
#define SATURATION 255   // 0-255, 0 is pure white, 255 is fully saturated color
#define STEPS        2   // How wide the bands of color are.  1 = more like a gradient, 10 = more like stripes
#define BUTTON_PIN   3   // button is connected to pin 2 and GND
#define COLOR_ORDER GRB  // Try mixing up the letters (RGB, GBR, BRG, etc) for a whole new world of color combinations
#define LOOPSTART 0

#define LEDMODE_SELECT_DEFAULT_INTERVAL 50  // default scheduling time for LEDMODESELECT
#define PALETTE_SPEED  30   // How fast the palette colors move.   Higher delay = slower movement.
#define FIRE_SPEED  85   // Fire Speed; delay in millseconds. Higher delay = slower movement.
#define CYLON_SPEED 25  // Cylon Speed; delay in millseconds. Higher delay = slower movement.
#define FADEGLITTER_SPEED 10  // Cylon Speed; delay in millseconds. Higher delay = slower movement.
#define DISCOGLITTER_SPEED 20  // Cylon Speed; delay in millseconds. Higher delay = slower movement.

CRGB leds[NUM_LEDS];
byte ledMode = 13 ; // Which mode do we start with
unsigned long lastButtonChange = 0; // button debounce timer.

const char *routines[] = {
  "rb",         // 0
  "rb_stripe",  // 1
  "ocean",      // 2
  "heat",       // 3
  "party",      // 4
  "cloud",      // 5
  "forest",     // 6
  "fire2012",   // 7
  "cylon",      // 8
  "cylonmulti", // 9
  "fglitter",   // 10
  "dglitter",   // 11
  "strobe",     // 12
  "flashbpm",   // 13
  "pulse",      // 14
  "pulsestatic",// 15
  //  "pulse2",     // 16
  //  "pulsesuck",  // 17
  "racers",     // 18
  "black"       // 19
};
#define NUMROUTINES (sizeof(routines)/sizeof(char *)) //array size  

/* Scheduler stuff */
void ledModeSelect() ; // prototype method
Scheduler runner;
Task taskLedModeSelect( LEDMODE_SELECT_DEFAULT_INTERVAL, TASK_FOREVER, &ledModeSelect); // routine which adds/removes tasks according to ledmode
//#define _TASK_SLEEP_ON_IDLE_RUN

// ==================================================================== //
// ===                      MPU6050 stuff                         ===== //
// ==================================================================== //


// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 mpu;

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer


int aaRealX = 0 ;
int aaRealY = 0 ;
int aaRealZ = 0 ;
int yprX = 0 ;
int yprY = 0 ;
int yprZ = 0 ;

void getDMPData() ; // prototype method
void getYPRAccel() ; // prototype method
Task taskGetDMPData( 1, TASK_FOREVER, &getDMPData);
Task taskGetYPRAccel( 10, TASK_FOREVER, &getYPRAccel);

#ifdef DEBUG
void printDebugging() ; // prototype method
Task taskPrintDebugging( 100, TASK_FOREVER, &printDebugging);
#endif






void setup() {
  delay( 1000 ); // power-up safety delay
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), shortKeyPress, RISING);

#ifdef DEBUG
  Serial.begin(115200) ;
  DEBUG_PRINT( F("Starting up. Numroutines = ")) ;
  DEBUG_PRINTLN( NUMROUTINES ) ;

#endif

  /* Start the scheduler */
  runner.init();
  runner.addTask(taskLedModeSelect);
  taskLedModeSelect.enable() ;

  // ==================================================================== //
  // ==================================================================== //

  // join I2C bus (I2Cdev library doesn't do this automatically)
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  mpu.initialize();
  devStatus = mpu.dmpInitialize();

  // Normal offsets
  mpu.setXAccelOffset(-487);
  mpu.setYAccelOffset(80);
  mpu.setZAccelOffset(1566); // 1688 factory default for my test chip
  mpu.setXGyroOffset(-38);
  mpu.setYGyroOffset(1);
  mpu.setZGyroOffset(-1);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();

  } else {
    // ERROR!
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    DEBUG_PRINT(F("DMP Initialization failed (code "));
    DEBUG_PRINT(devStatus);
    DEBUG_PRINTLN(F(")"));
  }

  runner.addTask(taskGetDMPData);
  runner.addTask(taskGetYPRAccel);
//  taskGetDMPData.enable() ;
  taskGetYPRAccel.enable() ;

#ifdef DEBUG
  runner.addTask(taskPrintDebugging);
  taskPrintDebugging.enable() ;
#endif

}

void loop() {
  runner.execute();
}






void ledModeSelect() {

  if ( ledMode >= 0 and ledMode <= 6 ) {
    FillLEDsFromPaletteColors() ;
    taskLedModeSelect.setInterval( PALETTE_SPEED ) ;

    // FastLED Fire2012 split down the middle, so the fire flows "down" from the neck of the scarf to the ends
  } else if ( strcmp(routines[ledMode], "fire2012") == 0 ) {
    Fire2012() ;
    taskLedModeSelect.setInterval( FIRE_SPEED ) ;

    // Cylon / KITT / Larson scanner with fading tail and slowly changing color
  } else if ( strcmp(routines[ledMode], "cylon") == 0 ) {
    taskLedModeSelect.setInterval( CYLON_SPEED ) ;
    cylon() ;

    // Cylon / KITT / Larson scanner with 4 "movers"
  } else if ( strcmp(routines[ledMode], "cylonmulti") == 0 ) {
    taskLedModeSelect.setInterval( CYLON_SPEED ) ;
    cylonMulti() ;

    // Fade glitter
  } else if ( strcmp(routines[ledMode], "fglitter") == 0 ) {
    taskLedModeSelect.setInterval( FADEGLITTER_SPEED ) ;
    fadeGlitter() ;

    //  Disco glitter
  } else if ( strcmp(routines[ledMode], "dglitter") == 0 ) {
    taskLedModeSelect.setInterval( DISCOGLITTER_SPEED ) ;
    discoGlitter() ;

    // With thanks to Hans for the strobe idea https://www.tweaking4all.nl/hardware/arduino/adruino-led-strip-effecten/#strobe
  } else if ( strcmp(routines[ledMode], "strobe") == 0 ) {
    //    setInterval is done in the subroutine itself
    strobe( 0, 10 ) ;

  } else if ( strcmp(routines[ledMode], "flashbpm") == 0 ) {
    strobe( 130, 2 ) ;

    // Black - off
  } else if ( strcmp(routines[ledMode], "black") == 0 ) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    taskLedModeSelect.setInterval( 500 ) ;  // long because nothing is going on anyways.

  } else if ( strcmp(routines[ledMode], "pulse") == 0 ) {
    pulse() ;
    taskLedModeSelect.setInterval( 20 ) ;

  } else if ( strcmp(routines[ledMode], "pulsestatic") == 0 ) {
    pulse_static() ;
    taskLedModeSelect.setInterval( 25 ) ;

    /*
      } else if ( strcmp(routines[ledMode], "pulse2") == 0 ) {
        pulse2() ;

        // Caterpillar walk
      } else if ( strcmp(routines[ledMode], "pulsesuck") == 0 ) {
        pulse_suck() ;
    */

  } else if ( strcmp(routines[ledMode], "racers") == 0 ) {
    racingLeds() ;
    taskLedModeSelect.setInterval( 5 ) ;
  }
}





// interrupt triggered button press with a very simple debounce (discard multiple button presses < 500ms)
void shortKeyPress() {
  if ( millis() - lastButtonChange > 300 ) {
    ledMode++;
    DEBUG_PRINT(F("ledMode = ")) ;
    DEBUG_PRINT( routines[ledMode] ) ;
    DEBUG_PRINT(F(" mode ")) ;
    DEBUG_PRINTLN( ledMode ) ;

    if (ledMode >= NUMROUTINES ) {
      ledMode = 0;
    }

    lastButtonChange = millis() ;
  } else {
    //    DEBUG_PRINTLN(F("Too short an interval") ) ;
  }
}





