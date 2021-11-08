// code adapted from https://tinycircuits.com/blogs/learn/tinyscreen-tinyscreen-menu-scroll
      
#define SELECTION_KEYBOARD 0
#define SELECTION_MOUSE 1
#define SELECTION_USB 2
#define SELECTION_VERBOSE 3

#define mainMenuIndex 0

// Change the menu font colors for 16bit color use TS_16b prefix, for 8bit use TS_8b:
// Black, Gray, DarkGray(16b exclusive), White, Blue, DarkBlue, Red, DarkRed, Green, DarkGreen, Brown, DarkBrown, Yellow

// color configs
uint16_t defaultFontColor = TS_16b_DarkGreen;
uint16_t defaultActiveFontColor =TS_16b_Blue;
uint16_t defaultFontBG = TS_16b_Black;
uint16_t inactiveFontColor = TS_16b_Gray;
uint16_t inactiveFontBG = TS_16b_Black;

// global menu vars
unsigned long time_now = 0;
uint8_t menuHistory[5];
uint8_t menuHistoryIndex = 0;
uint8_t menuSelectionLineHistory[5];
int currentMenu = 0;
int currentMenuLine = 0;
int lastMenuLine = -1;
int currentSelectionLine = 0;
int lastSelectionLine = -1;

bool needMenuDraw = true;

// config array, defaulting notify to true
int configSelection[4] = {0,0,0,1};


void (*menuHandler)(uint8_t) = NULL;
uint8_t (*editorHandler)(uint8_t, int*, /*int*,*/ void (*)()) = NULL;


//Struct for creating menus
typedef struct
{
  const uint8_t amtLines; //How many lines in said menu
  const char* const * strings; // Provide an Array of Strings per lines
  void (*selectionHandler)(uint8_t); //Handle to Menu
} menu_info;


//Main Menu Array 
static const char PROGMEM mainMenuString0[] = "Keyboard Block";
static const char PROGMEM mainMenuString1[] = "Mouse Block";
static const char PROGMEM mainMenuString2[] = "USB Block"; //Replace these instances with the function name
static const char PROGMEM mainMenuString3[] = "Breach Notify";
static const char PROGMEM mainMenuString4[] = "Activate";


static const char* const PROGMEM mainMenuStrings[] =
{
  mainMenuString0, mainMenuString1, mainMenuString2, mainMenuString3, mainMenuString4
};

const menu_info mainMenuInfo =
{
  5,
  mainMenuStrings,
  mainMenu,
};

const menu_info menuList[] = {mainMenuInfo};


// methods for main program loop

uint32_t millisOffset() {
  return millis();
}

uint32_t getSecondsCounter() {
  return millis()/1000;
}

int requestScreenOn() {
  sleepTimer = millisOffset();
  if (!displayOn) {
    setTime(counter);
    displayOn = 1;
    updateMainDisplay();
    return 1;
  }
  return 0;
}

void checkButtons() {
  byte buttons = display.getButtons();
  if (buttonReleased && buttons) {
    if (displayOn) {
      buttonPress(buttons);
    }
    requestScreenOn();
    buttonReleased = 0;
  }
  if (!buttonReleased && !(buttons & 0x1F)) {
    buttonReleased = 1;
  }
}

void initHomeScreen() {
  displayBuffer.clearWindow(0, 1, 96, 5);
  rewriteTime = true;
  rewriteMenu = true;
  updateMainDisplay();
}

void updateMainDisplay() {
  updateDateTimeDisplay();
  if (currentDisplayState == displayStateHome) {
    displayBuffer.setCursor(9, menuTextY[6]);
    displayBuffer.print("Menu");
    
    leftArrow(0, 57);    
    rewriteMenu = false;
  }
}


void updateDateTimeDisplay() {
  displayBuffer.clearWindow(0, 0, 96, 8);
  int currentDay = day();
  lastDisplayedDay = currentDay;
  displayBuffer.setCursor(0, -1);
  displayBuffer.print(dayShortStr(weekday()));
  displayBuffer.print(' ');
  displayBuffer.print(month());
  displayBuffer.print('/');
  displayBuffer.print(day());
  displayBuffer.print("  ");
  lastHourDisplayed = hour();
  lastMinuteDisplayed = minute();
  lastSecondDisplayed = second();

  int hour12 = lastHourDisplayed;
  int AMPM = 1;
  if (hour12 > 12) {
    AMPM = 2;
    hour12 -= 12;
  }
  lastHourDisplayed = hour12;
  displayBuffer.setCursor(58, -1);
  if (lastHourDisplayed < 10)displayBuffer.print(' ');
  displayBuffer.print(lastHourDisplayed);
  displayBuffer.write(':');
  if (lastMinuteDisplayed < 10)displayBuffer.print('0');
  displayBuffer.print(lastMinuteDisplayed);
  displayBuffer.setCursor(80, -1);
  if (AMPM == 2) {
    displayBuffer.print(" PM");
  } else {
    displayBuffer.print(" AM");
  }
  rewriteTime = false;
}

void liveDisplay() 
{
  displayBuffer.setCursor(0, menuTextY[1]);
  displayBuffer.print("Welcome to RCDOD");
}


void buttonPress(uint8_t buttons) {
  if (currentDisplayState == displayStateHome) {   
    if (buttons == menuButton) {
      menuHandler = viewMenu;
      newMenu(mainMenuIndex);
      menuHandler(0);
    }
  } else if (currentDisplayState == displayStateMenu || currentDisplayState == displayStateCalibration) {
    if (menuHandler) {
      menuHandler(buttons);
    }
  } else if (currentDisplayState == displayStateEditor) {
    if (editorHandler) {
      editorHandler(buttons,0, NULL);
    }
  }
}

void newMenu(int8_t newIndex) {
  currentMenuLine = 0;
  lastMenuLine = -1;
  currentSelectionLine = 0;
  lastSelectionLine = -1;
  if (newIndex >= 0) {
    menuHistory[menuHistoryIndex++] = currentMenu;
    currentMenu = newIndex;
  } else {
    if (currentDisplayState == displayStateMenu) {
      menuHistoryIndex--;
      currentMenu = menuHistory[menuHistoryIndex];
      SerialMonitorInterface.print("History index:");
      SerialMonitorInterface.print(menuHistoryIndex);
    }
  }
  if (menuHistoryIndex) {
    currentDisplayState = displayStateMenu;
    currentSelectionLine = menuSelectionLineHistory[menuHistoryIndex];
  } else {
    menuSelectionLineHistory[menuHistoryIndex + 1] = 0;
    currentDisplayState = displayStateHome;
    initHomeScreen();
  }
}


void printCenteredAt(int y, char * text) {
  int width = displayBuffer.getPrintWidth(text);
  //displayBuffer.clearWindow(96 / 2 - width / 2 - 1, y, width + 2, 8);
  displayBuffer.clearWindow(0, y, 96, 10); // adjust the first arg if not wiping properly
  displayBuffer.setCursor(96 / 2 - width / 2, y);
  displayBuffer.print(F(text));
}


void toggleSelection(int index){
  if(configSelection[index]){
    configSelection[index]--;
  }
  else{
    configSelection[index]++;
  }
  
  // needed to refresh screen with new highlights
  needMenuDraw = true;
}


void mainMenu(uint8_t selection) // selection = array index of the menu item
{
  if (selection == 0)
  {
    toggleSelection(SELECTION_KEYBOARD);
  }
  if (selection == 1)
  {
    toggleSelection(SELECTION_MOUSE);
  }
  if (selection == 2)
  {
    toggleSelection(SELECTION_USB);
  }
  if (selection == 3)
  {
    toggleSelection(SELECTION_VERBOSE);
  }
  if (selection == 4)
  {
    // from keyboard_payloads
    startBinary(configSelection);

    // keyboard.begin somehow disconnects the screen from the board.
    // reconnecting it with begin allows for updates to screen after keyboard functionality
    display.begin();
    printCenteredAt(menuTextY[3], "Done!");
  }
}

// vars for drawMenu
int changeDir;
int changeEnd;
int changeStart;
int yChange;

void drawMenu() {
  if (needMenuDraw) {
    needMenuDraw = false;
    displayBuffer.clearWindow(0, 7, 96, 56);
    for (int i = 0; i < menuList[currentMenu].amtLines; i++) {
      if (i < 4 && configSelection[i]){
        displayBuffer.fontColor(defaultActiveFontColor, ALPHA_COLOR);
      }
      else if ((i == currentSelectionLine) && i < 4 && configSelection[currentSelectionLine]) {
        displayBuffer.fontColor(defaultActiveFontColor, ALPHA_COLOR);
      } else if (i == currentSelectionLine){
        displayBuffer.fontColor(defaultFontColor, ALPHA_COLOR);
      } else {
        displayBuffer.fontColor(inactiveFontColor, ALPHA_COLOR);
      }
      char buffer[20];
      strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[currentMenu].strings[currentMenuLine + i])));
      int width = displayBuffer.getPrintWidth(buffer);
      displayBuffer.setCursor(9, menuTextY[i]  + yChange - (currentSelectionLine * 8) + 8);
      displayBuffer.print(buffer);
    }

    writeArrows();
  }
  if (yChange != changeEnd) {
    if (abs(yChange - changeEnd) > 5) {
      yChange += changeDir;
    }
    if (abs(yChange - changeEnd) > 3) {
      yChange += changeDir;
    }
    yChange += changeDir;
    needMenuDraw = true;
  }
  displayBuffer.fontColor(0xFFFF, ALPHA_COLOR);
}

void viewMenu(uint8_t button) {
  if (!button) {

  } else {
    if (button == upButton) {
      if (currentSelectionLine > 0) {
        currentSelectionLine--;
      }
    } else if (button == downButton) {

      if (currentSelectionLine < menuList[currentMenu].amtLines - 1) {
        currentSelectionLine++;
      }
    } else if (button == selectButton) {
      menuList[currentMenu].selectionHandler(currentMenuLine + currentSelectionLine);
    } else if (button == backButton) {
      SerialMonitorInterface.print("BACK");
      needMenuDraw = true;
      newMenu(-1);
      if (!menuHistoryIndex)
        return;
    }
  }
  if (lastMenuLine != currentMenuLine || lastSelectionLine != currentSelectionLine) {

    if (currentSelectionLine < lastSelectionLine) {
      changeDir = 1;
      changeEnd = 0;
      changeStart = -7;
    } else {
      changeDir = -1;
      changeEnd = 0;
      changeStart = 7;
    }
    if (lastSelectionLine == -1) {
      changeStart = changeEnd; //new menu, just draw once
    }
    yChange = changeStart;
    needMenuDraw = true;
  }
  lastMenuLine = currentMenuLine;
  lastSelectionLine = currentSelectionLine;
  menuSelectionLineHistory[menuHistoryIndex] = currentSelectionLine;


  displayBuffer.fontColor(0xFFFF, ALPHA_COLOR);
}

void writeArrows() {
  upArrow(0, 15 + 2);
  downArrow(0, 45 + 5);

  rightArrow(90, 15 + 2);
  leftArrow(90, 45 + 4);
}

void leftArrow(int x, int y) {
  displayBuffer.drawLine(x + 2, y - 2, x + 2, y + 2, 0xFFFF);
  displayBuffer.drawLine(x + 1, y - 1, x + 1, y + 1, 0xFFFF);
  displayBuffer.drawLine(x + 0, y - 0, x + 0, y + 0, 0xFFFF);
}

void rightArrow(int x, int y) {
  displayBuffer.drawLine(x + 0, y - 2, x + 0, y + 2, 0xFFFF);
  displayBuffer.drawLine(x + 1, y - 1, x + 1, y + 1, 0xFFFF);
  displayBuffer.drawLine(x + 2, y - 0, x + 2, y + 0, 0xFFFF);
}

void upArrow(int x, int y) {
  displayBuffer.drawLine(x + 0, y - 0, x + 4, y - 0, 0xFFFF);
  displayBuffer.drawLine(x + 1, y - 1, x + 3, y - 1, 0xFFFF);
  displayBuffer.drawLine(x + 2, y - 2, x + 2, y - 2, 0xFFFF);
}
void downArrow(int x, int y) {
  displayBuffer.drawLine(x + 0, y + 0, x + 4, y + 0, 0xFFFF);
  displayBuffer.drawLine(x + 1, y + 1, x + 3, y + 1, 0xFFFF);
  displayBuffer.drawLine(x + 2, y + 2, x + 2, y + 2, 0xFFFF);
}
