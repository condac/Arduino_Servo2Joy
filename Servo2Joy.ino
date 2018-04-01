// Program used to test the USB Joystick object on the 
// Arduino Leonardo or Arduino Micro.
//
// Matthew Heironimus
// 2015-03-28
// Updated on 2015-11-18 to use the new Joystick library written for version 1.6.6.
//------------------------------------------------------------

#include "Joystick.h"

#define PIN_CH1 2
#define PIN_CH2 3
#define PIN_CH3 4
#define PIN_CH4 5

/*Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_MULTI_AXIS, 4, 0,
  false, false, false, false, false, false,
  false, false, true, true, true);*/

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID,JOYSTICK_TYPE_GAMEPAD,
  2, 0,                  // Button Count, Hat Switch Count
  true, true, false,     // X and Y, but no Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering
  
// Set to true to test "Auto Send" mode or false to test "Manual Send" mode.
//const bool testAutoSendMode = true;
const bool testAutoSendMode = false;

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;

int flippflopp = 1500;
int ch1Value;
int ch2Value;
int ch3Value;
int ch4Value;

bool mode;
long lastTimeBlink;

bool chWorking[10] = {true, true, true, true, true, true, true, true, true, true};
int chErrors[10];

int banan;
long timerstart;
bool ch1Running = false;
bool ch2Running = false;
bool ch3Running = false;
bool ch4Running = false;
bool ch1NewValue = false;
bool ch2NewValue = false;
bool ch3NewValue = false;
bool ch4NewValue = false;
long temp1;
long temp2;
long temp3;
long temp4;
int updates = 0;

bool readChannel(int ch) {
  if (chWorking[ch] ) {
    return digitalRead(ch);
  } else {
    return false;
  }
}

void reportErrorCh(int ch) {
  chErrors[ch]++;
  if ( chErrors[ch]>100) {
    chWorking[ch] = false;
    chErrors[ch] = 200;
    Serial.print("Disable error channel");
    Serial.print(ch);
  }
}
int readLoop() {
  timerstart = micros();
  ch1Running = false;
  ch2Running = false;
  ch3Running = false;
  ch4Running = false;
  ch1NewValue = false;
  ch2NewValue = false;
  ch3NewValue = false;
  ch4NewValue = false;
        
        
  if (digitalRead(PIN_CH1) ) {
    ch1Running = true;
    ch1NewValue = true;
  }
  if (digitalRead(PIN_CH2) ) {
    ch2Running = true;
    ch2NewValue = true;
  }
  if (readChannel(PIN_CH3) ) {
    ch3Running = true;
    ch3NewValue = true;
  }
  if (readChannel(PIN_CH4) ) {
    ch4Running = true;
    ch4NewValue = true;
  }
  long micro = 0;
  while( ch1Running || ch2Running || ch3Running || ch4Running) {
    micro++;
    
    if (((PIND & _BV (1)) == 0) && ch1Running) {
      temp1 = micros();
      ch1Running = false;
    }
    if (((PIND & _BV (0)) == 0) && ch2Running) {
      temp2 = micros();
      ch2Running = false;
    }

    //40 steg med digitalRead ((PIND & _BV (1)) == 0)
    if (((PIND & _BV (4)) == 0) && ch3Running) {
      temp3 = micros();
      ch3Running = false;
    }
    if (((PINC & _BV (6)) == 0) && ch4Running) {
      temp4 = micros();
      ch4Running = false;
    }
    
    if (micro > 3000) {
      temp1 = micro;
      temp2 = micro;
      //Serial.println("TIMEOUT");
      if ( ch1Running ) {
        ch1NewValue = true;
        Serial.println("TIMEOUT ch1");
      }
      if ( ch2Running ) {
        ch2NewValue = true;
        Serial.println("TIMEOUT ch2");
      }
      if ( ch3Running ) {
        ch3NewValue = false;
        reportErrorCh(PIN_CH3);
        Serial.println("TIMEOUT ch3");
      }
      if ( ch4Running ) {
        ch4NewValue = false;
        reportErrorCh(PIN_CH4);
        Serial.println("TIMEOUT ch4");
      }
      break;
    }
  }
  Serial.println(micro);
  if (ch1NewValue) {
    ch1Value = temp1 - timerstart;
    updates++;
  }
  if (ch2NewValue) {
    ch2Value = temp2 - timerstart;
    updates++;
  }
  if (ch3NewValue) {
    ch3Value = temp3 - timerstart;
    updates++;
  }
  if (ch4NewValue) {
    ch4Value = temp4 - timerstart;
    updates++;
  }
}

bool readServoParalell() {
  

  
  //Serial.println("begin");
  // Check if there is an ongoing command and wait for it to end
  if (readChannel(PIN_CH1) || readChannel(PIN_CH2) || readChannel(PIN_CH3) || readChannel(PIN_CH4)) {
    //Serial.println("Wait command to end");
    while(readChannel(PIN_CH1) || readChannel(PIN_CH2 || readChannel(PIN_CH3) || readChannel(PIN_CH4)) ){
      
      banan++;
    }
  }
  //Serial.println("Wait for next command");
  // Wait for next command
  while(!readChannel(PIN_CH1) && !readChannel(PIN_CH2) && !readChannel(PIN_CH3) && !readChannel(PIN_CH4)){
        
    banan++;
  }
  readLoop();


  
  if (updates == 1) {
    
      if (readChannel(PIN_CH1) || readChannel(PIN_CH2) || readChannel(PIN_CH3) || readChannel(PIN_CH4) ) {
        //Serial.println("Direct message after previus 1");
        readLoop();
        if (readChannel(PIN_CH1) || readChannel(PIN_CH2) || readChannel(PIN_CH3) || readChannel(PIN_CH4) ) {
          //Serial.println("Direct message after previus 2");
          readLoop();
          if (readChannel(PIN_CH1) || readChannel(PIN_CH2) || readChannel(PIN_CH3) || readChannel(PIN_CH4) ) {
            //Serial.println("Direct message after previus 3");
            readLoop();
            
          }
        }
        return false;
      }
      
  
    return true;
  }
  return false;
  /*Serial.print("ch1");
  Serial.println(ch1Value);
  Serial.print("ch2");
  Serial.println(ch2Value);*/

}


void setup() {
  pinMode(PIN_CH1, INPUT);
  pinMode(PIN_CH2, INPUT);
  pinMode(PIN_CH3, INPUT);
  pinMode(PIN_CH4, INPUT);
  
  Joystick.setXAxisRange(0, 3000);
  Joystick.setYAxisRange(0, 3000);
  
  if (testAutoSendMode)
  {
    Joystick.begin();
  }
  else
  {
    Joystick.begin(false);
  }
  lastTimeBlink = millis();
  

  Serial.begin(115200);
  Serial.println("Starting program");
  pinMode( LED_BUILTIN_RX, OUTPUT);
  digitalWrite(LED_BUILTIN_RX,HIGH);
}


long updateRate;
long lastTimeBlink2;
bool blinkstate;
int blinkcounter;
long btimer;
bool error = false;
int firsttime = 0;

void blink() {
  blinkcounter++;
  if (blinkcounter > 50) {
    blinkcounter = 0;
    blinkstate = !blinkstate;
    //digitalWrite(LED_BUILTIN_RX,blinkstate);
    if (blinkstate) {
      TXLED1;
    } else {
      TXLED0;
    }
    
  }
  
}

void loop() {

  //readServo();
  //benchTest();
  updates = 0;
  bool tryagain = readServoParalell();
  
  while (tryagain) {
    //Serial.println("read again");
    tryagain = readServoParalell();
    
  }
  Serial.print("ch1:");
  Serial.print(ch1Value);
  Serial.print("ch2:");
  Serial.println(ch2Value);
  btimer = micros();
  Joystick.setXAxis(ch1Value);
  Joystick.setYAxis(ch2Value);
  if (ch3Value>1600) {
    Joystick.setButton(0, true);
  } else {
    Joystick.setButton(0, false);
  }
  if (ch4Value>1600) {
    Joystick.setButton(1, true);
  } else {
    Joystick.setButton(1, false);
  }
  Joystick.sendState();
  btimer = micros() - btimer;
  if (btimer>1000) {
    digitalWrite(LED_BUILTIN_RX,error);
    error = !error;
  }
  blink();
  if(firsttime<100 && btimer < 1000) {
    digitalWrite(LED_BUILTIN_RX,HIGH);
    firsttime++;
  }
  //Serial.print("sendState");
  //Serial.println(btimer);
  
  
}

