// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100     // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300     // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

#define _EMA_ALPHA 0.5    // EMA weight of new sample (range: 0 to 1)
                          // Setting EMA to 1 effectively disables EMA filter.
#define NUM_SAMPLES 30
// global variables
unsigned long last_sampling_time;   // unit: msec
float samples[NUM_SAMPLES];
int index = 0;
bool filled = false;

float medianFilter(){
  if (!filled && index < 2 ) return samples[0]; // 샘플이 충분히 쌓이지 않았다면 첫 샘플 반환
  int count = filled ? NUM_SAMPLES : index; // 버퍼가 찼는지 여부에 따라 샘플 개수 설정
  float temp[NUM_SAMPLES]; // 정렬용 임시 배열 생성

  for (int i = 0; i < count; i++) temp[i] = samples[i]; // 원본 샘플 복사

  for (int i = 0; i < count; i++){
    float key = temp[i];  // 현재 비교할 값
    int j = i -1;   // 이전 인덱스부터 비교 시작
    while (j >= 0 && temp[j]>key) {   // 앞쪽 값이 더 크면 한 칸씩 뒤로 밀기 
      temp[j+1] = temp[j];
      j--;
    }
    temp[j+1] = key;  // 올바른 위치에 key 삽입
  }
  return temp[count/2];   //중위수 반환
}

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  pinMode(PIN_ECHO,INPUT);
  digitalWrite(PIN_TRIG, LOW);

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  

  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  float dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);

  if (dist_raw > 0 && dist_raw < 4000) {    // 유효 범위의 측정값만 저장
    samples[index++] = dist_raw;
    if (index >= NUM_SAMPLES){    //버퍼가 가득 차면
      index = 0;    // 인덱스 초기화
      filled = true;    // 버퍼 상태 갱신
    }
  }

  float dist_median = medianFilter();   // 중위수 계산 함수 호출
  
  // output the distance to the serial port
  Serial.print("Min:");   Serial.print(_DIST_MIN);
  Serial.print(",raw:"); Serial.print(min(dist_raw, _DIST_MAX + 100));
  Serial.print(",median:"); Serial.print(dist_median);
  Serial.print(",Max:");  Serial.print(_DIST_MAX);
  Serial.println("");

  // do something here
  if ((dist_raw < _DIST_MIN) || (dist_raw > _DIST_MAX))
    digitalWrite(PIN_LED, 1);       // LED OFF
  else
    digitalWrite(PIN_LED, 0);       // LED ON

  // update last sampling time
  last_sampling_time += INTERVAL;   // 다음 샘플링 갱신
}

// get a distance reading from USS. return value is in millimeter.
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);
  
  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE; // unit: mm

  // Pulse duration to distance conversion example (target distance = 17.3m)
  // - pulseIn(ECHO, HIGH, timeout) returns microseconds (음파의 왕복 시간)
  // - 편도 거리 = (pulseIn() / 1,000,000) * SND_VEL / 2 (미터 단위)
  //   mm 단위로 하려면 * 1,000이 필요 ==>  SCALE = 0.001 * 0.5 * SND_VEL
  //
  // - 예, pusseIn()이 100,000 이면 (= 0.1초, 왕복 거리 34.6m)
  //        = 100,000 micro*sec * 0.001 milli/micro * 0.5 * 346 meter/sec
  //        = 100,000 * 0.001 * 0.5 * 346
  //        = 17,300 mm  ==> 17.3m
}
