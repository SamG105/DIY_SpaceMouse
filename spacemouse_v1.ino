#include <OneButton.h>
#include <Keyboard.h>
#include <Mouse.h>
#include <SimpleKalmanFilter.h>
#include <TLx493D_inc.hpp>

// Definitions for the magnetometer
using namespace ifx::tlx493d;

  //Definition of the pwoer pin and sensor object
const uint8_t POWER_PIN = 7;
TLx493D_A1B6 mag(Wire, TLx493D_IIC_ADDR_A0_e);

//define the kalman filter
SimpleKalmanFilter xFilter(.3,.5,0.1), yFilter(.3,.5,0.1), zFilter(.3,.5,0.1);

//Definitions for the buttons
#define BUTTON_1_PIN 1
#define BUTTON_2_PIN 2

  // define button objects
OneButton button_1;
OneButton button_2;

// global variables
float xHome = 0, yHome = 0, zHome = 0;
float x = 0, y = 0, z = 0;

// constants
const float xyDeadzone = .3;
const float zDeadzone = .15;
const float zThreshold = .5;
int calibrationSamples = 300;
bool isOrbit = false;
double sensitivity = 0.25;
double zSensitivity = 1;

//range of the mag
int magRange = 10;
//range fo the HID report
int HIDRange = 127;

bool movementActive = false;

void setup() {
  Serial.begin(115200);
  delay(50);

  //Setup for the magnetometer
  mag.setPowerPin(POWER_PIN, OUTPUT, INPUT, LOW, HIGH, 1000, 250000);
  mag.begin();
  mag.setSensitivity(TLx493D_SHORT_RANGE_e);
  

  //Setup for the buttons
  button_1.setup(BUTTON_1_PIN, INPUT_PULLUP, true);
  button_2.setup(BUTTON_2_PIN, INPUT_PULLUP, true);
  //Attaching the clicks
  button_1.attachClick(goHome);
  button_1.attachDoubleClick(goHome);

  button_2.attachClick(fitToScreen);
  button_2.attachDoubleClick(fitToScreen);

  //Basic calibration to find center point of mouse
  for (int i = 0; i < calibrationSamples; i++) {
    double xRaw, yRaw, zRaw;
    mag.getMagneticField(&xRaw, &yRaw, &zRaw);
    //mag.printRegisters();

    // to eventually average
    xHome += float(xRaw);
    yHome += float(yRaw);
    zHome += float(zRaw);

    Serial.print(".");
  }

  //average measurments
  xHome = xHome / calibrationSamples;
  yHome = yHome / calibrationSamples;
  zHome = zHome / calibrationSamples;

  Serial.println();
  Serial.println(xHome);
  Serial.println(yHome);
  Serial.println(zHome);

  //Initialize keyboard and mouse
  Keyboard.begin();
  Mouse.begin();
  
}

void loop() {
  //run the buttons at the start of the loop
  button_1.tick();
  button_2.tick();

  //To store new data from mag
  double xRaw, yRaw, zRaw;

  //get new data
  if (!mag.getMagneticField(&xRaw, &yRaw, &zRaw)) {
    Serial.println("error while getting data");
  }

  //Update the filters
  x = xFilter.updateEstimate(float(xRaw) - xHome);
  y = yFilter.updateEstimate(float(yRaw) - yHome);
  z = zFilter.updateEstimate(float(zRaw) - zHome);

  applyDeadzonef(&x, &y, &z);

  if (abs(x) > 0 || abs(y) > 0) {
    // Start of movement → press keys only once
    if (!movementActive) {
      Keyboard.press(KEY_LEFT_SHIFT);
      Mouse.press(MOUSE_MIDDLE);
      movementActive = true;
      Serial.println("Movement started - keys pressed");
    }

    // Send mouse movement every loop (this is what you want while moving)
    double xMove = map(x, -magRange, magRange, -HIDRange, HIDRange);
    double yMove = map(y, -magRange, magRange, -HIDRange, HIDRange);

    Mouse.move(xMove * sensitivity, yMove * sensitivity, 0);
  } 
  else {
    // End of movement → release keys only once
    if (movementActive) {
      Mouse.release(MOUSE_MIDDLE);
      Keyboard.releaseAll();
      movementActive = false;
      Serial.println("Movement ended - keys released");
    }
  }
  // === Z-axis Zoom (independent vertical scroll) ===
  if (abs(z) > 0) {
    double zMove = map(z, -magRange, magRange, -HIDRange, HIDRange);
    Mouse.move(0, 0, -(zMove * zSensitivity));   // wheel = third parameter
  }
  // Optional: small delay if you need to reduce CPU usage / USB traffic
  // delay(5);   // uncomment and tune if needed
}

//BUtton press actions
void goHome() {

}

void fitToScreen() {
  Mouse.click(MOUSE_MIDDLE);
  delay(80);
  Mouse.click(MOUSE_MIDDLE);
}

void applyDeadzone(double *x, double *y, double *z) {
    if (abs(*x) < xyDeadzone) *x = 0.0;
    if (abs(*y) < xyDeadzone) *y = 0.0;
    if (abs(*z) < zDeadzone)  *z = 0.0;
}
void applyDeadzonef(float *x, float *y, float *z) {
    if (fabsf(*x) < xyDeadzone) *x = 0.0f;
    if (fabsf(*y) < xyDeadzone) *y = 0.0f;
    if (fabsf(*z) < zDeadzone)  *z = 0.0f;
}
