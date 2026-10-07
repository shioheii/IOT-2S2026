#include <ESP32Servo.h>

#define RES 4
#define SER 25

Servo servo;

void setup() {
  // put your setup code here, to run once:
  pinMode(RES, INPUT);
  servo.attach(SER);
  Serial.begin(9600);
}

int angulo;

void loop() {
  // put your main code here, to run repeatedly:
  angulo = map(analogRead(RES), 1920, 4095, 0, 180);

  Serial.println(angulo);
  servo.write(angulo);

  delay(500);
}
