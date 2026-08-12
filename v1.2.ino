#include <IRremote.hpp>
#include <LiquidCrystal.h>
#include <Servo.h>

unsigned long lastServoMove = 0;
const unsigned long servoInterval = 30;
int sweepDirection = 1;
bool systemKilled = false;
Servo myservo;
int pos=0;
LiquidCrystal lcd(12,11,7,8,9,4);
const int buttonPin = 2;
const int led1pin = A1;
const int led2pin = A2;
const int led3pin = A3;
const int buzzerPin = 10;
unsigned long lastToggle = 0;
bool buzzerState = false;
int buzzerFreq = 0; // fixes the timer bug
//starts disarmed
bool armed = false;
bool lastButtonReading = HIGH;
bool debouncedButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
const int irPin = A0;
const unsigned long KILL_CODE  = 0xB847FF00UL;
const unsigned long RESET_CODE = 0xBA45FF00UL;
const unsigned long ARM_CODE    = 0xF30CFF00UL; // Button 1
const unsigned long DISARM_CODE = 0xE718FF00UL; // Button 2
const int joyXpin = A4;
const int joySwPin= 13;
bool sweepmode = true;
//joystick debounce button
bool lastJoySwReading = HIGH;
bool debouncedJoyswState = HIGH;
unsigned long lastJoyDebounceTime = 0;
// sweep timing
unsigned long lastSweepStep = 0;
const unsigned long sweepStepDelay = 30;
//distance ranges(severity)
const float NEAR_CM = 10.0;
const float MID_CM = 20.0;
const float FAR_CM = 40;
const float NUN_CM = 60; //0 leds on

void setup() {
  Serial.begin(9600);
  pinMode(6,OUTPUT);
  pinMode(5,INPUT);
  myservo.attach(3);
  pinMode(buttonPin,INPUT_PULLUP);
  pinMode(led1pin, OUTPUT);
  pinMode(led2pin, OUTPUT);
  pinMode(led3pin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  allOff();
  pinMode(joySwPin, INPUT_PULLUP);
  IrReceiver.begin(irPin);
  lcd.begin(16,2); // 16x2 display
  lcd.print("Starting:");
}
void updateBuzzer() {
  if (buzzerFreq == 0) {
    digitalWrite(buzzerPin, LOW);
    return;
  }
  unsigned long halfPeriod = 500000UL / buzzerFreq; // microseconds per half-cycle
  if (micros() - lastToggle >= halfPeriod) {
    buzzerState = !buzzerState;
    digitalWrite(buzzerPin, buzzerState);
    lastToggle = micros();
  }
}
void checkButton(){
  bool reading = digitalRead(buttonPin);
  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }
  if ((millis()- lastDebounceTime)> debounceDelay){
    if (reading != debouncedButtonState){
      debouncedButtonState = reading;
      if (debouncedButtonState == LOW){
        armed = !armed; // toggle part
        Serial.println(armed ? "ARMED" : "DISARMED");
        if (!armed){
          allOff();
        }
      }
    }
  }
  lastButtonReading = reading;
}
void killSystem(){
  systemKilled = true;
  armed = false;
  allOff();
  buzzerFreq = 0;
  myservo.detach();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("System killed");
  lcd.setCursor(0,1);
  lcd.print("Press 1 to reset");
  Serial.println("System killed");
}
void restartSystem(){
  systemKilled = false;
  myservo.attach(3);
  pos = 90;
  myservo.write(pos);
  lcd.clear();
  lcd.print("System Restarted");
  delay(500);
  Serial.println("System Restarted");
}
void checkIrRemote(){
  if (IrReceiver.decode()){
    unsigned long code = IrReceiver.decodedIRData.decodedRawData;
    if(code == KILL_CODE && !systemKilled){
      killSystem();
    }
    else if(code == RESET_CODE && systemKilled){
      restartSystem();
    }
    if(code == ARM_CODE && ! armed){
      armed = true;
      Serial.println("ARMED (remote)");
    }
    else if (code == DISARM_CODE && armed){
      armed = false;
      Serial.println("DISARMED(remote)");
      allOff();
    }
    IrReceiver.resume();
  }
}
void checkJoystickButton() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(joySwPin);
  // Button has just been pressed
  if (lastState == HIGH && currentState == LOW) {
    sweepmode = !sweepmode;
    Serial.print("MODE CHANGED: ");
    Serial.println(sweepmode ? "SWEEP" : "JOYSTICK");
    lcd.setCursor(11, 0);
    lcd.print(sweepmode ? "SWEEP" : "JOY  ");
    delay(200); // debounce
  }

  lastState = currentState;
}
void runJoystickControl(){
  int rawX = analogRead(joyXpin);
  int newPos= map(rawX,0,1023,0,180);
  newPos=constrain(newPos,0,180);
  if (newPos != pos){
    pos = newPos;
    myservo.write(pos);
  }
}
void calcDist(){
    //Send a pulse from the trig pin(11)
  if(systemKilled){
    return;
  }
  digitalWrite(6,LOW);
  delayMicroseconds(2);
  digitalWrite(6,HIGH);
  delayMicroseconds(10);
  digitalWrite(6,LOW);
  //Measure how long it takes the echo to return
  long duration = pulseIn(5,HIGH, 25000);
  //Calc Distance in cm
  float distanceCm= duration* 0.0343/2; // since speed of sound is 343m/s its 0.0343 cm/ microsecond then div by 2 since its accounting duration to go to object and back
  Serial.print("Distance is: ");
  Serial.print(distanceCm);
  Serial.println("cm");
  Serial.println(armed?"ARMED":"DISARMED");
  lcd.clear();  // print distance
  lcd.setCursor(0,0);
  lcd.print(distanceCm);
  lcd.print("cm");
  lcd.setCursor(11,0);
  lcd.print(sweepmode ? "SWEEP":"JOY");
  lcd.setCursor(0,1); // print the angle
  lcd.print(pos);
  lcd.print("degrees  ");
  lcd.print(armed ? "ARM": "OFF");
  updateAlerts(distanceCm);
  updateBuzzer();
}
void updateAlerts(float distanceCm){
  if (!armed){
    allOff();
    return;
  }
  if (distanceCm < NEAR_CM) {
    digitalWrite(led1pin, HIGH);
    digitalWrite(led2pin, HIGH);
    digitalWrite(led3pin, HIGH);
    buzzerFreq = 2000; 
  }
  else if (distanceCm < MID_CM ){
    digitalWrite(led1pin, HIGH);
    digitalWrite(led2pin, HIGH);
    digitalWrite(led3pin, LOW);
    buzzerFreq = 800;
  }
  else if (distanceCm < FAR_CM ){
    digitalWrite(led1pin, HIGH);
    digitalWrite(led2pin, LOW);
    digitalWrite(led3pin, LOW);
    buzzerFreq = 100;
  }
  else if (distanceCm > FAR_CM ){
    allOff();
  }
}
void allOff(){
    digitalWrite(led1pin, LOW);
    digitalWrite(led2pin, LOW);
    digitalWrite(led3pin, LOW);
    buzzerFreq = 0;
}
void updateServo(){
  if(systemKilled){
    return;
  }
  if(sweepmode){
    if(millis()- lastServoMove >= servoInterval){
      lastServoMove = millis();
      pos+= sweepDirection;
      if(pos>=180){
        pos=180;
        sweepDirection=-1;
      }
      if(pos<=0){
        pos = 0;
        sweepDirection = 1;
      }
    }
    myservo.write(pos);
  }
  else{
    runJoystickControl();
  }
}
void loop() {
 checkIrRemote();
 if (systemKilled){
  return;
 }
 checkButton();
 checkJoystickButton();
 updateServo();
 static unsigned long lastDistance = 0;
 if(millis()-lastDistance>=100){
  lastDistance=millis();
  calcDist();
 }
}
