#include <Wire.h>
#include <TinyScreen.h>
#include <GraphicsBuffer.h>
#include <TimeLib.h>


GraphicsBuffer displayBuffer = GraphicsBuffer(96, 64, colorDepth16BPP);

TinyScreen display = TinyScreen(TinyScreenDefault); // TinyScreen TinyShield

uint8_t menuTextY[8] = {1 * 8 - 1, 2 * 8 - 1, 3 * 8 - 1, 4 * 8 - 1, 5 * 8 - 1, 6 * 8 - 1, 7 * 8 - 3, 8 * 8 - 3};

// You can change buttons here, but you will need to change the arrow graphics elsewhere in the program to
// match what you change here 
const uint8_t upButton     = TSButtonUpperLeft;
const uint8_t downButton   = TSButtonLowerLeft;
const uint8_t backButton   = TSButtonLowerRight;
const uint8_t selectButton = TSButtonUpperRight;
const uint8_t menuButton   = TSButtonLowerLeft;
const uint8_t shortcutButton = TSButtonLowerRight;

unsigned long sleepTimer = 0;
int sleepTimeout = 0;

volatile uint32_t counter = 0;

uint32_t lastPrint = 0;

uint8_t rewriteTime = true;
uint8_t lastAMPMDisplayed = 0;
uint8_t lastHourDisplayed = -1;
uint8_t lastMinuteDisplayed = -1;
uint8_t lastSecondDisplayed = -1;
uint8_t lastDisplayedDay = -1;

uint8_t displayOn = 0;
uint8_t buttonReleased = 1;
uint8_t rewriteMenu = false;
unsigned long mainDisplayUpdateInterval = 20;
unsigned long lastMainDisplayUpdate = 0;

int toggled = 0;

const uint8_t displayStateHome = 0x01;
const uint8_t displayStateMenu = 0x02;
const uint8_t displayStateEditor = 0x03;
const uint8_t displayStateCalibration = 0x04;

uint8_t currentDisplayState = displayStateHome;

// Make Serial Monitor compatible for all TinyCircuits processors
#if defined(ARDUINO_ARCH_AVR)
  #define SerialMonitorInterface Serial
#elif defined(ARDUINO_ARCH_SAMD)
  #define SerialMonitorInterface SerialUSB
#endif

void setup() {
  Wire.begin();
  SerialMonitorInterface.begin(200000);
  delay(100);
  
  display.begin();
  display.setFlip(true);
  display.setBitDepth(1);
  display.initDMA();  

  if (displayBuffer.begin()) {
    //memory allocation error- buffer too big
    while (1) {
      SerialMonitorInterface.println("Display buffer memory allocation error!");
      delay(1000);
    }
  }

  displayBuffer.setFont(thinPixel7_10ptFontInfo);

  displayBuffer.clear();
  
  initHomeScreen();
  requestScreenOn();
}

void loop() {
  displayOn = 1;
  
  if (displayOn && (millisOffset() > mainDisplayUpdateInterval + lastMainDisplayUpdate)) {
    lastMainDisplayUpdate = millis();
    display.endTransfer();
    display.goTo(0, 0);
    display.startData();
    display.writeBufferDMA(displayBuffer.getBuffer(), displayBuffer.getBufferSize());
  }

  if (currentDisplayState == displayStateHome) {
    if ( display.getButtons(TSButtonLowerRight) || display.getButtons(TSButtonUpperRight) || display.getButtons(TSButtonLowerLeft) || display.getButtons(TSButtonUpperLeft)){
      displayBuffer.clear();  // if a button is pressed, clear the screen
    }

    updateMainDisplay(); 
    liveDisplay(); // This is the main home screen display
    display.writeBufferDMA(displayBuffer.getBuffer(), displayBuffer.getBufferSize());
    delay (500); // Making this smaller will make the screen more scattered, making it longer will mean you need to hold in buttons longer
  } else {
    drawMenu();
  }

  checkButtons();
  updateMainDisplay();
}
