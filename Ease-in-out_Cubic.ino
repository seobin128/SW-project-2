#include <Servo.h>
#include <math.h>

// 핀 설정
#define PIN_TRIG 12
#define PIN_ECHO 13
#define PIN_SERVO 10

// 상수 정의
#define SOUND_VELOCITY 0.034  // cm/µs
#define DETECT_THRESHOLD 20   // 차량 감지 거리 (cm)
#define SERVO_MIN 0
#define SERVO_MAX 90

Servo servo;
bool carDetected = false;
unsigned long detectStart = 0;    // 차량이 감지된 시점
unsigned long releaseStart = 0;   // 차량이 사라진 시점

// Ease-in-out Cubic 함수
float smoothFunc(float x) {
  return (x < 0.5) ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
}

// 초음파 거리 측정 함수
float getDistance() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  long duration = pulseIn(PIN_ECHO, HIGH, 30000); // 30ms timeout
  float distance = duration * SOUND_VELOCITY / 2; // 왕복 → 절반
  return distance;
}

// 서보모터 부드럽게 이동 (Ease-in-out Cubic)
void moveServoSmooth(int startAngle, int endAngle) {
  int steps = 30; // 단계 수 (값이 높을수록 부드러움)
  for (int i = 0; i <= steps; i++) {
    float t = (float)i / steps;   
    float factor = smoothFunc(t);
    int angle = startAngle + (endAngle - startAngle) * factor;
    servo.write(angle);
    delay(15); // 움직임 속도 (값 증가 시 느려짐)
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  servo.attach(PIN_SERVO);
  servo.write(SERVO_MIN);
  Serial.println("Parking Gate ");
}

void loop() {
  float distance = getDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  // 차량 감지 구간
  if (distance > 0 && distance < DETECT_THRESHOLD) {
    if (detectStart == 0) detectStart = millis();  // 감지 시작 시간 기록
    releaseStart = 0;  // 해제 타이머 초기화

    // 차량이 일정 시간 이상 머물렀을 때만 차단봉 올림
    if (!carDetected && millis() - detectStart > 800) {
      carDetected = true;
      Serial.println("Car detected -> Bar UP");
      moveServoSmooth(SERVO_MIN, SERVO_MAX);
    }
  } 
  // 차량이 사라진 경우
  else {
    detectStart = 0; // 감지 타이머 초기화

    if (carDetected && releaseStart == 0) {
      releaseStart = millis(); // 차량 사라진 시간 기록
    }

    // 차량이 일정 시간 이상 사라졌을 때 차단봉 하강
    if (carDetected && releaseStart > 0 && millis() - releaseStart > 1000) {
      carDetected = false;
      Serial.println("Car gone -> Bar DOWN");
      moveServoSmooth(SERVO_MAX, SERVO_MIN);
    }
  }

  delay(200);
}
