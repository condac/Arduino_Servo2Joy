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


long ch1IntTime;
long ch2IntTime;
long ch3IntTime;
long ch4IntTime;

void ch1inter() {
  long mic = micros();
  if(((PIND & _BV (1)) == 0)) { // pin is LOW signal done
    ch1Value = mic - ch1IntTime;
    ch1NewValue = true;
  } else {
    ch1IntTime = mic;
  }
  
}

void ch2inter() {
  long mic = micros();
  if(((PIND & _BV (0)) == 0)) { // pin is LOW signal done
    ch2Value = mic - ch2IntTime;
    ch2NewValue = true;
  } else {
    ch2IntTime = mic;
  }

}


bool ch3prevState;
bool ch4prevState;

void ch3nonblocking() {
  bool state = ((PIND & _BV (4)) == 0);
  if (state == ch3prevState) {
    return;
  } else {
    ch3prevState = state;
    long mic = micros();
    if(state) { // pin is LOW signal done
      int ch3ValueTemp = mic - ch3IntTime;
      if (ch3ValueTemp<2200 && ch3ValueTemp>850) {
        ch3Value = ch3ValueTemp;
        ch3NewValue = true;
      }
      
    } else {
      ch3IntTime = mic;
    }
  }
  
}
void ch4nonblocking() {
  bool state = ((PINC & _BV (6)) == 0);
  if (state == ch4prevState) {
    return;
  } else {
    ch4prevState = state;
    long mic = micros();
    if(state) { // pin is LOW signal done
      int ch4ValueTemp = mic - ch4IntTime;
      if (ch4ValueTemp<2200 && ch4ValueTemp>850) {
        ch4Value = ch4ValueTemp;
        ch4NewValue = true;
      }
      
    } else {
      ch4IntTime = mic;
    }
  }
  
}

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
    
void setup() {
  pinMode(PIN_CH1, INPUT);
  pinMode(PIN_CH2, INPUT);
  pinMode(PIN_CH3, INPUT);
  pinMode(PIN_CH4, INPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_CH1), ch1inter, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_CH2), ch2inter, CHANGE);
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





void loop() {


  ch3nonblocking();
  ch4nonblocking();

  if (ch1NewValue && ch2NewValue) {
      
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
      ch1NewValue = false;
      ch2NewValue = false;
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
  }
  
  //Serial.print("sendState");
  //Serial.println(btimer);
  
  
}

