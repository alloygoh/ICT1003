#include "Keyboard.h"

char binaryCmdline[] = "start rcdob.exe";
char notifyConfig[] = "set RCDO_NOTIFY=0";
char apiKey[] = "set RCDO_KEY=whyare";

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
  delay(30);
  delay(500);
}

void setupConfig(int notify){
  Keyboard.print(apiKey);
  Keyboard.write(KEY_RETURN);
  delay(30);

  if (!notify){
    Keyboard.print(notifyConfig);
    Keyboard.write(KEY_RETURN);
    delay(30);    
  }
}

void startBinary(int *option){
  for (int i=0; i < 4; i++){
    SerialMonitorInterface.print(option[i]);
  }
  pinMode(LED_BUILTIN, OUTPUT);

  // blocking loop until plugged in
  while(1){
    float battVoltageReading = getBattVoltage();
    if (battVoltageReading < 4.25)
    {
      digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(500);                       // wait for a second
      digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
      delay(500);  
    }
    else{
      break; 
    }
  }

  // wait 1 sec to allow for higher success rates
  // due to some funky windows USB recognition
  delay(1000);
  Keyboard.begin();
  //runPS();
  setupConfig(option[3]);
  char buf[560];
  strcpy(buf,binaryCmdline);
  char *mods = selectModules(option);

  strcat(buf,mods);
  
  Keyboard.print(buf);
  Keyboard.write(KEY_RETURN);

  delay(500);
  Keyboard.print("exit");
  Keyboard.write(KEY_RETURN);

  delay(30);
  
  free(mods);
  Keyboard.end();
}
