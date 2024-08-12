//command format: e0-1 f255 p0
//pump: 70-255
//fan: 30-255
String incomingString;

byte fan = 0;
byte pump = 0;
bool evap = 0;

#define RELAY_PIN 8
#define FAN_PIN 9
#define PUMP_PIN 10

void setup() {

Serial.begin(9600);
pinMode(RELAY_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(PUMP_PIN, OUTPUT);

}

void loop() {

if (Serial.available() > 0) {

incomingString = Serial.readStringUntil(' ');

  if (incomingString.startsWith("e")) {
    incomingString.remove(0,1);
    evap = incomingString.toInt();
  }
  if (incomingString.startsWith("f")) {
    incomingString.remove(0,1);
    fan = incomingString.toInt();
  }
  if (incomingString.startsWith("p")) {
    incomingString.remove(0,1);
    pump = incomingString.toInt();
  }

}

  if(evap) {
    digitalWrite(RELAY_PIN, LOW);
  }
  else {
    digitalWrite(RELAY_PIN, HIGH);
  }

  analogWrite(FAN_PIN, fan);
  analogWrite(PUMP_PIN, pump);  
}
