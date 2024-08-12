#include <Button.h> //simple library to control a button (and account for jitter and all that button weirdness)
#include <VarSpeedServo.h> //very nice library for smooth servo control

//Button LED pins
#define RED_PIN 3
#define GREEN_PIN 4
//Motor control pins
#define LEFT_MOTOR_PIN 5
#define RIGHT_MOTOR_PIN 6
//Servo attachment pins
#define DOOR_PIN 10
#define TELESCOPE_PIN 11

//creating two servo objects with self-explanatory names
VarSpeedServo door;
VarSpeedServo telescope;
//creating two button objects with somewhat self-explanatory names
Button limitSwitch(2);
Button button(8);


//declaring some values to be used
uint32_t currentTime = 0; //used for the button in void loop()
bool isCalibrated = false; //used in calibrate() and sequence(), self-explanatory

//lights up the button RED
void lightRed() {
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, LOW);
}
//lights up the button GREEN
void lightGreen() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
}

//spins the motor RIGHT at a set speed (0-255)
void spin_right(int speed) {
  analogWrite(RIGHT_MOTOR_PIN, speed);
  digitalWrite(LEFT_MOTOR_PIN, LOW);
}
//spins the motor LEFT at a set speed (0-255)
void spin_left(int speed) { 
  analogWrite(LEFT_MOTOR_PIN, speed);
  digitalWrite(RIGHT_MOTOR_PIN, LOW);
} 
//locks down the motor, no spinning, full stop
void stop_spin() {
  digitalWrite(RIGHT_MOTOR_PIN, LOW);
  digitalWrite(LEFT_MOTOR_PIN, LOW);

}

/* the calibration function has a very simple method of operation. Since I only have one limit switch, initially I force the whole observatory
to spin to one end for about 9 seconds, which is the time it takes for a full spin from the other side which ensures that no matter where it is
initially, it will end up stuck at the end (tugging at wires that will definitely eventually rip apart) Then the whole thing spins back around
and awaits a button press, after which it goes on for a bit longer into it's initial desired position. */
void calibrate() {
  //there's also a little servo test + calibration, that just pops them into their basic states: doors closed, telescope down
  door.attach(DOOR_PIN); 
  door.write(170, 20, true);
  door.detach();
  telescope.attach(11);
  telescope.write(5, 20, true);
  telescope.detach();
  spin_right(100);
  delay(9000); //this may change with wire length and/or future reconstruction of the innerds of the observatory
  spin_left(100);
  while (!isCalibrated) {
    if (limitSwitch.pressed()) {
      isCalibrated = true;
      delay(3000); //gets you back to your desired initial position
      stop_spin();
      delay(500); //wait a bit just to be sure;
      break;
    }
  }
}

void sequence() {
  //reminder: servo.write(a, b, TRUE) is a BLOCKING method, do FALSE for non-blocking

  lightRed(); //signify the beginning of our sequence with a red light

  door.attach(DOOR_PIN); //enable door
  door.write(60, 7, true); //slowly open the door
  door.detach(); //disable door !!!VERY IMPORTANT, servos are powered through the arduino itself
  //which is kinda bad, but whatever. The arduino can't push more than about an amp of current through itself (and that current also needs to power the chip)
  //if you don't disable your servos or move them too fast the arduino will die and reset.
  delay(1000);
  telescope.attach(11); //enable telescope
  telescope.write(100, 7, true); //slowly move telescope up
  telescope.detach(); //disable telescope
  delay(1000);

  spin_right(80);
  delay(6000); //we just turn right (which is actually left)
  stop_spin();

  telescope.attach(11); //enable telescope
  telescope.write(5, 7, true); //slowly lower the telescope
  telescope.detach(); //disable telescope
  delay(1000);
  door.attach(DOOR_PIN);  //enable door
  door.write(170, 7, true); //slowly close the door
  door.detach(); //disable door
  
  isCalibrated = false;
  spin_left(80); //start moving back to the initial position
  while (!isCalibrated) { //a smaller copy of the main calibration function, essentially waits for the limit switch to be pressd to then spin a bit more to the initial position
    if (limitSwitch.pressed()) {
      isCalibrated = true;
      stop_spin();
      spin_left(80);
      delay(4000); //this one may need tinkering so it gets back to the actual initial position and doesn't drift
      stop_spin();
      break; //yeah I use break, what are you going to do about it
    }
  }
  stop_spin();
  lightRed(); //light up red again, go into cooldown for 120s
  delay(120000); //big number
  lightGreen(); //cooldown over, sequence over, back to loop() to wait for another button press
}

void setup() {
  lightRed(); //light up the button red for the duration of the calibration sequence
  limitSwitch.begin(); //start up the limit switch button
  button.begin(); //start up the external button

  pinMode(RIGHT_MOTOR_PIN, OUTPUT); //self-explanatory, select pin modes for the 4 pins we use for motor control
  pinMode(LEFT_MOTOR_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT); //and the green and red LED signals going to the external button
  pinMode(RED_PIN, OUTPUT);

  calibrate(); //initial calibration after a new start-up
  lightGreen(); //light up green to signify that we're ready to work
}

void loop() {
  if (button.pressed()) {
      currentTime = millis(); //when the button is initially pressed, it takes in the time in millis when it was pressed
  }
  if (button.released()) {
    if ((millis() - currentTime >= 200)) {
      sequence(); //then it compares it to a "currentTime" value and checks if the button was pressed for longer than 200 milliseconds, to ensure protection from random slips and whatever
      }
    }
  }
