#define menu_debug_print true // Debug messages will print on Serial Monitor if True

// Change the menu font colors for 16bit color use TS_16b prefix, for 8bit use TS_8b:
// Black, Gray, DarkGray(16b exclusive), White, Blue, DarkBlue, Red, DarkRed, Green, DarkGreen, Brown, DarkBrown, Yellow
uint16_t defaultFontColor = TS_16b_DarkGreen;
uint16_t defaultFontBG = TS_16b_Black;
uint16_t inactiveFontColor = TS_16b_Gray;
uint16_t inactiveFontBG = TS_16b_Black;

uint8_t menuHistory[5];
uint8_t menuHistoryIndex = 0;
uint8_t menuSelectionLineHistory[5];
int currentMenu = 0;
int currentMenuLine = 0;
int lastMenuLine = -1;
int currentSelectionLine = 0;
int lastSelectionLine = -1;

void (*menuHandler)(uint8_t) = NULL;
uint8_t (*editorHandler)(uint8_t, int*, char*, void (*)()) = NULL;

//Struct for creating menus
typedef struct
{
  const uint8_t amtLines; //How many lines in said menu
  const char* const * strings; // Provide an Array of Strings per lines
  void (*selectionHandler)(uint8_t); //Handle to Menu
} menu_info;

//Main Menu Array Example
static const char PROGMEM mainMenuString0[] = "Date & Time";
static const char PROGMEM mainMenuString1[] = "Brightness";
static const char PROGMEM mainMenuString2[] = "Function Menu";
static const char PROGMEM mainMenuString3[] = "TEST";
static const char PROGMEM mainMenuString4[] = "Lorem Ipsum Dolor";

static const char* const PROGMEM mainMenuStrings[] =
{
    mainMenuString0, mainMenuString1, mainMenuString2, mainMenuString3, mainMenuString4,
};

const menu_info mainMenuInfo =
{
    5,
    mainMenuStrings,
    mainMenu,
};

// Date and Time Menu Creation
static const char PROGMEM dateTimeMenuStrings0[] = "Set Year";
static const char PROGMEM dateTimeMenuStrings1[] = "Set Month";
static const char PROGMEM dateTimeMenuStrings2[] = "Set Day";
static const char PROGMEM dateTimeMenuStrings3[] = "Set Hour";
static const char PROGMEM dateTimeMenuStrings4[] = "Set Minute";
static const char PROGMEM dateTimeMenuStrings5[] = "Set Second";

static const char* const PROGMEM dateTimeMenuStrings[] =
{
  dateTimeMenuStrings0,
  dateTimeMenuStrings1,
  dateTimeMenuStrings2,
  dateTimeMenuStrings3,
  dateTimeMenuStrings4,
  dateTimeMenuStrings5,
};

const menu_info dateTimeMenuInfo =
{
  6,
  dateTimeMenuStrings,
  dateTimeMenu,
};

//Function Menu Creation
static const char PROGMEM functionMenuString0[] = "Function 1"; //Replace these instances with the function name
static const char PROGMEM functionMenuString1[] = "Function 2";
static const char PROGMEM functionMenuString2[] = "Function 3";
static const char PROGMEM functionMenuString3[] = "Function 4";
static const char PROGMEM functionMenuString4[] = "Function 5";

static const char* const PROGMEM functionMenuStrings[] =
{
    functionMenuString0,functionMenuString1,functionMenuString2,functionMenuString3,functionMenuString4
};

const menu_info functionMenuInfo =
{
  5, //change this value if there is more functions
  functionMenuStrings,
  functionMenu,
};

const menu_info menuList[] = {mainMenuInfo, dateTimeMenuInfo, functionMenuInfo};
#define mainMenuIndex 0
#define dateTimeMenuIndex 1
#define functionMenuIndex 2

bool needMenuDraw = true;

void buttonPress(uint8_t buttons) {
  if (currentDisplayState == displayStateHome) {
    if (buttons == viewButton) {
      menuHandler = viewMenu;
      newMenu(mainMenuIndex);    
      menuHandler(0);
      
    } else if (buttons == menuButton) {
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
      editorHandler(buttons, 0, 0, NULL);
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
    }
  }
  if (menuHistoryIndex) {
    currentDisplayState = displayStateMenu;
    if (menu_debug_print)SerialMonitorInterface.print("New menu index ");
    if (menu_debug_print)SerialMonitorInterface.println(currentMenu);
    currentSelectionLine = menuSelectionLineHistory[menuHistoryIndex];
  } else {
    if (menu_debug_print)SerialMonitorInterface.print("New menu index ");
    if (menu_debug_print)SerialMonitorInterface.println("home");
    menuSelectionLineHistory[menuHistoryIndex + 1] = 0;
    currentDisplayState = displayStateHome;
    initHomeScreen();
  }
}

int currentVal = 0;
int digits[4];
int currentDigit = 0;
int maxDigit = 4;
int *originalVal;
void (*editIntCallBack)() = NULL;

uint8_t editInt(uint8_t button, int *inVal, char *intName, void (*cb)()) {
  if (menu_debug_print)SerialMonitorInterface.println("editInt");
  if (!button) {
    if (menu_debug_print)SerialMonitorInterface.println("editIntInit");
    editIntCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = editInt;
    currentDigit = 0;
    originalVal = inVal;
    currentVal = *originalVal;
    digits[3] = currentVal % 10; currentVal /= 10;
    digits[2] = currentVal % 10; currentVal /= 10;
    digits[1] = currentVal % 10; currentVal /= 10;
    digits[0] = currentVal % 10;
    currentVal = *originalVal;

    displayBuffer.clearWindow(0, 8, 96, 64);
    writeArrows();

    displayBuffer.fontColor(defaultFontColor, defaultFontBG);
    int width = displayBuffer.getPrintWidth(intName);
    displayBuffer.setCursor(96 / 2 - width / 2, menuTextY[2]);
    displayBuffer.print(intName);

    displayBuffer.setCursor(69, 15 - 3);
    displayBuffer.print("Save");
    displayBuffer.setCursor(69, 45 + 3);
    displayBuffer.print("Back");

    //displayBuffer.drawLine(1, 14,    1, 12, 0xFFFF);
    //displayBuffer.drawLine(1, 12,    6, 12, 0xFFFF);

    //displayBuffer.drawLine(1, 54,    1, 56, 0xFFFF);
    //displayBuffer.drawLine(1, 56,    6, 56, 0xFFFF);

  } else if (button == upButton) {
    if (digits[currentDigit] < 9)
      digits[currentDigit]++;
  } else if (button == downButton) {
    if (digits[currentDigit] > 0)
      digits[currentDigit]--;
  } else if (button == selectButton) {
    if (currentDigit < maxDigit - 1) {
      currentDigit++;
    } else {
      //save
      int newValue = (digits[3]) + (digits[2] * 10) + (digits[1] * 100) + (digits[0] * 1000);
      *originalVal = newValue;
      viewMenu(backButton);
      if (editIntCallBack) {
        editIntCallBack();
        editIntCallBack = NULL;
      }
      return 1;
    }
  } else if (button == backButton) {
    if (currentDigit > 0) {
      currentDigit--;
    } else {
      if (menu_debug_print)SerialMonitorInterface.println("back");
      viewMenu(backButton);
      return 0;
    }
  }

  displayBuffer.setCursor(96 / 2 - 16, menuTextY[3] + 3);
  for (uint8_t i = 0; i < 4; i++) {
    if (i != currentDigit)displayBuffer.fontColor(inactiveFontColor, defaultFontBG);
    displayBuffer.print(digits[i]);
    if (i != currentDigit)displayBuffer.fontColor(defaultFontColor, defaultFontBG);
  }
  displayBuffer.print(F("   "));
  displayBuffer.fontColor(0xFFFF, ALPHA_COLOR);
  return 0;
}

//testing simple menu status function
void (*functionViewCallBack)() = NULL;

uint8_t functionView(uint8_t button, int *inVal, char *intName, void (*cb)()) {
  if (menu_debug_print)SerialMonitorInterface.println("editInt");
  if (!button) {
    if (menu_debug_print)SerialMonitorInterface.println("editIntInit");
    functionViewCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = functionView;

    displayBuffer.clearWindow(0, 8, 96, 64);
    writeArrows();

    displayBuffer.fontColor(defaultFontColor, defaultFontBG);
    int width = displayBuffer.getPrintWidth(intName); //displays function name
    displayBuffer.setCursor(96 / 2 - width / 2, menuTextY[3]);
    displayBuffer.print(intName);

    displayBuffer.setCursor(59, 15 - 3);
    displayBuffer.print("Enable");
    displayBuffer.setCursor(57, 45 + 3);
    displayBuffer.print("Return");

//runs whatever function was supplied in the functionView() call
  } else if (button == selectButton) {
      if (functionViewCallBack) {
        functionViewCallBack();
        functionViewCallBack = NULL;
      }
      return 1;
  } else if (button == backButton) {
      if (menu_debug_print)SerialMonitorInterface.println("back");
      viewMenu(backButton);
      return 0;
    }
  return 0;
}
//simple test program to print 9 on the screen upon function activation
void simpleRandGen()
{
    int x, y;
    x = 4;
    y = 5;
    displayBuffer.setCursor(24, 32);
    displayBuffer.print(x + y);
}
//end of test

void printCenteredAt(int y, char * text) {
  int width = displayBuffer.getPrintWidth(text);
  //displayBuffer.clearWindow(96 / 2 - width / 2 - 1, y, width + 2, 8);
  displayBuffer.clearWindow(10, y, 96 - 20, 10);
  displayBuffer.setCursor(96 / 2 - width / 2, y);
  displayBuffer.print(text);
}

int tempOffset = 0;

void saveTempCalibration() {
  tempOffset = constrain(tempOffset,0,20);
//  writeSettings();
}

void mainMenu(uint8_t selection) // selection = array index of the menu item
{
    if (selection == 0) //date time
    {
        newMenu(dateTimeMenuIndex);
    }
    if (selection == 1) //brightness
    {
        char buffer[20];
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[mainMenuIndex].strings[selection])));
        editInt(0, &brightness, buffer, setBrightnessCB);
    }
    if (selection == 2) //function menu creation
    {
        newMenu(functionMenuIndex);
    }
    if (selection == 3)
    {
        //TEST
        startBinary(1);
    }
    if (selection == 4)
    {
        //placeholder
    }
}

void setBrightnessCB()
{
    brightness = constrain(brightness, 0 , 15); //limits brightness to be solely within 0 - 15
}

void functionMenu(uint8_t selection)
{
    if (selection == 0) 
    {
        //program logic goes here for this specific option
    }
    if (selection == 1)
    {
        char buffer[20];
        strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[functionMenuIndex].strings[selection]))); //obtains the function name to print in the following function view
        functionView(0, &test, buffer, simpleRandGen); // follow this convention to call your functions the last args is the function name!
    }
    if (selection == 2) 
    {
        //placeholder
    }
    if (selection == 3)
    {
        //placeholder
    }
    if (selection == 4)
    {
        //placeholder
    }
}

//function test for printing text to screen
void testDisplay()
{
    displayBuffer.setCursor(24, 24);
    displayBuffer.print("Testing Testing");
}

uint8_t dateTimeSelection = 0;
int dateTimeVariable = 0;

void saveChangeCallback() {
  int timeData[] = {year(), month(), day(), hour(), minute(), second()};
  timeData[dateTimeSelection] = dateTimeVariable;
  setTime(timeData[3], timeData[4], timeData[5], timeData[2], timeData[1], timeData[0]);
  noInterrupts();
  counter = now();
  interrupts();
  if (menu_debug_print)SerialMonitorInterface.print("set time ");
  if (menu_debug_print)SerialMonitorInterface.println(dateTimeVariable);
}

void dateTimeMenu(uint8_t selection) {
  if (menu_debug_print)SerialMonitorInterface.print("dateTimeMenu ");
  if (menu_debug_print)SerialMonitorInterface.println(selection);
  if (selection >= 0 && selection < 6) {
    int timeData[] = {year(), month(), day(), hour(), minute(), second()};
    dateTimeVariable = timeData[selection];
    dateTimeSelection = selection;
    char buffer[20];
    strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[dateTimeMenuIndex].strings[selection])));
    editInt(0, &dateTimeVariable, buffer, saveChangeCallback);
  }
}


int changeDir;
int changeEnd;
int changeStart;
int yChange;

void drawMenu() {
  //for (int yChange = changeStart; yChange != changeEnd; yChange += changeDir) {
  if (needMenuDraw) {
    needMenuDraw = false;
    displayBuffer.clearWindow(0, 7, 96, 56);
    for (int i = 0; i < menuList[currentMenu].amtLines; i++) {
      if (i == currentSelectionLine) {
        displayBuffer.fontColor(defaultFontColor, ALPHA_COLOR);
      } else {
        displayBuffer.fontColor(inactiveFontColor, ALPHA_COLOR);
      }
      char buffer[20];
      strcpy_P(buffer, (PGM_P)pgm_read_word(&(menuList[currentMenu].strings[currentMenuLine + i])));
      int width = displayBuffer.getPrintWidth(buffer);
      displayBuffer.setCursor(96 / 2 - width / 2, menuTextY[i] + 5 + yChange - (currentSelectionLine * 8) + 16);
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
  if (menu_debug_print)SerialMonitorInterface.print("viewMenu ");
  if (menu_debug_print)SerialMonitorInterface.println(button);
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
      if (menu_debug_print)SerialMonitorInterface.print("select ");
      if (menu_debug_print)SerialMonitorInterface.println(currentMenuLine + currentSelectionLine);
      menuList[currentMenu].selectionHandler(currentMenuLine + currentSelectionLine);
    } else if (button == backButton) {
      newMenu(-1);
      if (!menuHistoryIndex)
        return;
    }
  }
  if (lastMenuLine != currentMenuLine || lastSelectionLine != currentSelectionLine) {
    if (menu_debug_print)SerialMonitorInterface.println("drawing menu ");
    if (menu_debug_print)SerialMonitorInterface.println(currentMenu);


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
