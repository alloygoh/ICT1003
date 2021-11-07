/*
    Menu Functions and Libaries
    Written by: Ben Rose and Laveréna Wienclaw
*/

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
uint8_t (*editorHandler)(uint8_t, int*, /*int*,*/ void (*)()) = NULL;

//Struct for creating menus
typedef struct
{
  const uint8_t amtLines; //How many lines in said menu
  const char* const * strings; // Provide an Array of Strings per lines
  void (*selectionHandler)(uint8_t); //Handle to Menu
} menu_info;

//Main Menu Array Example
static const char PROGMEM mainMenuString0[] = "Function 1";
static const char PROGMEM mainMenuString1[] = "Function 2";
static const char PROGMEM mainMenuString2[] = "Function 3"; //Replace these instances with the function name
static const char PROGMEM mainMenuString3[] = "Function 4";
static const char PROGMEM mainMenuString4[] = "Function 5";
static const char PROGMEM mainMenuString5[] = "Function 6";
static const char PROGMEM mainMenuString6[] = "Function 7";


static const char* const PROGMEM mainMenuStrings[] =
{
  mainMenuString0, mainMenuString1, mainMenuString2, mainMenuString3, mainMenuString4,
  mainMenuString5, mainMenuString6,
};

const menu_info mainMenuInfo =
{
  7,
  mainMenuStrings,
  mainMenu,
};

const menu_info menuList[] = {mainMenuInfo};
#define mainMenuIndex 0

bool needMenuDraw = true;


//testing simple menu status function
void (*functionViewCallBack)() = NULL;
char args[5] = "UKMA";
int currentVal = 0;
// int array to hold values set
int toggle[4];
int currentArgs = 0;
int arraySize = 4;
int *originalVal;
//int *hasRan;
uint8_t functionView(uint8_t button, int *inVal,/*int *active,*/ void (*cb)()) {

  if (!button) {
    functionViewCallBack = cb;
    currentDisplayState = displayStateEditor;
    editorHandler = functionView;
    currentArgs = 0;
    originalVal = inVal;
    currentVal = *originalVal;
    toggle[3] = currentVal % 10; currentVal /= 10;
    toggle[2] = currentVal % 10; currentVal /= 10;
    toggle[1] = currentVal % 10; currentVal /= 10;
    toggle[0] = currentVal % 10;
    currentVal = *originalVal;

    displayBuffer.clearWindow(0, 8, 96, 64);
    writeArrows();
    //hasRan = active;
    //if (*hasRan == 1) {
    //  displayBuffer.fontColor(defaultFontColor, defaultFontBG);
    //  displayBuffer.setCursor(96 / 2 , menuTextY[4]);
    //  displayBuffer.print("Currently Active!");
    //}

    displayBuffer.setCursor(59, 15 - 3);
    displayBuffer.print("Enable");
    displayBuffer.setCursor(57, 45 + 3);
    displayBuffer.print("Return");


    //if(*hasRan == 1){
    //  *hasRan = 0;
    //} else{
    //    *hasRan = 1;
    //}
    
    } else if (button == upButton) {
    if (toggle[currentArgs] < 1)
      toggle[currentArgs]++;
  } else if (button == downButton) {
    if (toggle[currentArgs] > 0)
      toggle[currentArgs]--;
  } else if (button == selectButton) {
    if (currentArgs < arraySize - 1) {
      currentArgs++;
    } else {
      //save
      displayBuffer.clearWindow(5, menuTextY[2], 96, 20);
      int newValue = (toggle[3]) + (toggle[2] * 10) + (toggle[1] * 100) + (toggle[0] * 1000);
      *originalVal = newValue;
      if (functionViewCallBack) {
        functionViewCallBack();
        functionViewCallBack = NULL;
      }
      return 1;
    }
  } else if (button == backButton) {
    if (currentArgs > 0) {
      currentArgs--;
    } else {
      viewMenu(backButton);
      return 0;
    }
  }

  // print UMKA
  for (uint8_t i = 0; i < 4; i++) {
    if (i != currentArgs)displayBuffer.fontColor(inactiveFontColor, defaultFontBG);
    displayBuffer.setCursor(96 / 2 - 16 + i * 6 , menuTextY[2]);
    displayBuffer.print(args[i]);
    displayBuffer.setCursor(96 / 2 - 16 + i * 6, menuTextY[3] + 3);
    displayBuffer.print(toggle[i]);
    if (i != currentArgs)displayBuffer.fontColor(defaultFontColor, defaultFontBG);
  }
  
  return 0;

}

void buttonPress(uint8_t buttons) {
  if (currentDisplayState == displayStateHome) {
    //if (buttons == viewButton) {
    //  menuHandler = viewMenu;
    //  newMenu(mainMenuIndex);
    //  menuHandler(0);
    
    if (buttons == shortcutButton)
    {
      functionView(0, &toggled,/*&ran,*/ startExec); // follow this convention to call your functions the last args is the function name!  
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


  
// start binary execution based on commands
void startExec(){
  printCenteredAt(menuTextY[3], "Plug In USB");
  for (int i=0; i < 4; i++){
    SerialMonitorInterface.print(toggle[i]);
  }
  startBinary(toggle);
  printCenteredAt(menuTextY[3], "Done!");
}
//end of test

void printStatus(char * text){
  displayBuffer.clearWindow(5, menuTextY[3], 96, 10); // adjust the first arg if not wiping properly
  displayBuffer.setCursor(28, menuTextY[3]);
  displayBuffer.print(text);
}

void printCenteredAt(int y, char * text) {
  int width = displayBuffer.getPrintWidth(text);
  //displayBuffer.clearWindow(96 / 2 - width / 2 - 1, y, width + 2, 8);
  displayBuffer.clearWindow(5, y, 96, 10); // adjust the first arg if not wiping properly
  displayBuffer.setCursor(96 / 2 - width / 2, y);
  displayBuffer.print(F(text));
}

int tempOffset = 0;

void saveTempCalibration() {
  tempOffset = constrain(tempOffset, 0, 20);
  //  writeSettings();
}

void mainMenu(uint8_t selection) // selection = array index of the menu item
{
  if (selection == 0)
  {
    functionView(0, &toggled,/*&ran,*/ startExec); // follow this convention to call your functions the last args is the function name!
  }

  if (selection == 1)
  {
    //startBinary(1);
   
  }
  if (selection == 2)
  {

  }
  if (selection == 3)
  {
    //placeholder
  }
  if (selection == 4)
  {
    //placeholder
  }
  if (selection == 4)
  {
    //placeholder
  }
  if (selection == 4)
  {
    //placeholder
  }
  if (selection == 4)
  {
    //placeholder
  }
  if (selection == 4)
  {
    //placeholder
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
      displayBuffer.setCursor(0  + width / 4, menuTextY[i]  + yChange - (currentSelectionLine * 8) + 8);
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
