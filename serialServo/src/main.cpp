#include <Arduino.h>
#include <Servo.h>

#define NOTE_AS4 466
#define NOTE_CS5 554
#define NOTE_DS5 622

void playJingle();
void alarmAndFlash();
void smoothMove(int fromAngle, int toAngle);
int readX();

const int startBtnPin = 2;
const int servoPin = 9;
const int buzzerPin = 6;
const int laserPin = 7;
const int analogXpin = A0;

const int moveThreshold = 20;
const unsigned long redLightTime = 4000;
const int angleBack = 180;
const int angleFront = 0;
const int minX = 0;
const int maxX = 1023;

enum State { WAITING, GREEN_LIGHT, RED_LIGHT, CELEBRATING };
State state = WAITING;

Servo head;
unsigned long stateTimer = 0;
int lastX = 0;
int currentAngle = angleBack;

const float songSpeed = 1.0;
const int notes[] = {
  NOTE_AS4, NOTE_DS5, NOTE_DS5, 0,
  NOTE_DS5, 0,
  NOTE_CS5, 0,
  NOTE_DS5, NOTE_DS5, NOTE_AS4, NOTE_AS4, NOTE_CS5
};
const int durations[] = {
  300, 300, 300, 300,
  300, 300,
  250, 250,
  300, 300, 300, 300, 300
};
const int melodyLength = sizeof(notes) / sizeof(int);

void setup() {
  pinMode(startBtnPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(laserPin, OUTPUT);
  head.attach(servoPin);
  head.write(currentAngle);
  Serial.begin(9600);
  digitalWrite(laserPin, LOW);
}

void loop() {
  switch (state) {
    case WAITING:
      if (digitalRead(startBtnPin) == LOW) {
        playJingle();
        smoothMove(currentAngle, angleBack);
        state = GREEN_LIGHT;
      }
      break;

    case GREEN_LIGHT:
      smoothMove(currentAngle, angleFront);
      stateTimer = millis();
      state = RED_LIGHT;
      break;

    case RED_LIGHT:
      lastX = readX();
      stateTimer = millis();
      state = CELEBRATING;
      break;

    case CELEBRATING: {
      int currentX = readX();
      if (abs(currentX - lastX) > moveThreshold) {
        int cheaterX = constrain(currentX, minX, maxX);
        int targetAngle = map(cheaterX, minX, maxX, 0, 180);
        smoothMove(currentAngle, targetAngle);
        alarmAndFlash();
        smoothMove(currentAngle, angleBack);
        state = WAITING;
      } else if (millis() - stateTimer >= redLightTime) {
        smoothMove(currentAngle, angleBack);
        state = WAITING;
      }
      lastX = currentX;
    } break;
  }
}

int readX() {
  if (Serial.available()) {
    int x = Serial.parseInt();
    while (Serial.available() && Serial.read() != '\n');
    return constrain(x, minX, maxX);
  }
  return lastX;
}

void playJingle() {
  for (int i = 0; i < melodyLength; i++) {
    int note = notes[i];
    float duration = durations[i] / songSpeed;
    if (note != 0) {
      tone(buzzerPin, note, duration);
    } else {
      noTone(buzzerPin);
    }
    delay(duration * 1.3);
  }
  noTone(buzzerPin);
}

void alarmAndFlash() {
  unsigned long start = millis();
  while (millis() - start < 3000) {
    tone(buzzerPin, 1000);
    digitalWrite(laserPin, HIGH);
    delay(200);
    noTone(buzzerPin);
    digitalWrite(laserPin, LOW);
    delay(200);
  }
  noTone(buzzerPin);
  digitalWrite(laserPin, LOW);
}

void smoothMove(int fromAngle, int toAngle) {
  int step = (toAngle > fromAngle) ? 1 : -1;
  for (int a = fromAngle; a != toAngle; a += step) {
    head.write(a);
    delay(15);
  }
  head.write(toAngle);
  currentAngle = toAngle;
  delay(500);
}
