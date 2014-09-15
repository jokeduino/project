/*
JokeDuino - How to connect push button directly to digital input 
with no resitor - (using the build in pull up one)

Connect just the buttons and test them before putting all components together 

                BUTTON A   
GND o------------o/ o-----------o PIN D7

                BUTTON B   
GND o------------o/ o-----------o PIN D8


Upload this sketch to your arduino board and open the Serail Monitor

The output should be:


selected mode (buttonA): 0  selected option (buttonB): 0  (max option: 0)
selected mode (buttonA): 0  selected option (buttonB): 0  (max option: 0)
...
press button A
selected mode (buttonA): 1  selected option (buttonB): 0  (max option: 4)
selected mode (buttonA): 1  selected option (buttonB): 0  (max option: 4)
...
press button A again 
selected mode (buttonA): 2  selected option (buttonB): 0  (max option: 5)
selected mode (buttonA): 2  selected option (buttonB): 0  (max option: 5)
...
press button B
selected mode (buttonA): 2  selected option (buttonB): 1  (max option: 5)
selected mode (buttonA): 2  selected option (buttonB): 1  (max option: 5)
... 
etc. etc. ...


Big Thanks and credit to: 
https://www.youtube.com/watch?v=jJnD6LdGmUo
*/

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

/*
 * Variable used for counter
 */

int counter = 0;  // counter is a simple mechanism to cut off printing to serial - we print only when counter == MAX_COUNTER
const int MAX_COUNTER = 20000; 

void setup(){
  Serial.begin(9600);
    
  pinMode(buttonPinA, INPUT_PULLUP);    // this enable build in pull up resistor for pin 12
                                        // so now button can be connected between pin 12 and ground without any resistor
  pinMode(buttonPinB, INPUT_PULLUP);    
  
  Serial.println("All set up");
 }

 void loop(){
   
   readButtonA();
   readButtonB();

   if(counter==MAX_COUNTER){
     counter = 0;  
     printInfoAboutCurrentlySelectedModesAndOptions();
   }
   counter++;
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

void printInfoAboutCurrentlySelectedModesAndOptions(){
     Serial.println("");
     Serial.print("selected mode (buttonA): ");
     Serial.print(currentMode, DEC);
     Serial.print("\tselected option (buttonB): ");
     Serial.print(currentModeOption[currentMode], DEC);
     Serial.print("\t(max option: ");
     Serial.print( maxModeOption[currentMode], DEC);
     Serial.print(")");
}