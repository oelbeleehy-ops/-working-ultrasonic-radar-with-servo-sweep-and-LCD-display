#include <LiquidCrystal.h>
#include <Servo.h>

Servo myservo;
int pos=0;
LiquidCrystal lcd(12,11,7,8,9,4);

void setup() {
  Serial.begin(9600);
  pinMode(6,OUTPUT);
  pinMode(5,INPUT);
  myservo.attach(3);
  lcd.begin(16,2); // 16x2 display
  lcd.print("Starting:");
}

void calcDist(){
    //Send a pulse from the trig pin(11)
  digitalWrite(6,LOW);
  delayMicroseconds(2);
  digitalWrite(6,HIGH);
  delayMicroseconds(10);
  digitalWrite(6,LOW);
  //Measure how long it takes the echo to return
  long duration = pulseIn(5,HIGH);
  //Calc Distance in cm
  float distanceCm= duration* 0.0343/2; // since speed of sound is 343m/s its 0.0343 cm/ microsecond then div by 2 since its accounting duration to go to object and back
  Serial.print("Distance is: ");
  Serial.print(distanceCm);
  Serial.println("cm");
  lcd.clear();  // print distance
  lcd.setCursor(0,0);
  lcd.print(distanceCm);
  lcd.print("cm");
  lcd.setCursor(0,1); // print the angle
  lcd.print(pos);
  lcd.print("degree");
}

void loop() {
 for (pos = 0; pos <= 180; pos += 1) {
  myservo.write(pos);
  calcDist();      // take a reading at every step
  delay(30);
  }
  for (pos = 180; pos >= 0; pos -= 1) {
    myservo.write(pos);
    calcDist();
  delay(100);
  }
}
