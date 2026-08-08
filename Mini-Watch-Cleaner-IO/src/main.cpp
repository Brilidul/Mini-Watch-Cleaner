/**
 * From christoferjh :
 * 
 * Mini Watch Cleaner
 * Using A continous rotation servo and some 3d printing.
 * Running on an Arduino Pro Mini.
 * A 4 digit led tube for display.
 * 2 Buttons.
 * 
 * Hello, and warning. Here are some code. It works, but I have spend 0 time on making it good or pretty.
 * I'm uploading it on the chance that someone perhaps will find it usefull as a starting point.
 * 
 * 
 * From Brilidul:
 * I adapted this code for my servo and arduino model. 
 * Maybe some features will be added in the future, but for now it is a wip. (maybe adding an LCD screen ?)
 */

#include <Arduino.h>
#include <Servo.h>
#include "Button.h"


#include <DIYables_4Digit7Segment_74HC595.h> // DIYables_4Digit7Segment_74HC595 library

// Pin definitions for the 4 digit 7 segment display
#define SCLK  8  // The Arduino pin connected to SCLK
#define RCLK  7  // The Arduino pin connected to RCLK
#define DIO   6  // The Arduino pin connected to DIO
DIYables_4Digit7Segment_74HC595 display(SCLK, RCLK, DIO);

// Pin definitions for the buttons
#define BUTTON_A A0
#define BUTTON_B A1

// Pin definition for the servo
#define PIN_SERVO 16  //D16

// Servo speeds definitions
#define NORMAL_SPEED_SERVO 10
#define SLOW_SPEED_SERVO 2
// stop value for the servo
#define SERVO_STOP_VALUE 95

// Valeur pour piloter le servo continu en manuel ONLY FOR TESTS
#define MANUAL_SPEED_SERVO 30



enum class MODES {
  STOP,
  CLEANING,
  RINSING,
  SLOW_CLEANING
  // SPINNING/WHIRLING ? maybe in the future.
};

MODES activemode = MODES::STOP;
MODES selectmode = MODES::CLEANING;
unsigned long ulLastUpdate=0;
int iTimer = 0;
boolean bRotateClockwise = true;
int iDebounceTimer = 0;
Button * buttonA;
Button * buttonB;
Servo myservo;  // create Servo object to control a servo

void rotateServo(int speed,boolean clockwise);

// easy to remember function to stop the servo.
void stopServo(){
  myservo.write(SERVO_STOP_VALUE); // stop the servo
}

// Activates a cleaning mode, initializes its countdown and starts the servo at the mode's defined speed.
void startMode(MODES newMode){
  activemode = newMode;
  switch (activemode)
    {
    case MODES::STOP:
      stopServo();
      break;
    case MODES::CLEANING:
      iTimer = 300;
      bRotateClockwise = true;
      rotateServo(90,bRotateClockwise);
      break;
    case MODES::RINSING:
      iTimer = 180;
      bRotateClockwise = true;
      rotateServo(90,bRotateClockwise);
      break;
    case MODES::SLOW_CLEANING:
      iTimer = 300;
      bRotateClockwise = true;
      rotateServo(SLOW_SPEED_SERVO,bRotateClockwise);
      break;
    default:
      break;
    }
}

// When A button is pressed, starts the mode currently selected.
void callbackButtonA(char c){
  startMode(selectmode);
}

// When B button is pressed, stops an active mode, or cycles through the available modes when the cleaner is already stopped.
void callbackButtonB(char c){
  //Stops if active
  if (activemode!=MODES::STOP){
   startMode(MODES::STOP);
   selectmode = MODES::CLEANING;
  }else{
    //next mode
    switch (selectmode)
    {
    case MODES::STOP:
      selectmode = MODES::CLEANING;
      break;
    case MODES::CLEANING:
      selectmode = MODES::RINSING;
      break;
    case MODES::RINSING:
      selectmode = MODES::SLOW_CLEANING;
      break;
    case MODES::SLOW_CLEANING:
      selectmode = MODES::CLEANING;
      break;
    default:
      break;
    }
  }
}
    /**
     *  ___        7
     * |   |     2   6
     *  ---        1
     * |   |     3   5
     *  ---  .     4    0
     * 
     */
  /**
 *   
 * B11000000,  // 0
  B11111001,  // 1
  B10100100,  // 2
  B10110000,  // 3
  B10011001,  // 4
  B10010010,  // 5
  B10000010,  // 6
  B11111000,  // 7
  B10000000,  // 8
  B10010000,  // 9
  B10111111,  // - (dash)
  B11110111,  // _ (underscore)
  B11000110,  // C
  B10000110,  // E
  B10001110,  // F
  B11000111,  // L
  B11000111,  // A/R
  B10011100   // ° (degree)
  
  I have modified the 4 digit 7 segment code to allow for raw input, like this:
      void setRaw(int pos, byte value);

      void DIYables_4Digit7Segment_74HC595::setRaw(int pos, byte value) {
        if (pos < 1 || pos > 4)
          return;

        if (value < 0 || pos > 9)
          return;

        _digit_sets[pos - 1] = value;
      }

 */

// Displays the symbol representing a mode at the requested digit position.
void showModeAtPos(MODES mode,int pos){
  switch (mode)
  {
  case MODES::STOP:
    display.setNumber(pos, 5); 
    break;
  case MODES::CLEANING:
    display.setChar(pos, SegChars::C);
    break;
  case MODES::RINSING:
    //display.setChar(pos, SegChars::DEGREE);
    display.setRaw(pos, B10001000); // A/R ?
    break;
  case MODES::SLOW_CLEANING:
    //selectmode = MODES::CLEANING;
    display.setRaw(pos, B11000111); // L
    break;
  default:
    break;
  }
}

// Refreshes the display with the selected mode or the remaining time for the active mode.
void updateDisp(){

  display.clear();
  switch (activemode)
  {
  case MODES::STOP:
    showModeAtPos(selectmode,2);
    break;
  case MODES::CLEANING:
    display.printInt(iTimer, false);
    break;
  case MODES::SLOW_CLEANING:
    display.printInt(iTimer, false);
  break;
  case MODES::RINSING:
    display.printInt(iTimer, false);
  break;
  
  default:
    break;
  }
  showModeAtPos(activemode,1);
  display.setDot(1);                    // set . at the 1st digit
  display.show();                       // show on the display */

}

// Placeholder for a non-blocking delay implementation; currently it does nothing.
// I didn't delete this as i don't know if christoferjh will update this code and use it in the future.
void smartDelay(long time){

}

// Sets the servo speed and direction; values below or above the neutral command control opposite directions.
void rotateServo(int speed,boolean clockwise){
  if (clockwise){
    myservo.write(90-speed);

  }
  else{
    myservo.write(90+speed);
  }
}

// Decrements the mode timer and reverses the servo after each configured number of seconds.
//Run every 1 sec
void updateCleaningProgram(int secperpart,int speed){
  iTimer--;

  if (iTimer<=0) {
    startMode(MODES::STOP);
    
    return;
  }

  if (iTimer%secperpart==0){
    bRotateClockwise = !bRotateClockwise;
    rotateServo(speed,bRotateClockwise);
  }

}

///////////////////////////////////////////////////////
/////////// This section is Brilidul's code ///////////
///////////////////////////////////////////////////////

// Simple test: back-and-forth 
void testServo2() {
  myservo.write(SERVO_STOP_VALUE-SLOW_SPEED_SERVO);
  digitalWrite(LED_BUILTIN, true);
  delay(1000);
  digitalWrite(LED_BUILTIN, false);
  delay(100); 

  myservo.write(SERVO_STOP_VALUE);
  digitalWrite(LED_BUILTIN, true);
  delay(1000);
  digitalWrite(LED_BUILTIN, false);
  delay(100);

  myservo.write(SERVO_STOP_VALUE+MANUAL_SPEED_SERVO);
  digitalWrite(LED_BUILTIN, true);
  delay(1000);
  digitalWrite(LED_BUILTIN, false);
  
  stopServo();
  delay(500);
}

//////////////////////////////////////
/////////// End of Section ///////////
//////////////////////////////////////


// the setup function runs once when you press reset or power the board
// It configure the hardware, register button callbacks, and test the servo.
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  // initialize the buttons
  //pinMode(BUTTON_A, INPUT_PULLUP);
  //pinMode(BUTTON_B, INPUT_PULLUP);
  buttonA = new Button(BUTTON_A);
  buttonB = new Button(BUTTON_B);
  buttonA->init();
  buttonB->init();
  buttonA->setSelectorCharCallback('a',&callbackButtonA);
  buttonB->setSelectorCharCallback('b',&callbackButtonB);



  myservo.attach(PIN_SERVO);  // attaches the servo on pin 9 to the Servo object

  stopServo();
  ulLastUpdate = millis();

  testServo2(); // retirer après vérification
  stopServo();
}


// Runs continuously to poll both buttons;
void loop() {

  buttonA->update();
  buttonB->update();
 
  digitalWrite(LED_BUILTIN, (buttonA->isPressed() || buttonB->isPressed()) ? HIGH : LOW);

  if (buttonA->isPressed()) {
    myservo.write(0 - MANUAL_SPEED_SERVO); // rotation dans un sens
  } else if (buttonB->isPressed()) {
    myservo.write(0 + MANUAL_SPEED_SERVO); // rotation dans l'autre sens
  } 
  else 
  {
    stopServo();       // arrêt
  }

  unsigned long ulNow = millis();
  if (ulNow - ulLastUpdate>=1000){
    ulLastUpdate = ulNow;
    //every 1 sec
    switch (activemode)
    {
    case MODES::CLEANING:
      updateCleaningProgram(5,90);
      break;
    case MODES::SLOW_CLEANING:
      updateCleaningProgram(5,SLOW_SPEED_SERVO);
      break;
    case MODES::RINSING:
      updateCleaningProgram(500,90);
      break;
    default:
      break;
    }
  }
 
  updateDisp();

  display.loop(); // MUST call the display.loop() function in loop()
*/
}
