// ===============================================
// 아두이노 서보 모터 실험 코드
// - EXPERIMENT 값을 1 또는 2로 바꾸어 실험 선택
// - millis()를 활용해 지정 시간 동안 부드러운 속도로 회전
// - Serial 출력으로 경과 시간 및 각도 확인 가능
// ===============================================

#include <Servo.h>
#define PIN_SERVO 10        // 서보모터 제어 핀

// ===== 현재 실행할 실험 선택 =====
#define EXPERIMENT 1        // 1 = 실험 1, 2 = 실험 2
// ==============================

Servo servo;                // 서보 객체 생성

// 실험별 설정값 변수
int TARGET_ANGLE;           // 목표 각도 (0° → 이 값)
unsigned long MOVE_TIME;    // 회전 시간 (msec 단위)

void setup() {
  Serial.begin(9600);       // 시리얼 모니터 시작
  servo.attach(PIN_SERVO);  // 서보모터 연결
  servo.write(0);           // 서보 초기 위치: 0°
  delay(1000);

  // === 실험 1: 60초 동안 180° 회전 ===
  // === 실험 2: 300초 동안 90° 회전 ===
  // EXPERIMENT 값에 따라 각도/시간 세팅
#if EXPERIMENT == 1
  TARGET_ANGLE = 180;
  MOVE_TIME = 60000;        // 60초 (60000ms)
  Serial.println("실험 1 시작: 60초 동안 180° 이동");

#elif EXPERIMENT == 2
  TARGET_ANGLE = 90;
  MOVE_TIME = 300000;       // 300초 (5분 = 300000ms)
  Serial.println("실험 2 시작: 300초 동안 90° 이동");

#else
  #error "EXPERIMENT 값은 1 또는 2만 가능"
#endif

  delay(500);               // 실험 전 준비 시간
  runServoExperiment(0, TARGET_ANGLE, MOVE_TIME);

  Serial.println("실험 완료 (마지막 각도에서 멈춤)");
}

void loop() {
  // setup()에서 실험이 완료되므로 loop는 비워둠
}

// ===============================================
// runServoExperiment 함수
// - startAng: 시작 각도
// - endAng: 목표 각도
// - duration: 총 이동 시간 (millis 단위)
// - millis() 기반으로 일정 시간 동안 서서히 이동
// ===============================================
void runServoExperiment(int startAng, int endAng, unsigned long duration) {
  unsigned long startTime = millis();   // 실험 시작 시간 저장
  unsigned long lastPrint = 0;          // 출력 주기 제어용
  unsigned long currentTime = millis(); // 현재 시간 변수

  // 지정된 시간 동안 반복 실행
  while (currentTime - startTime <= duration) {
    currentTime = millis();  // 매 반복마다 현재 시간 갱신

    // 진행도 계산 (0.0 ~ 1.0)
    float progress = (float)(currentTime - startTime) / duration;

    // 현재 각도 계산: 선형 보간
    int currentAng = startAng + (endAng - startAng) * progress;
    servo.write(currentAng);   // 계산된 각도를 서보에 전송

    delay(5);   // 너무 빠른 전송 방지 (부드러운 회전)
    
    // 0.5초마다 실험 진행 정보 출력
    if (currentTime - lastPrint >= 500) {
      Serial.print("경과 시간: ");
      Serial.print((currentTime - startTime) / 1000);
      Serial.print("초 / 현재 각도: ");
      Serial.println(currentAng);
      lastPrint = currentTime;
    }
  }
}
