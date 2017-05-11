// ================================================================
// ================================================================
// =====             MPU 6050 CODE STUFF                      =====
// ================================================================
// ================================================================


// ===               INTERRUPT DETECTION ROUTINE                ===
volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
  //  DEBUG_PRINTLN(F(":"));
}



void getDMPData() {

  if ( fifoCount > packetSize ) {
    DEBUG_PRINTLN(F("yo!"));
    mpu.resetFIFO();
  }

  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();

  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
    mpu.resetFIFO();
    DEBUG_PRINTLN(F("FIFO overflow!"));

  } else if (mpuIntStatus & 0x02) {
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    fifoCount -= packetSize;
  }
}




// display Euler angles in degrees
void getYPRAccel() {
  // orientation/motion vars
  Quaternion quat;        // [w, x, y, z]         quaternion container
  VectorInt16 aa;         // [x, y, z]            accel sensor measurements
  VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
  VectorFloat gravity;    // [x, y, z]            gravity vector
  float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


  mpu.dmpGetQuaternion(&quat, fifoBuffer);
  mpu.dmpGetGravity(&gravity, &quat);
  mpu.dmpGetYawPitchRoll(ypr, &quat, &gravity);
  mpu.dmpGetAccel(&aa, fifoBuffer);
  mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);

  yprX = (ypr[0] * 180 / M_PI) + 180;
  yprY = (ypr[1] * 180 / M_PI) + 90;
  yprZ = (ypr[2] * 180 / M_PI) + 90;

  aaRealX = aaReal.x ;
  aaRealY = aaReal.y ;
  aaRealZ = aaReal.z ;
}



bool isShaking() {
  return activityLevel() > 6000 ;
}



#define POWER 256
int activityLevel() {
  static int value ;
  const int alpha = 178;
  int measurement = round( (abs( aaRealX)  + abs( aaRealY)  + abs( aaRealZ )) / 3 );
  value = (alpha * measurement + (POWER - alpha) * value ) / POWER; //
  //  return value ;
  return round( (abs( aaRealX )  + abs( aaRealY )  + abs( aaRealZ )) / 3 );
}



bool isTilted() {
#define TILTED_AT_DEGREES 10
  return ( 90 - TILTED_AT_DEGREES > max(yprY, yprZ) or yprY > 90 + TILTED_AT_DEGREES ) ;
}

// check if MPU is pitched up
bool isMpuUp() {
  return yprZ > 90 ;
}

// check if MPU is pitched down
bool isMpuDown() {
  return yprZ < 90 ;
}

bool isYawReliable() {
#define MAXANGLE 45
  // Check if yprY or yprZ are tilted more than MAXANGLE. 90 = level
  // yaw is not reliable below MAXANGLE
  return ( MAXANGLE < min(yprY, yprZ) and max(yprY, yprZ) < 90 + MAXANGLE ) ;
}


void printDebugging() {
  DEBUG_PRINT(yprX);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(yprY);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(yprZ);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(aaRealX);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(aaRealY);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(aaRealZ);
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(activityLevel());
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT(isYawReliable());
  DEBUG_PRINT(F("\t"));
  DEBUG_PRINT( taskGetDMPData.getRunCounter() ) ;
  DEBUG_PRINT("\t");
  DEBUG_PRINT( freeRam() ) ;
  DEBUG_PRINTLN() ;
}




int freeRam ()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
