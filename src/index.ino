#include <Time.h>
#include <Servo.h>
#include "RTClib.h"

RTC_DS3231 RTC;

Servo servo;

const int potPin = 0;
const int servoPin = 9;
const int pushButtonPin = 2;
const int stateLedPin = 13;
const int stopLedPin = 12;

const int bedHour = 22;
const int wakeUpHour = 4;

int potValue;
int state = HIGH;
int reading;
int previous = LOW;

long currentTime = 0;
const int debounce = 200;

void setup() {
  Serial.begin(9600);

  servo.attach(servoPin);

  pinMode(pushButtonPin, INPUT);
  pinMode(stateLedPin, OUTPUT);
  pinMode(stopLedPin, OUTPUT);

  RTC.begin();
  DateTime dt = DateTime(__DATE__, __TIME__);
  RTC.adjust(dt);
}

void loop() {
  DateTime now = RTC.now();

  reading = digitalRead(pushButtonPin);

  if (reading == HIGH && previous == LOW && millis() - currentTime > debounce) {
    if (state == HIGH) {
      state = LOW;
    } else {
      state = HIGH;
    }

    currentTime = millis();    
  }

  digitalWrite(stateLedPin, state);

  previous = reading;

  potValue = analogRead(potPin);
  potValue = map(potValue, 0, 1023, 0, 180);

  if (state == HIGH && (now.hour() >= bedHour || now.hour() < wakeUpHour)) {
    // Coupure la nuit
    servo.write(0);
    digitalWrite(stopLedPin, HIGH);
  } else {
    // Réglage manuel ou de jour
    servo.write(potValue);
    digitalWrite(stopLedPin, LOW);
  }

  delay(10);
}
