
/*
  Pet feed indicator

  Arduino Nano. Any version.

  Outputs
  Pin 2 - PM Pet fed LED 
  Pin 3 - PM Pet hungry LED 
  Pin 4 - AM Pet fed LED 
  Pin 5 - AM Pet hungry LED 
  
  Inputs
  Pin 7 - PM pet fed button
  Pin 8 - AM pet fed button
  


*/



#include <Arduino.h>
#include <Bounce2.h>

#define nextFeedHrs 6   // Time it takes for your pet to get hungry in hours and minutes
#define nextFeedMin 0   

#define blinkRate 500 // Blinking period in miliseconds

// INSTANTIATE Button OBJECTs
Bounce2::Button buttonPMFed = Bounce2::Button();
Bounce2::Button buttonAMFed = Bounce2::Button();

#define PMPetFedLEDPin    2
#define PMPetHungryLEDPin 3
#define AMPetFedLEDPin    4
#define AMPetHungryLEDPin 5

#define PMPetFedButtonPin 7
#define AMPetFedButtonPin 8


enum tummy {HUNGRY, FULL};
enum period {MORNING, NIGHT};

unsigned long timeNow = 0;
unsigned long timeLast = 0;
int seconds = 0;
int minutes = 0;
int hours = 0;
bool ledBlink = HIGH;

tummy myPetsTummy = HUNGRY;
period today = MORNING;

void updateLEDs() {
  // Clear all LEDs
  digitalWrite(AMPetFedLEDPin,LOW); 
  digitalWrite(AMPetHungryLEDPin,LOW);
  digitalWrite(PMPetFedLEDPin,LOW);
  digitalWrite(PMPetHungryLEDPin,LOW);

  // Set the one LED that should be ON
  if (today == MORNING) {
    if (myPetsTummy == FULL) {  // Pet is fed
      digitalWrite(AMPetFedLEDPin,HIGH);
    } else {  // Pet is hungry
      if (ledBlink) {
        digitalWrite(AMPetHungryLEDPin,HIGH);
      } else {
        digitalWrite(AMPetHungryLEDPin,LOW);
      }
    }
  } else {  // Time of day is night
    if (myPetsTummy == FULL) {  // Pet is fed
      digitalWrite(PMPetFedLEDPin,HIGH);
    } else {  // Pet is hungry
      if (ledBlink) {
        digitalWrite(PMPetHungryLEDPin,HIGH);
      } else {
        digitalWrite(PMPetHungryLEDPin,LOW);
      }
    }

  }
}

void setup() {
  
  // BUTTON SETUP 
  buttonPMFed.attach( PMPetFedButtonPin, INPUT_PULLUP );
  buttonAMFed.attach( AMPetFedButtonPin, INPUT_PULLUP );

  // DEBOUNCE INTERVAL IN MILLISECONDS
  buttonPMFed.interval(5); 
  buttonAMFed.interval(5); 

  // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
  buttonPMFed.setPressedState(LOW); 
  buttonAMFed.setPressedState(LOW); 
  
  // LED SETUP
  pinMode(PMPetFedLEDPin,OUTPUT);
  digitalWrite(PMPetFedLEDPin,LOW);

  pinMode(PMPetHungryLEDPin,OUTPUT);
  digitalWrite(PMPetHungryLEDPin,LOW);

  pinMode(AMPetFedLEDPin,OUTPUT);
  digitalWrite(AMPetFedLEDPin,LOW);

  pinMode(AMPetHungryLEDPin,OUTPUT);
  digitalWrite(AMPetHungryLEDPin,HIGH);

  // Initialise time-of-day monitoring
  timeNow = millis()/1000; // the number of seconds since boot
  timeLast = timeNow;

  // To assist debugging
  Serial.begin(9600);

}

void loop() {

  // Waiting for a full pet to become hungry again
  if (myPetsTummy == FULL) {

    timeNow = millis(); // The number of seconds since boot
    if ((timeNow - timeLast) >= 1000) {
      seconds = seconds + 1;
      timeLast = timeNow;
      Serial.println("Time: " + (String)hours + ":" + (String)minutes + ":" + (String)seconds);
    }
    // Catch wrap-around case
    if (timeLast > timeNow) {
      timeLast = timeNow;     
    }

    if (seconds == 60) {    // Check for minute elapsed
      timeLast = timeNow;
      seconds = 0;
      minutes = minutes + 1; 
   }

    if (minutes == 60){     // Check for hour elapsed
      seconds = 0;
      minutes = 0;
      hours = hours + 1; 
  }

  }

  // Check for a hungry pet
  if (hours >= nextFeedHrs && minutes >= nextFeedMin && myPetsTummy == FULL ) {
    myPetsTummy = HUNGRY;

    // Toggle LEDs
    if (today == MORNING) {
      today = NIGHT;
      Serial.println("Time of day: NIGHT");
      Serial.println("My pet is: HUNGRY");
      updateLEDs(); 

    } else {
      today = MORNING;
      Serial.println("Time of day: MORNING");
      Serial.println("My pet is: HUNGRY");
      updateLEDs(); 
    }
    
    // Reset time monitoring
    hours = minutes = seconds = 0;

  }

  // Check for pet being fed
  // Update the button status.  Must be called every loop.
  buttonPMFed.update();
  buttonAMFed.update();

  if (buttonPMFed.pressed()) {
    myPetsTummy = FULL;
    today = NIGHT;
    Serial.println("Time of day: NIGHT");
    Serial.println("My pet is: FULL");
    updateLEDs(); 
    hours = minutes = seconds = 0;
  }

  if (buttonAMFed.pressed()) {
    myPetsTummy = FULL;
    today = MORNING;
    Serial.println("Time of day: MORNING");
    Serial.println("My pet is: FULL"); 
    updateLEDs(); 
    hours = minutes = seconds = 0;
  }

  // Blink "pet hungry" LEDs
  // Check current miliseconds. 
  // If the value divided by blinkRate is even, turn the LED ON
  // If it is odd, turn the LED OFF
  if ( (millis() / (blinkRate/2)) % 2  == 1 ) {
    // Remainder is 0, LED should be ON
    if (ledBlink == LOW ) {
      ledBlink = HIGH;
      updateLEDs();
    }
  } else {  
    // Remainder is 1, LED should be OFF
    if (ledBlink == HIGH ) {
      ledBlink = LOW;
      updateLEDs();
    }
  }

}

