#include <Arduino.h>
#include <Servo.h>

Servo servoX;
//Servo servoY;

void setup() {
  Serial.begin(9600);
  servoX.attach(9);
  //servoY.attach(10);
}

void loop() {
  if (Serial.available()){
    String input = Serial.readStringUntil('\n');
    int comma = input.indexOf(',');
    if (comma > 0) {
      int x = input.substring(0, comma).toInt();
      //int y = input.substring(0, comma).toInt();

      int servoXPos = map(x, 0, 1280, 0, 180);
      //int servoYPos = map(y, 0, 720, 0, 180);

      servoX.write(servoXPos);
      // servoY.write(servoYPos);
    }
  }
}
