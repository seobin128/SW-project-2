
#include <Servo.h>
#include <math.h>

#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 10

#define SOUND_VELOCITY 0.034
#define DETECT_THRESHOLD 20
#define SERVO_MIN 0
#define SERVO_MAX 90

Servo servo;
bool carDetected = false;
unsigned long detectStart = 0;
unsigned long releaseStart = 0;

float sigmoid(float x){
  return 1.0 / (1.0 + exp(-x));
}

float getDistance(){
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000);
  float distance = duration * SOUND_VELOCITY / 2;
  return distance;
}

void moveServoSmooth(int startAngle, int endAngle){
  int steps = 50;
  for (int i = 0; i <= steps; i++){
    float t = (float)i/ steps;
    float factor = sigmoid(12 * (t-0.5));
    int angle = startAngle + (endAngle - startAngle) * factor;
    servo.write(angle);
    delay(20);
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  servo.attach(PIN_SERVO);
  servo.write(SERVO_MIN);
  Serial.println("Sigmoid-controlled Parking Gate Initialized");

}

void loop() {
  // put your main code here, to run repeatedly:
  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (distance > 0 && distance < DETECT_THRESHOLD){
    if (detectStart == 0) detectStart = millis();
    releaseStart = 0;

    if (!carDetected && millis()- detectStart > 800){
    carDetected = true;
    Serial.println("Car detected -> Bar UP");
    moveServoSmooth(SERVO_MIN, SERVO_MAX);
    }
  }
  else {
    detectStart = 0;

    if (carDetected && releaseStart == 0) {
      releaseStart = millis();
    }

    if (carDetected && releaseStart >0 && millis()- releaseStart > 1000){
      carDetected = false;
      Serial.println("Car gone -> Bar DOWN");
      moveServoSmooth(SERVO_MAX, SERVO_MIN);
    }
  }
  delay(200);
}
