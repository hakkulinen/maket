#include <Button.h>
#include <Servo.h>
//HIGH = закрыто, LOW = открыто
#define SPEL_BUTTON_PIN A1
#define SPEL_LIGHT_PIN 12

#define UFO_BUTTON_PIN A3
#define UFO_RED_LIGHT_PIN 9
#define UFO_BLUE_LIGHT_PIN 10

#define WORM_BUTTON_PIN A2
#define WORM_LIGHT_PIN 11
#define WORM_SERVO_PIN 5

bool isRed = true;
bool spelPressed = false;
bool ufoPressed = false;
bool wormPressed = false;
bool wormDirection = false;
unsigned long every20seconds_spel = 0;
unsigned long every20seconds_ufo = 0;
unsigned long every20seconds_worm = 0;
unsigned long every700millis = 0;
unsigned long every60millis = 0;
byte wormAngle = 100;

Button spelButton(SPEL_BUTTON_PIN);
Button ufoButton(UFO_BUTTON_PIN);
Button wormButton(WORM_BUTTON_PIN);
Servo worm;

bool WaitMillis(unsigned long* t, long waitMillis) { 
   if ((millis() - *t) > waitMillis) { 
     *t = millis(); 
     return true; 
   } 
   if (millis() < *t) *t = millis();
   return false; 
}

void setup() {
  wormButton.begin();
  ufoButton.begin();
  spelButton.begin();
  //Serial.begin(9600);
  pinMode(SPEL_LIGHT_PIN, OUTPUT);
  pinMode(UFO_RED_LIGHT_PIN, OUTPUT);
  pinMode(UFO_BLUE_LIGHT_PIN, OUTPUT);
  pinMode(WORM_LIGHT_PIN, OUTPUT);
  worm.attach(WORM_SERVO_PIN);
  worm.write(170);
  every20seconds_worm = millis();
  every20seconds_ufo = millis();
  every20seconds_spel = millis();
  every700millis = millis();
  every60millis = millis();
   //close the gates
  digitalWrite(SPEL_LIGHT_PIN, HIGH);
  digitalWrite(UFO_RED_LIGHT_PIN, HIGH);
  digitalWrite(UFO_BLUE_LIGHT_PIN, HIGH);
  digitalWrite(WORM_LIGHT_PIN, HIGH); 
}
//button3.toggled()
void loop() {
  if(!wormPressed && wormButton.toggled()) {
    digitalWrite(WORM_LIGHT_PIN, LOW);
    wormPressed = true;
  }
  if (wormPressed) {
    if (WaitMillis(&every20seconds_worm, 20000)) {
    digitalWrite(WORM_LIGHT_PIN, HIGH);
    wormPressed = false;
    } 
  }

  if(!ufoPressed && ufoButton.toggled()) {
    ufoPressed = true;
  }

  if (ufoPressed) {
    if (WaitMillis(&every20seconds_ufo, 20000)) {
    digitalWrite(UFO_RED_LIGHT_PIN, HIGH);
    digitalWrite(UFO_BLUE_LIGHT_PIN, HIGH);
    ufoPressed = false;
    } 
  }

  if(!spelPressed && spelButton.toggled()) {
    digitalWrite(SPEL_LIGHT_PIN, LOW);
    spelPressed = true;
  }
  if (spelPressed) {
    if (WaitMillis(&every20seconds_spel, 20000)) {
    digitalWrite(SPEL_LIGHT_PIN, HIGH);
    spelPressed = false;
    } 
  }

 if (ufoPressed) {
    if (WaitMillis(&every700millis, 700)) {
      if (isRed) {
        digitalWrite(UFO_RED_LIGHT_PIN, LOW);
        digitalWrite(UFO_BLUE_LIGHT_PIN, HIGH);
        isRed = false;
      }
      else {
        digitalWrite(UFO_RED_LIGHT_PIN, HIGH);
        digitalWrite(UFO_BLUE_LIGHT_PIN, LOW);
        isRed = true;
      }
    }
  }

  if (wormPressed) {
    if (WaitMillis(&every60millis, 60)) {
      if (wormAngle <= 100) {
        wormAngle = 100;
        wormAngle++;
        wormDirection = true;
      }
      if (wormAngle >= 170) {
        wormAngle = 170;
        wormAngle--;
        wormDirection = false;
      }
      if (wormDirection) {
        worm.write(wormAngle);
        wormAngle++;
      }
      else {
        worm.write(wormAngle);
        wormAngle--;
      }
    }
  }
}

