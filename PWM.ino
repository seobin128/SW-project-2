
#define LED_PIN 7

double period_us = 1000;
int duty_percent = 0;
unsigned long cycle_start_time;
unsigned long last_pwm_time;

void set_period(double period) {
  period_us = period;
}

void set_duty(int duty) {
  if (duty < 0) duty = 0;
  if (duty > 100) duty = 100;
  duty_percent = duty;
}

void setup(){
  pinMode(LED_PIN, OUTPUT);
  cycle_start_time = micros();
  last_pwm_time = micros();
}

void loop(){
  double periods[] = {10000, 1000, 100};

  for (int p = 0; p < 3; p++) {
    set_period(periods[p]);

    unsigned long phase_start = micros();
    unsigned long one_sec = 1000000;  // 1초

    while (micros() - phase_start < one_sec) {
      unsigned long elapsed = micros() - phase_start;

      // 0→100→0 사이클 계산 (2차 함수)
      double progress = (double)elapsed / one_sec;  // 0~1
      double duty;

      if (progress <= 0.5) {
        // 0 → 100: 2차 함수
        double x = progress * 2;  // 0~1
        duty = 100 * x * x;
      } else {
        // 100 → 0: 역2차 함수
        double x = (progress - 0.5) * 2;  // 0~1
        duty = 100 * (1 - x * x);
      }

      set_duty((int)duty);

      // PWM 출력 (실시간)
      pwm_output();
    }
  }
}

void pwm_output() {
  static bool led_state = LOW;
  static unsigned long pwm_start = 0;

  unsigned long current_time = micros();
  unsigned long elapsed = current_time - pwm_start;

  double high_time = period_us * (duty_percent / 100.0);

  if (led_state == LOW && elapsed >= period_us) {
    // 새 PWM 사이클 시작
    digitalWrite(LED_PIN, HIGH);
    led_state = HIGH;
    pwm_start = current_time;
  } else if (led_state == HIGH && elapsed >= high_time) {
    // HIGH → LOW 전환
    digitalWrite(LED_PIN, LOW);
    led_state = LOW;

  }
}
