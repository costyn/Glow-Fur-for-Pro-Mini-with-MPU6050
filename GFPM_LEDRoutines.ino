void FillLEDsFromPaletteColors() {
  static uint8_t startIndex = 0;  // initialize at start
  static byte flowDir = 1 ;

  const CRGBPalette16 palettes[] = { RainbowColors_p, RainbowStripeColors_p, OceanColors_p, HeatColors_p, PartyColors_p, CloudColors_p, ForestColors_p } ;

  if( isMpuUp() ) {
    flowDir = 1 ;
  } else if ( isMpuDown() ) {
    flowDir = -1 ;
  }
  
  startIndex += flowDir ;

  uint8_t colorIndex = startIndex ;

  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette( palettes[ledMode], colorIndex, BRIGHTNESS, LINEARBLEND);
    colorIndex += STEPS;
  }
  addGlitter(80);

  FastLED.show();
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

// Not used anywhere, but feel free to replace addGlitter with addColorGlitter in FillLEDsFromPaletteColors() above
void addColorGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] = CHSV( random8(), 255, 255);
  }
}

void fadeGlitter() {
  addGlitter(90);
  FastLED.show();
  fadeall(250);
}

void discoGlitter() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  addGlitter(90);
  FastLED.show();
}


// If you want to restrict the color cycling to a HSV range, adjust these:
#define STARTHUE 0
#define ENDHUE 255

void cylon() {
//  static uint8_t hueAdder = 1 ;  // set to higher for faster color cycling
//  static uint8_t hue = STARTHUE;
  static uint8_t ledPosAdder = 1 ;
  static uint8_t ledPos = 0;

  leds[ledPos] = CHSV( map( yprX, 0, 360, 0, 255 ) , 255, 255);
//  leds[ledPos] = CHSV(hue, 255, 255);

/*  // If current LED position is divisible by 5, increase hue by hueAdder
  if ( ledPos % 10 == 0 ) {
    hue += hueAdder ;
  }
  // if we reach ENDHUE or STARTHUE, start cycling back.
  if ( hue > ENDHUE or hue == STARTHUE ) {
    hueAdder *= -1 ;
  }

*/

  ledPos += ledPosAdder ;
  if ( ledPos == 0 or ledPos == NUM_LEDS ) {
    ledPosAdder *= -1 ;
    ledPos += ledPosAdder ;
  }

  FastLED.show();
  fadeall(230);
}

void cylonMulti() {
  static uint8_t ledPos[] = {0, 29, 48, 87}; // Starting position
  static int ledAdd[] = {1, 1, 1, 1}; // Starting direction

  for (int i = 0; i < 4; i++) {
    leds[ledPos[i]] = CHSV(40 * i, 255, 255);
    if ( (ledPos[i] + ledAdd[i] == 0) or (ledPos[i] + ledAdd[i] == NUM_LEDS) ) {
      ledAdd[i] *= -1 ;
    }
    ledPos[i] += ledAdd[i] ;
  }

  FastLED.show();
  fadeall(180);
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

void pulse() {
  static uint8_t startPixelPos = 0 ;
  uint8_t endPixelPos = startPixelPos + 20 ;
  uint8_t middlePixelPos = endPixelPos - round( (endPixelPos - startPixelPos) / 2 ) ;

  uint8_t hue = map( yprX, 0, 360, 0, 255 ) ;
  
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

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  fill_gradient(leds, startPixelPos, CHSV(hue, 255, 0), middlePixelPos, CHSV(hue, 255, brightness), SHORTEST_HUES);
  fill_gradient(leds, middlePixelPos, CHSV(hue, 255, brightness), endPixelPos, CHSV(hue, 255, 0), SHORTEST_HUES);
  FastLED.show();
}


void pulse2() {
  int middle ;
  int startP ;
  static int endP ;
  uint8_t hue ;
  int brightness;
  int bAdder ;
  static bool flowDir = 1; // remember flowDir between calls to pulse2

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  //  taskLedModeSelect.delay(random16(100, 1000)) ;

  hue = random8(0, 60) ;
  brightness = 1 ;
  bAdder = 15 ;
  flowDir = ! flowDir ; // flip it!

  if ( flowDir ) {
    endP = random8(30, 70);
  } else {
    startP = random8(30, 70);
  }

  while ( brightness > 0 ) {
    if ( flowDir ) {
      endP-- ;
      startP = endP - 20 ;
    } else {
      startP++ ;
      endP = startP + 20 ;
    }

    if ( startP == 89 or endP == 1 ) {
      break ;
    }

    middle = endP - round( (endP - startP) / 2 ) ;

    startP = constrain(startP, 0, NUM_LEDS - 1) ;
    middle = constrain(middle, 0, NUM_LEDS - 1) ;
    endP = constrain(endP, 0, NUM_LEDS - 1) ;

    brightness += bAdder ;
    brightness = constrain(brightness, 0, 255) ;
    if ( brightness >= 250 ) {
      bAdder = -10 ;
      //        Serial.print(" bAdder: ") ;
      //        Serial.print(bAdder) ;
      //        brightness += bAdder ;
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_gradient(leds, startP, CHSV(hue, 255, 0), middle, CHSV(hue, 255, brightness), SHORTEST_HUES);
    fill_gradient(leds, middle, CHSV(hue, 255, brightness), endP, CHSV(hue, 255, 0), SHORTEST_HUES);
    FastLED.show();
    //    taskLedModeSelect.delay(25);
  }
}

#define MIN_BRIGHT 10

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
    brightness = constrain(brightness, 0, 255) ;
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

void pulse_suck() {
  int middle ;
  int startPixelPos ;
  int endPixelPos ;
  uint8_t hue ;
  int brightness;
  int brightnessAdder ;
  static int lastPixelEndPos ;

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  taskLedModeSelect.delay(random16(200, 700)) ;

  hue = random8(0, 60) ;
  brightness = MIN_BRIGHT + 1 ;
  brightnessAdder = 10 ;
  startPixelPos = lastPixelEndPos ;
  endPixelPos = startPixelPos ;

  while ( brightness > MIN_BRIGHT ) {
    if ( brightnessAdder < 0 and startPixelPos < endPixelPos ) {
      startPixelPos += 2 ;
      if ( startPixelPos == endPixelPos ) {
        lastPixelEndPos = startPixelPos ;
        if ( lastPixelEndPos > 70 ) {
          lastPixelEndPos = 0 ;
        }
        break ;
      }
    }
    if ( brightnessAdder > 0  and ( endPixelPos - startPixelPos < 20 ) ) {
      endPixelPos += 2 ;
    }
    middle = endPixelPos - round( (endPixelPos - startPixelPos) / 2 ) ;

    startPixelPos = constrain(startPixelPos, 0, NUM_LEDS - 1) ;
    middle = constrain(middle, 0, NUM_LEDS - 1) ;
    endPixelPos = constrain(endPixelPos, 0, NUM_LEDS - 1) ;

    brightness += brightnessAdder ;
    brightness = constrain(brightness, 0, 255) ;
    if ( brightness >= 250 ) {
      brightnessAdder = -5 ;
    }

    fill_solid(leds, NUM_LEDS, CRGB::Black);
    fill_gradient(leds, startPixelPos, CHSV(hue, 255, 0), middle, CHSV(hue, 255, brightness), SHORTEST_HUES);
    fill_gradient(leds, middle, CHSV(hue, 255, brightness), endPixelPos, CHSV(hue, 255, 0), SHORTEST_HUES);
    FastLED.show();
    taskLedModeSelect.delay(25);
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
  static long loopCounter = 0 ;
  static uint8_t racer[] = {0, 1, 2}; // Starting positions
  static int racerDir[] = {1, 1, 1}; // Current direction
  static int racerSpeed[] = { random8(1, 4), random8(1, 4) , random8(1, 4) }; // Starting speed
  CRGB racerColor[] = { CRGB::Red, CRGB::Blue, CRGB::White }; // Racer colors

#define NUMRACERS sizeof(racer) //array size  

  fill_solid(leds, NUM_LEDS, CRGB::Black);    // Start with black slate

  for ( int i = 0; i < NUMRACERS ; i++ ) {
    leds[racer[i]] = racerColor[i]; // Assign color

    // If "loopcounter" is evenly divisible by 'speed' then check if we've reached the end (if so, reverse), and do a step
    if ( loopCounter % racerSpeed[i] == 0 ) {
      if ( (racer[i] + racerDir[i] >= NUM_LEDS) or (racer[i] + racerDir[i] <= 0) ) {
        racerDir[i] *= -1 ;
      }
      racer[i] += racerDir[i] ;
    }

    if ( loopCounter % 40 ) {
      racerSpeed[i] = random8(2, 4) ;  // Randomly speed up or slow down
    }
  }

  loopCounter++ ;
  
  FastLED.show();
}
