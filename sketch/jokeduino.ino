/*
JokeDuino - full sketch
All things connected and checked?
Upload this sketch and you are ready to go.

Some notes:

Free ram on start:
without PROGMEM for all static strings 344
with PROGMEM for all atatic strings 504

watch it if free memory at start is <= 300
then playback does not work 
The minimum at start has to be 340 
*/

#include <avr/pgmspace.h>
#include <MemoryFree.h>
#include <pgmStrToRAM.h>
#include <SD.h>             // need to include the SD library
#define SD_ChipSelectPin 4  // using digital pin 4 on arduino nano 328
#include <TMRpcm.h>         // also need to include this library...
#include "Timer.h"

#define DEBUG  // comment this line out to disable debug statements

#ifdef DEBUG
  #define DEBUG_PRINTLN(x)  Serial.println(F(x))
#else
  #define DEBUG_PRINT(x)
#endif

TMRpcm tmrpcm;   // create an object for use in this sketch
Sd2Card card;

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
int lightLevel;

// constants
const int darkLevel = 500; //900;
boolean isItDark = true;  // whether the string is complete

const unsigned long CHECKLIGHTPERIOD = 250;    //250ms
Timer t1;                               //instantiate the timer object
int t1ID;

const unsigned long HOW_OTEN_TO_PRINT = 1000;  //1s
Timer t2;            //instantiate the timer object
int t2ID;

const unsigned long HOW_OTEN_TO_PRANK = 1 * 20 * 1000;  //5min
Timer t3;            //instantiate the timer object
int t3ID;


/*
 * Variables used by button A and B
 *
 */

//constants:
const int buttonPinA = 7;
const int buttonPinB = 8;
const long BUTTON_DEBOUNCE_DELAY = 50;  // the debounce time; increase if the output flickers


// variables:
int ledStateA = HIGH;         // the current state of the output pin
int buttonStateA;             // the current reading from the input pin
int lastButtonStateA = LOW;   // the previous reading from the input pin

int ledStateB = HIGH;         // the current state of the output pin
int buttonStateB;             // the current reading from the input pin
int lastButtonStateB = LOW;   // the previous reading from the input pin

// the following variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long lastDebounceTimeB = 0;   // the last time the output pin was toggled
long lastDebounceTimeA = 0;   // the last time the output pin was toggled



/*
Vriables used to store MODES and OPTIONS
*/

// we have 4 modes
int currentMode = 0;
const int maxMode = 3;
boolean modeChanged = false;

int   currentModeOption[4] = {-1,-1, 2,-1};
const int maxModeOption[4] = { 0, 4, 4, 1};
boolean optionChanged = false;

/*
here current option has to be stored separatelly per mode 
for mode0 - welcome message there is no options - "Hi I'm jokeDuino "
for mode1 - choose prank type there will be N options looping "choose different pranks by pressing B button"
            0 - moskito

for mode2 - set up volume options 0 - 5 options looping "change volume by pressing B button" (we start from level 3 - there is 7 levels of volumes but the last ones 6 and 7 do not work for us )
for mode3 - ready to go - no options "I'm ready to go do not press anything more - selected prank is: "
*/

prog_char intro[] PROGMEM = { "modes/intro.wav" };

prog_char mo0[]  PROGMEM = { "modes/mo0.wav" };

prog_char mo1[]    PROGMEM = { "modes/mo1.wav" };
// mode 1 option sounds
prog_char mo1op0[] PROGMEM = { "modes/mo1op0.wav" };  // mosquito
prog_char mo1op1[] PROGMEM = { "modes/mo1op1.wav" };  // door
prog_char mo1op2[] PROGMEM = { "modes/mo1op2.wav" };  // police
prog_char mo1op3[] PROGMEM = { "modes/mo1op3.wav" };  // dog
prog_char mo1op4[] PROGMEM = { "modes/mo1op4.wav" };  // cat

/* Actuall sounds used in pranks for each mode */
prog_char smo1op0[] PROGMEM = { "insects/mosq1.wav" };
prog_char smo1op1[] PROGMEM = { "doors/door1.wav" };
prog_char smo1op2[] PROGMEM = { "insects/mosq1.wav" };  //TODO: choose sounds
prog_char smo1op3[] PROGMEM = { "insects/mosq1.wav" };
prog_char smo1op4[] PROGMEM = { "insects/mosq1.wav" };

prog_char* selectedPrank = smo1op0;
          
prog_char mo2[] PROGMEM = { "modes/mo2.wav" };
prog_char mo2op[] PROGMEM = { "modes/mo2op.wav" }; // TODO: record /modes/mo2o.wav "sound level testing, sound level testing"

prog_char mo3[] PROGMEM = { "modes/mo3.wav" };


// ==========================================
// commands from serial
// ==========================================

prog_char modeupx[] PROGMEM = { "modex" };
prog_char optionupx[] PROGMEM = { "optionx" };

// ==========================================

void setup(){
  // reserve memory for commands over serial port
  inputString.reserve(10);
  
  pinMode(buttonPinA, INPUT_PULLUP);    // this enable build in pull up resistor for pin 12
  pinMode(buttonPinB, INPUT_PULLUP);    

  
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH); // davekw7x: If it's low, the Wiznet chip corrupts the SPI bus
  
  tmrpcm.speakerPin = 9; // set the speaker pin to 9

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  } 
  
  
  if (!card.init(SPI_HALF_SPEED, SD_ChipSelectPin)) {
    Serial.println(F("initialization failed. Things to check:"));
    Serial.println(F("* is a card is inserted?"));
    Serial.println(F("* Is your wiring correct?"));
    //Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   Serial.println(F("Wiring is correct and a card is present.")); 
  }
  
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println(F("SD fail :-("));  
    return;   // don't do anything more if not
  }
  
  t1ID = t1.every(CHECKLIGHTPERIOD, checkLightSensor);
  
  #ifdef DEBUG
    t2ID = t2.every(HOW_OTEN_TO_PRINT, printInfo);
  #else
    // print it just once here
    printInfo(); 
  #endif
  
  t3ID = t3.every(HOW_OTEN_TO_PRANK, prank);
  tmrpcm.play(getString(intro)); //play the intro message
}

void loop(){  

  // TODO: now add light sensor 
  // and play sounds only when it is dark 
  // do brakes between playing a sound
  
  
  readButtonA();
  readButtonB();
  t1.update();
  t2.update();
  t3.update();
  
  if(modeChanged==true){
    checkModes();
  }
  
  if(optionChanged==true){
    checkOptions();
  }
  
  
  
  if (stringComplete) {
    // received command from serial  
    changeCurrentModeBasedOnSerial();
    changeCurrentModeOptionBasedOnSerial();
    
    /*
    if(inputString == "mosq1x"){ //send the letter p over the serial monitor to start playback
      
      tmrpcm.stopPlayback();
      tmrpcm.play("insects/mosq1.wav");
    
    } else if(inputString == "door1x"){ //send the letter p over the serial monitor to start playback
      // here test the 
      tmrpcm.stopPlayback();
      tmrpcm.play("doors/door1.wav");
    
    } else if(inputString == "qx"){ //send the letter p over the serial monitor to start playback
      // here test the 
      tmrpcm.stopPlayback();

    } else if(inputString == "dx"){ 
      tmrpcm.volume(0);
    } else if(inputString == "ux"){ 
      tmrpcm.volume(1);
    }
    */
    
    inputString = "";
    stringComplete = false;
  }
  delay(500);
}


void checkLightSensor(){
  lightLevel = analogRead(A0);
  if(lightLevel > darkLevel ){
    isItDark = true;
    // here start some timer 
  }else{
    isItDark = false;
    //TODO: here instead of stopping directly change flag
    // this way we will have more control e.g. do not stop the intro message
    tmrpcm.stopPlayback();
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == 'x') {
      stringComplete = true;
    } 
  }
}

/*************** 
 SERIAL FUNCTIONS
 ****************/


void printInfo(){
  Serial.println(F(""));
  Serial.print(F("Free RAM: "));
  Serial.print(freeMemory(), DEC); 
  Serial.print(F("\tLight level: "));
  Serial.print(lightLevel, DEC);

   Serial.print(F("\tselected mode (buttonA): "));
   Serial.print(currentMode, DEC);
   Serial.print(F("\tselected option (buttonB): "));
   Serial.print(currentModeOption[currentMode], DEC);
   Serial.print(F("\t(max option: "));
   Serial.print( maxModeOption[currentMode], DEC);
   Serial.print(F(")"));
}

void changeCurrentModeBasedOnSerial(){
  if(inputString == getString(modeupx)){
    currentMode++;
    if(currentMode > maxMode){
      currentMode = 0;
    }
    modeChanged=true;
  }
}

void changeCurrentModeOptionBasedOnSerial(){
  if(inputString == getString(optionupx)){
    currentModeOption[currentMode]++;
    if(currentModeOption[currentMode] > maxModeOption[currentMode] ){
      currentModeOption[currentMode] = 0;
    }
    optionChanged=true;
  }  
}

/*************** 
 BUTTON FUNCTIONS
 ****************/
 
 void readButtonA(){
    // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPinA);
  
  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonStateA) {
    // reset the debouncing timer
    lastDebounceTimeA = millis();
  } 
  
  if ((millis() - lastDebounceTimeA) > BUTTON_DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
   // if the button state has changed:
    if (reading != buttonStateA) {
      buttonStateA = reading;

      // action when button in LOW state
      if (buttonStateA == LOW) {
         currentMode++;
         if(currentMode > maxMode){
           currentMode = 0;
         }
         modeChanged = true;
      }
    }
  }
   // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonStateA = reading;
 }
 
 
 void readButtonB(){
      // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPinB);
  
  // check to see if you just pressed the button 
  // (i.e. the input went from LOW to HIGH),  and you've waited 
  // long enough since the last press to ignore any noise:  

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonStateB) {
    // reset the debouncing timer
    lastDebounceTimeB = millis();
  } 
  
  if ((millis() - lastDebounceTimeB) > BUTTON_DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:
    // if the button state has changed:
    if (reading != buttonStateB) {
      buttonStateB = reading;

      // action when button in LOW state
      if (buttonStateB == LOW) {
         
         currentModeOption[currentMode]++;
         
         if(currentModeOption[currentMode] > maxModeOption[currentMode] ){
            currentModeOption[currentMode] = 0;
         } 
      }
    }
  }
  lastButtonStateB = reading;
}

void checkModes(){
    if(currentMode==0){
        tmrpcm.stopPlayback();
        tmrpcm.play(getString(mo0));
    }else if(currentMode==1){
        tmrpcm.stopPlayback();
        tmrpcm.play(getString(mo1));
    }else if(currentMode==2){
        tmrpcm.stopPlayback();
        tmrpcm.play(getString(mo2));
    }else if(currentMode==3){
        tmrpcm.stopPlayback();
        tmrpcm.play(getString(mo3));
    }
    modeChanged = false;
}

void  checkOptions(){
  if(optionChanged==true){
     if(currentMode==1){
       // choose prank type 
       switch (currentModeOption[1]) {
        case 0:
          selectedPrank = smo1op0;
          tmrpcm.stopPlayback();
          tmrpcm.play(getString(mo1op0));
          break;
        case 1:
          selectedPrank = smo1op1;
          tmrpcm.stopPlayback();
          tmrpcm.play(getString(mo1op1));
          break;
        case 2:
          selectedPrank = smo1op2;
          tmrpcm.stopPlayback();
          tmrpcm.play(getString(mo1op2));
          break;
        case 3:
          selectedPrank = smo1op3;
          tmrpcm.stopPlayback();
          tmrpcm.play(getString(mo1op3));
          break;
        case 4:
          selectedPrank = smo1op4;
          tmrpcm.stopPlayback();
          tmrpcm.play(getString(mo1op4));
          break;
        }
       //TODO: remove it
       Serial.println(F(""));
       Serial.println(F("==================="));
       Serial.println(getString(selectedPrank));
       Serial.println(F("==================="));
       
    }else if(currentMode==2){
      // volume mode 
      tmrpcm.stopPlayback();
      tmrpcm.setVolume(currentModeOption[2]);
      tmrpcm.play(getString(mo2op));
    } 
  
  optionChanged=false;
  }
}


#define MAX_STRING 30  //TODO: count longest string 
char stringBuffer[MAX_STRING];

char* getString(const char* str) {
  strcpy_P(stringBuffer, (char*)str);
  return stringBuffer;
}

void prank(){
  // here it might be different for different pranks 
  // but for now - check the light and ...
  Serial.println(F("==========================="));
  Serial.println(F("==== GOING TO PRANK NOW ==="));
  Serial.println(F("==========================="));
  
  tmrpcm.stopPlayback();
  tmrpcm.play(getString(selectedPrank));
}
