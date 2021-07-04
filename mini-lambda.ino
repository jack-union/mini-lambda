/*
  Lambda display with optional multifunction display

  jack union, 2021.

*/

#define MINI_GAUGE_VERSION "Lambda v0.1"

//----Libraries to Include--------
#include <Arduino.h>
#include <AltSoftSerial.h>
#include "SwitecX25.h"
#include <SPI.h>
#include <U8g2lib.h>
#include <avr/wdt.h>
//----End Libraries--------------

//----Define PIN Settings----------
#define STEPPIN_1 4     // back view top right pin
#define STEPPIN_2 5     // back view bottom right pin
#define STEPPIN_3 6     // back view bottom left pin
#define STEPPIN_4 7     // back view top left pin
#define SERIAL_RX 8 // D8 lambda digital serial input
#define SERIAL_TX 9 // D9 lambda digital serial input
//----End Define PIN Settings------

//----Define OLED Display Settings----
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// A4: SDA
// A5: SCL
//-----End OLED Display Settings------

//----Define other constants-------
#define UPDATE_INTERVAL 100 // milliseconds speedo update rate
#define EOL_CR 0x0D
#define EOL_LF 0x0A
#define FAST_EVERY_LOOPS 100 // every 100 loops send command to lambda sensor
//----End Define other constants---

//----Stepper settings and object----
// standard X25.168 range 315 degrees at 1/3 degree steps
#define STEPS 315 * 3
// speedo type
#define GAUGE_RANGE 810   // 270 * 3  scale from lambda 0.7 to 1.3
#define GAUGE_OFFSET 135  // 45 * 3   lambda 0.7 pos start of scale
#define GAUGE_NC 67       // n/c position
#define GAUGE_ND 0        // n/d position
#define SWEEP_AT_START 1
//----End Stepper settings and object----

//----Objects----
// 1,3" SH1106 display
U8G2_SH1106_128X64_NONAME_2_HW_I2C display(U8G2_R0);

// create the motor object with the maximum steps allowed
SwitecX25 stepper(STEPS, STEPPIN_1, STEPPIN_2, STEPPIN_3, STEPPIN_4);

// Serial input port
AltSoftSerial altSerial;
//----End Objects----

//-----Variables----------------------
uint16_t lambda = 100; // lambda factor *100
char lambdaInputBuffer[20];
byte lambdaInputNextPos = 0;
byte lambdaStatus = 0;
byte lambdaError = 127;
unsigned long lambdaLastDataAt = 0;
unsigned long displayUpdatedAt = 0;
byte loops = 0;
//----End Variables-------------------

//-----Start-up code run once---------
void setup() {
  MCUSR = 0;     // clear status register
  wdt_disable(); // watchdog disable

  Serial.begin(115200);
  altSerial.begin(115200);
  Serial.println(F(MINI_GAUGE_VERSION));
  altSerial.write(EOL_CR);
  
  initDisplay();
  reset_stepper();
  delay(1000);

  fastLambdaUpdate();
  Serial.println(F("Setup done."));
}
//------End Start-up code-------------

//------Start of Loop-----------------
void loop() {
  // process serial data
  if (lambdaReadInput()) {
    lambdaProcessInput();
  }
  
  // check if data is recent
  lambdaSenseTimeout();
  
  // display and data refresh every (UPDATE_INTERVAL) milliseconds
  if ((millis() - displayUpdatedAt) > UPDATE_INTERVAL) {
    display.firstPage();
    do {
      draw_lambda(lambda);
    } while (display.nextPage());
    displayUpdatedAt = millis();
    loops = loops + 1;

    // update stepper position
    set_stepper_pos();
  }

  // send fast update setting to lambda sensor
  if (loops == FAST_EVERY_LOOPS) {
    fastLambdaUpdate();
    loops = 0;
  }
}
//-------End of Loop------------------
