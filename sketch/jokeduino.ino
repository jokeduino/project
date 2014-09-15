/*
JokeDuino - full sketch
All things connected and checked?
Upload this sketch and you are ready to go.

*/

#include <MemoryFree.h>
#include <pgmStrToRAM.h>
#include <SD.h>             // need to include the SD library
#define SD_ChipSelectPin 4  //using digital pin 4 on arduino nano 328
#include <TMRpcm.h>         //  also need to include this library...
#include "Timer.h"

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


int   currentModeOption[4] = {0,0,0,0};
const int maxModeOption[4] = {0,4,5,1};
/*
here current option has to be stored separatelly per mode 
for mode0 - welcome message there is no options - "Hi I'm jokeDuino "
for mode1 - choose prank type there will be N options looping "choose different pranks by pressing B button"
            0 - moskito

for mode2 - set up volume options 4 options looping "change volume by pressing B button"
for mode3 - ready to go - no options "I'm ready to go do not press anything more - selected prank is: "
*/



void setup(){
  // reserve memory for commands over serial port
  inputString.reserve(20);
  
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
    //Serial.println("* is a card is inserted?");
    //Serial.println("* Is your wiring correct?");
    //Serial.println("* did you change the chipSelect pin to match your shield or module?");
    return;
  } else {
   Serial.println(F("Wiring is correct and a card is present.")); 
  }
  
  if (!SD.begin(SD_ChipSelectPin)) {  // see if the card is present and can be initialized:
    Serial.println(F("SD fail :-("));  
    return;   // don't do anything more if not
  }
  tmrpcm.play("intro/intro.wav"); //play the intro message

  t1ID = t1.every(CHECKLIGHTPERIOD, checkLightSensor);
  t2ID = t2.every(HOW_OTEN_TO_PRINT, printInfo);
  
}

void loop(){  

  // TODO: now add light sensor 
  // and play sounds only when it is dark 
  // do brakes between playing a sound
  
  
  readButtonA();
  readButtonB();
  t1.update();
  t2.update();
  
  
  if (stringComplete) {
    // received command from serial  
    changeCurrentModeBasedOnSerial();
    changeCurrentModeOptionBasedOnSerial();
    
    if(inputString == "mosq1x"){ //send the letter p over the serial monitor to start playback
      
      tmrpcm.stopPlayback();
      tmrpcm.play("insects/mosq1.wav");
    
    } else if(inputString == "mosq2x"){ //send the letter p over the serial monitor to start playback
  
      tmrpcm.stopPlayback();
      tmrpcm.play("insects/mosq2.wav");
  
    } else if(inputString == "nellyx"){ //send the letter p over the serial monitor to start playback
      
      tmrpcm.stopPlayback();
      tmrpcm.play("nelly.wav");
      
    } else if(inputString == "mosq1ax"){ //send the letter p over the serial monitor to start playback
      
      tmrpcm.stopPlayback();
      tmrpcm.play("insects/mosq1a.wav");
    
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
    
    inputString = "";
    stringComplete = false;
  }
  delay(1000);
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
  if(inputString == "modeUpx"){
    currentMode++;
    if(currentMode > maxMode){
      currentMode = 0;
    }
  }
}

void changeCurrentModeOptionBasedOnSerial(){
  if(inputString == "optionUpx"){
    currentModeOption[currentMode]++;
  }  
  if(currentModeOption[currentMode] > maxModeOption[currentMode] ){
    currentModeOption[currentMode] = 0;
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


