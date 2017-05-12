#define MAX_NEG_ACCEL -3000
#define MAX_POS_ACCEL 3000

void FillLEDsFromPaletteColors() {
  static uint8_t startIndex = 0;  // initialize at start
  static byte flowDir = 1 ;

  const CRGBPalette16 palettes[] = { RainbowColors_p, RainbowStripeColors_p, OceanColors_p, HeatColors_p, PartyColors_p, CloudColors_p, ForestColors_p } ;

  if ( isMpuUp() ) {
    flowDir = 1 ;
  } else if ( isMpuDown() ) {
    flowDir = -1 ;
  }

  startIndex += flowDir ;

  uint8_t colorIndex = startIndex ;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( palettes[ledMode], colorIndex, MAX_BRIGHT, NOBLEND );
    colorIndex += STEPS;
  }
  addGlitter(80);

  FastLED.setBrightness( map( constrain(aaRealZ, 0, MAX_POS_ACCEL), 0, MAX_POS_ACCEL, MAX_BRIGHT, 40 )) ;

  FastLED.show();
}

void addGlitter( fract8 chanceOfGlitter)
{
  for ( int i = 0 ; i < 4 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] += CRGB::White;
    }
  }
}

/*
  // Not used anywhere, but feel free to replace addGlitter with addColorGlitter in FillLEDsFromPaletteColors() above
  void addColorGlitter( fract8 chanceOfGlitter)
  {
  for ( int i = 0 ; i < 4 ; i++ ) {
    if ( random8() < chanceOfGlitter) {
      leds[ random16(NUM_LEDS) ] = CHSV( random8(), 255, MAX_BRIGHT);
    }
  }
  }
*/

void fadeGlitter() {
  addGlitter(90);
  FastLED.show();
  fadeall(200);
}

void discoGlitter() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  addGlitter(map( constrain( activityLevel(), 0, 3000), 0, 3000, 70, 255 ));
  FastLED.show();
}


// If you want to restrict the color cycling to a HSV range, adjust these:
#define STARTHUE 0
#define ENDHUE 255

void cylon() {
  static uint8_t ledPosAdder = 1 ;
  static uint8_t ledPos = 0;

  leds[ledPos] = CHSV( map( yprX, 0, 360, STARTHUE, ENDHUE ) , 255, MAX_BRIGHT);

  ledPos += ledPosAdder ;
  if ( ledPos == 0 or ledPos == NUM_LEDS ) {
    ledPosAdder *= -1 ;
    ledPos += ledPosAdder ;
  }

  FastLED.show();
  fadeall(245);
}

#define POS1 0
#define POS2 round(NUM_LEDS/3)
#define POS3 round(NUM_LEDS/3) + round(NUM_LEDS/3)
#define POS4 NUM_LEDS - 2

void cylonMulti() {
  static uint8_t ledPos[] = {POS1, POS2, POS3, POS4}; // Starting position
  static int ledAdd[] = {1, 1, 1, 1}; // Starting direction

  for (int i = 0; i < 4; i++) {
    // Turn around at ends:
    if ( (ledPos[i] + ledAdd[i] == 0) or (ledPos[i] + ledAdd[i] == NUM_LEDS) ) {
      ledAdd[i] *= -1 ;
    }
    ledPos[i] += ledAdd[i] ;

    leds[ledPos[i]] = CHSV(40 * i, 255, MAX_BRIGHT);

    /*    Circular:
           if ( ledPos[i] + ledAdd[i] < 0) {
          ledPos[i] = NUM_LEDS - 1 ;
        } else if (ledPos[i] + ledAdd[i] == NUM_LEDS) {
          ledPos[i] = 0 ;
        } else {
          ledPos[i] += ledAdd[i] ;
        }
    */
  }

  FastLED.show();
  fadeall(200);
}


void fadeall(uint8_t fade_all_speed) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(fade_all_speed);
  }
}

void brightall(uint8_t bright_all_speed) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] += leds[i].scale8(bright_all_speed) ;
  }
}

#define STROBE_ON_TIME 40

// Pretty awful - current timings are like lightning
void strobe( int bpm, uint8_t numStrobes ) {
  static uint8_t strobesToDo = numStrobes ;

  taskLedModeSelect.setInterval(STROBE_ON_TIME); // run this task every STROBE_ON_TIME seconds

  //  DEBUG_PRINTLN( taskLedModeSelect.getRunCounter() ) ;

  if ( (taskLedModeSelect.getRunCounter() % 2 ) == 0 ) {
    fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ), 255, 255) );
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  FastLED.show();

  // use getRunCounter (number of iterations of taskLedModeSelect), and if evenly divisible by strobesToDo, wait a bit
  if ( (taskLedModeSelect.getRunCounter() % strobesToDo) == 0 ) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    if ( bpm != 0 ) {
      // If we want to numStrobes of STROBE_ON_TIME and numStrobes of black we need to subtract it from the BPM to delay calculation
      taskLedModeSelect.setInterval( round(60000 / bpm) - ( STROBE_ON_TIME * numStrobes * 2) );
    } else {
      // Lightning simulation
      strobesToDo = random8(4, 12) ;
      taskLedModeSelect.setInterval(random16(1000, 2500));
    }
  }
}

#define S_SENSITIVITY 3000  // lower for less movement to trigger accelerometer routines

void strobe2() {
  if ( activityLevel() > S_SENSITIVITY ) {
    fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ), 255, MAX_BRIGHT)); // yaw for color
  } else {
    fadeall(150);
  }
  FastLED.show();
}

/*
  void pulse() {
  static uint8_t startPixelPos = 0 ;
  uint8_t endPixelPos = startPixelPos + 20 ;
  uint8_t middlePixelPos = endPixelPos - round( (endPixelPos - startPixelPos) / 2 ) ;

  uint8_t hue = map( yprX, 0, 360, 0, MAX_BRIGHT ) ;

  static int brightness = 0;
  static int brightAdder = 15;
  static int brightStartNew = random8(1, 30) ;

  // Writing outside the array gives weird effects
  startPixelPos  = constrain(startPixelPos, 0, NUM_LEDS - 1) ;
  middlePixelPos = constrain(middlePixelPos, 0, NUM_LEDS - 1) ;
  endPixelPos    = constrain(endPixelPos, 0, NUM_LEDS - 1) ;

  brightness += brightAdder ;
  if ( brightness >= 250 ) {
    brightAdder = random8(5, 15) * -1 ;
    brightness += brightAdder ;
  }
  if ( brightness <= 0 ) {
    brightAdder = 0 ;
    brightness = 0 ;
    if ( startPixelPos == brightStartNew ) {
      brightAdder = 15;
      brightStartNew = random8(1, 70) ;
    }
  }

  //  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fill_gradient(leds, startPixelPos, CHSV(hue, 255, 0), middlePixelPos, CHSV(hue, 255, brightness), SHORTEST_HUES);
  fill_gradient(leds, middlePixelPos, CHSV(hue, 255, brightness), endPixelPos, CHSV(hue, 255, 0), SHORTEST_HUES);
  FastLED.show();
  }
*/

#define MIN_BRIGHT 10

void pulse2() {
  int middle ;
  static int startP ;
  static int endP ;
  static uint8_t hue ;
  static int brightness = 0 ;
  static int bAdder = 1;
  static bool flowDir = 1;
  static bool sequenceEnd = true ;

  if ( brightness < MIN_BRIGHT ) {
    sequenceEnd = true ;
  }

  if ( not sequenceEnd ) {
    if ( flowDir ) {
      endP-- ;
      startP = endP - 20 ;
    } else {
      startP++ ;
      endP = startP + 20 ;
    }

    if ( startP == 89 or endP == 1 ) {
      sequenceEnd = true ;
    }

    middle = endP - round( (endP - startP) / 2 ) ;

    startP = constrain(startP, 0, NUM_LEDS - 1) ;
    middle = constrain(middle, 0, NUM_LEDS - 1) ;
    endP = constrain(endP, 0, NUM_LEDS - 1) ;

    brightness += bAdder ;
    brightness = constrain(brightness, 0, MAX_BRIGHT) ;
    if ( brightness >= 250 ) {
      bAdder = -10 ;
    }

    fill_gradient(leds, startP, CHSV(hue, 255, 0), middle, CHSV(hue, 255, brightness), SHORTEST_HUES);
    fill_gradient(leds, middle, CHSV(hue, 255, brightness), endP, CHSV(hue, 255, 0), SHORTEST_HUES);

  } else {

    fill_solid(leds, NUM_LEDS, CRGB::Black);
    hue = random8(0, 60) ;
    brightness = MIN_BRIGHT ;
    bAdder = 15 ;
    flowDir = ! flowDir ; // flip it!
    sequenceEnd = false ;

    if ( flowDir ) {
      endP = random8(30, 70);
    } else {
      startP = random8(30, 70);
    }
  }

  FastLED.show();
}


void pulse_static() {
  int middle ;
  static int startP ;
  static int endP ;
  static uint8_t hue ;
  static int bAdder ;

  static int brightness = 0 ;
  static bool sequenceEnd ;

  if ( brightness < MIN_BRIGHT ) {
    sequenceEnd = true ;
  }

  // while brightness is more than MIN_BRIGHT, keep increasing brightness etc.
  // If brightness drops below MIN_BRIGHT, we start a new sequence at a new position
  if ( not sequenceEnd ) {
    if ( bAdder < 0 and startP < endP ) {
      startP++ ;
      endP-- ;
      if ( startP == endP ) {
        sequenceEnd = true ;
      }
    }
    if ( bAdder > 0  and ( endP - startP < 30 ) ) {
      startP-- ;
      endP++ ;
    }
    middle = endP - round( (endP - startP) / 2 ) ;

    startP = constrain(startP, 0, NUM_LEDS - 1) ;
    middle = constrain(middle, 0, NUM_LEDS - 1) ;
    endP = constrain(endP, 0, NUM_LEDS - 1) ;

    brightness += bAdder ;
    brightness = constrain(brightness, 0, MAX_BRIGHT) ;
    if ( brightness >= 250 ) {
      bAdder = -5 ;
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_gradient(leds, startP, CHSV(hue, 255, 0), middle, CHSV(hue, 255, brightness), SHORTEST_HUES);
    fill_gradient(leds, middle, CHSV(hue, 255, brightness), endP, CHSV(hue, 255, 0), SHORTEST_HUES);
    FastLED.show();
  }

  if ( sequenceEnd ) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    hue = random8(0, 60) ;
    brightness = MIN_BRIGHT + 1 ;
    bAdder = 10 ;
    startP = random8(1, 70);
    endP = startP + 30 ;
    sequenceEnd = false ;
    taskLedModeSelect.setInterval(random16(200, 700)) ;
  }
}




#define COOLING  55
#define SPARKING 120
#define FIRELEDS round( NUM_LEDS / 2 )

// Adapted Fire2012. This version starts in the middle and mirrors the fire going down to both ends.
// Works well with the Adafruit glow fur scarf.
// FIRELEDS defines the position of the middle LED.

void Fire2012()
{
  // Array of temperature readings at each simulation cell
  static byte heat[FIRELEDS];

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < FIRELEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / FIRELEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = FIRELEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < SPARKING ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = FIRELEDS; j < NUM_LEDS; j++) {
    int heatIndex = j - FIRELEDS ;
    CRGB color = HeatColor( heat[heatIndex]);
    leds[j] = color;
  }

  /*  "Reverse" Mapping needed:
      ledindex 44 = heat[0]
      ledindex 43 = heat[1]
      ledindex 42 = heat[2]
      ...
      ledindex 1 = heat[43]
      ledindex 0 = heat[44]
  */
  for ( int j = 0; j <= FIRELEDS; j++) {
    int ledIndex = FIRELEDS - j ;
    CRGB color = HeatColor( heat[j]);
    leds[ledIndex] = color;
  }

  FastLED.show();
}

void racingLeds() {
  //  static long loopCounter = 0 ;
  static uint8_t racer[] = {0, 1, 2, 3}; // Starting positions
  static int racerDir[] = {1, 1, 1, 1}; // Current direction
  static int racerSpeed[] = { random8(1, 4), random8(1, 4) , random8(1, 4), random8(1, 4) }; // Starting speed
  const CRGB racerColor[] = { CRGB::Red, CRGB::Blue, CRGB::White, CRGB::Orange }; // Racer colors

#define NUMRACERS sizeof(racer) //array size  

  fill_solid(leds, NUM_LEDS, CRGB::Black);    // Start with black slate

  for ( int i = 0; i < NUMRACERS ; i++ ) {
    leds[racer[i]] = racerColor[i]; // Assign color

    // If taskLedModeSelect.getRunCounter() is evenly divisible by 'speed' then check if we've reached the end (if so, reverse), and do a step
    if ( ( taskLedModeSelect.getRunCounter() % racerSpeed[i]) == 0 ) {
      if ( racer[i] + racerDir[i] >= NUM_LEDS) {
        racer[i] = 0 ;
      } else {
        racer[i] += racerDir[i] ;
      }
      /*
            if ( (racer[i] + racerDir[i] >= NUM_LEDS) or (racer[i] + racerDir[i] <= 0) ) {
              racerDir[i] *= -1 ;
            }
            racer[i] += racerDir[i] ;
      */
    }

    if ( (taskLedModeSelect.getRunCounter() % 40 ) == 0 ) {
      racerSpeed[i] = random8(2, 6) ;  // Randomly speed up or slow down
    }
  }

  //  loopCounter++ ;

  FastLED.show();
}

#define MAX_NEG_ACCEL -5000
#define MAX_POS_ACCEL 5000
#define MIN_BRIGHT 20

void waveYourArms() {
  // Use yaw for color; use accelZ for brightness
  fill_solid(leds, NUM_LEDS, CHSV( map( yprX, 0, 360, 0, 255 ) , 255, map( constrain(aaRealZ, MAX_NEG_ACCEL, MAX_POS_ACCEL), MAX_NEG_ACCEL, MAX_POS_ACCEL, MIN_BRIGHT, MAX_BRIGHT )) );
  FastLED.show();
}


#define SENSITIVITY 2300  // lower for less movement to trigger

void shakeIt() {
  int startLed ;

  if ( isMpuDown() ) {  // Start near controller if down
    startLed = 0 ;
  } else if ( isMpuUp() ) {
    startLed = NUM_LEDS - 1 ;
  }

  if ( activityLevel() > SENSITIVITY ) {
    leds[startLed] = CHSV( map( yprX, 0, 360, 0, 255 ), 255, MAX_BRIGHT); // yaw for color
  } else {
    leds[startLed] = CHSV(0, 0, 0); // black
    //    leds[] = leds[NUM_LEDS - 1] ;  // uncomment for circular motion
  }

  if ( isMpuDown() ) {
    for (int i = NUM_LEDS - 2; i >= 0 ; i--) {
      leds[i + 1] = leds[i];
    }
  } else if ( isMpuUp() ) {
    for (int i = 0 ; i <= NUM_LEDS - 2 ; i++) {
      leds[i] = leds[i + 1];
    }
  }

  FastLED.show();
}

#define STRIPE_LENGTH 5

void whiteStripe() {
  static CRGB patternCopy[STRIPE_LENGTH] ;
  static int startLed = 0 ;

  if( taskWhiteStripe.getInterval() != WHITESTRIPE_SPEED ) {
    taskWhiteStripe.setInterval( WHITESTRIPE_SPEED ) ;
  }

  if ( startLed == 0 ) {
    for (int i = 0; i < STRIPE_LENGTH ; i++ ) {
      patternCopy[i] = leds[i];
    }
  }

  // 36 40   44 48 52 56   60
  
  leds[startLed] = patternCopy[0] ;
  for (int i = 0; i < STRIPE_LENGTH - 2; i++ ) {
    patternCopy[i] = patternCopy[i + 1] ;
  }
  patternCopy[STRIPE_LENGTH - 1] = leds[startLed + STRIPE_LENGTH] ;

  fill_gradient(leds, startLed + 1, CHSV(0, 0, 255), startLed + STRIPE_LENGTH, CHSV(0, 0, 255), SHORTEST_HUES);

  startLed++ ;

  if ( startLed + STRIPE_LENGTH == NUM_LEDS - 1) { // LED nr 90 is index 89
    for (int i = startLed; i < startLed + STRIPE_LENGTH; i++ ) {
      leds[i] = patternCopy[i];
    }
    
    startLed = 0 ;
    taskWhiteStripe.setInterval(random16(4000, 10000)) ;
  }

  FastLED.show();
}



